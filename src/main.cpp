#include <SFML/Graphics.hpp>

#include <iostream>
#include <string>
#include <sstream>
#include <cstdlib> // for rand()

#pragma GCC diagnostic ignored "-Wunused-local-typedefs" // temporarily disable warnings
	#include <boost/math/constants/constants.hpp>
	#include <boost/foreach.hpp>
	#define bforeach BOOST_FOREACH
	#define bforeach_r BOOST_REVERSE_FOREACH
#pragma GCC diagnostic pop // reenable warnings

#include "sparsearray3.hpp"
#include "framerate.hpp"
#include "assets/block.h"
#include "assets/whiterabbit.h"

// no one's bothered to implement the std:: version of this in MinGW
// something to do with C99, evidently
std::string to_string(int i) {
	std::stringstream s;
	s << i;
	return s.str();
}


struct Interpoland {
	float baseValue;
	float currValue;
	float destValue;

	Interpoland() {}
	Interpoland(float value): baseValue(value), currValue(value), destValue(value) {}
};
using InterpolandHandle = SparseArray3<Interpoland, 100>::Handle;
using SAInterpoland = SparseArray3<Interpoland, 100>;

using TweenFunc = std::function<float(float)>;

struct Interpolation {
	InterpolandHandle interpoland;
	float destDelta;
	sf::Time duration;
	sf::Time elapsed;
	TweenFunc scaling;

	Interpolation() {}
	Interpolation(InterpolandHandle interpoland, float delta, sf::Time duration, TweenFunc func):
		interpoland(interpoland), destDelta(delta), duration(duration), elapsed(sf::milliseconds(0)), scaling(func) {}
};
using SAInterpolation = SparseArray3<Interpolation, 200>;


SAInterpoland interpolands;
SAInterpolation interpolations;

auto MakeInterpoland(float value) {
	return interpolands.add(Interpoland(value));
}


namespace Tween {
	const float pi = boost::math::constants::pi<float>();
	const float pi_2 = pi / 2;

	float Linear(float progress) { return progress; }
	float DelayBefore(float progress) { return 0; }
	float DelayAfter(float progress) { return 1; }
	float SINE_INOUT(float progress) { return sinf(2 * (progress * pi_2) - pi_2) / 2 + .5f; }
	float SINE(float progress) { return sinf(progress * pi_2); }
}


auto Interpolate(InterpolandHandle interpoland, float delta, sf::Time duration, TweenFunc func) {
	return interpolations.add(Interpolation(interpoland, delta, duration, func));
}
auto InterpolateTo(InterpolandHandle interpoland, float dest, sf::Time duration, TweenFunc func) {
	return Interpolate(interpoland, dest - (*interpoland).destValue, duration, func);
}

float GetDelta(Interpolation& interpolation) {
	return interpolation.destDelta * interpolation.scaling(interpolation.elapsed.asSeconds() / interpolation.duration.asSeconds());
}

void UpdateInterpolations(sf::Time dt) {
	for(auto it = interpolands.begin(); it != interpolands.end(); it++)
		it->currValue = it->baseValue;

	for(auto it = interpolations.begin(); it != interpolations.end(); it++) {
		it->elapsed += dt;
		if(it->elapsed >= it->duration) {
			it->interpoland->currValue += it->destDelta;
			it->interpoland->baseValue += it->destDelta;
			interpolations.remove(it);
		}
		else {
			it->interpoland->currValue += GetDelta(*it);
		}
	}
}


struct DisplayText {
	sf::Font* font;
	sf::String message;
	float x;
	float y;
	float ax;
	float ay;

	DisplayText() {}
	DisplayText(sf::Font* font, sf::String message, float x, float y, float ax, float ay):
		font(font), message(message), x(x), y(y), ax(ax), ay(ay) {}
};
using TextHandle = SparseArray3<DisplayText, 100>::Handle;
using SAText = SparseArray3<DisplayText, 100>;

SAText texts;

auto MakeDisplayText(sf::Font* font, sf::String message, float x, float y, float ax, float ay) {
	return texts.add(DisplayText(font, message, x, y, ax, ay));
}

void UpdateTexts(sf::RenderWindow* window) {
    sf::Text text;
	text.setCharacterSize(12); // in pixels, not points
	text.setColor(sf::Color::White);

	for(auto it = texts.begin(); it != texts.end(); it++) {
		text.setFont(*(it->font));
		text.setString(it->message);
		text.setPosition(it->x, it->y);
		window->draw(text);
	}
}



//constexpr sf::Time one_second = sf::milliseconds(1000);

int main() {
	srand(0);

	sf::RenderWindow window(sf::VideoMode(800, 600), "/\\/\\/\\/");
	//sf::Window window(sf::VideoMode(800, 600), "/\\/\\/\\/");
	window.setKeyRepeatEnabled(false);

	//const char* version = (const char*)glGetString(GL_VERSION);
	//std::cout << version << std::endl;


	sf::Texture texture;
	texture.loadFromMemory(block, sizeof(block));

	sf::Font font;
	font.loadFromMemory(whiterabbit, sizeof(whiterabbit));


	sf::Sprite sprite;
	sprite.setTexture(texture);

	auto x = MakeInterpoland(0);
	auto y = MakeInterpoland(200);

	InterpolateTo(x, 100, sf::seconds(1), Tween::SINE);

	auto framerateText = MakeDisplayText(&font, "", 0, window.getSize().y - 60, 0, 0);
	auto interpolandCountText = MakeDisplayText(&font, "", 0, window.getSize().y - 45, 0, 0);
	auto interpolationCountText = MakeDisplayText(&font, "", 0, window.getSize().y - 30, 0, 0);
	auto textCountText = MakeDisplayText(&font, "", 0, window.getSize().y - 15, 0, 0);


	sf::Clock frame;
	Framerate<sf::Time> framerate(sf::seconds(1));
	//Framerate<sf::Time, one_second> framerate;

	sf::RectangleShape debugPanelRect;
	debugPanelRect.setPosition(sf::Vector2f(0, window.getSize().y - 60));
	debugPanelRect.setSize(sf::Vector2f(300, 60));
	debugPanelRect.setFillColor(sf::Color(128, 128, 128, 128));


	sf::VertexArray triangle(sf::Triangles, 3);

	// define the position of the triangle's points
	triangle[0].position = sf::Vector2f(10, 10);
	triangle[1].position = sf::Vector2f(100, 10);
	triangle[2].position = sf::Vector2f(55, 100);

	// define the color of the triangle's points
	triangle[0].color = sf::Color::Red;
	triangle[1].color = sf::Color::Blue;
	triangle[2].color = sf::Color::Green;


	unsigned int fireWidth = 50;
	unsigned int fireHeight = 50;
	unsigned int fireCells = fireWidth * fireHeight;
	unsigned char fireIntensity[fireWidth * fireHeight];

	/*for(int i = 0; i < sizeof(fireIntensity); i++) {
		fireIntensity[i] = 100;
	}
	sf::VertexArray fireVA(sf::Quads, (fireWidth + (fireWidth - 2)) * (fireHeight + (fireHeight - 2)));

	for(int x = 0; x < fireWidth; x++)
		for(int y = 0; y < fireHeight) {
			fireVA[x + y * fireWidth].position = sf::Vector2f(200 + x * 10, 100 + y * 10);
			fireVA[x + y * fireWidth].position = sf::Vector2f(200 + x * 10, 100 + y * 10);
		}*/
	sf::RectangleShape fireRects[fireCells];
	for(int x = 0; x < fireWidth; x++)
		for(int y = 0; y < fireHeight; y++) {
			fireRects[x + y * fireWidth].setSize(sf::Vector2f(2, 2));
			fireRects[x + y * fireWidth].setPosition(sf::Vector2f(200 + x * 2, 100 + y * 2));
			//fireRects[x + y * fireWidth].setFillColor(sf::Color(50, 55, 10));
		}








	sf::VertexArray rrr(sf::TrianglesStrip, 4);

	//for(char* it = fireIntensity; it < fireIntensity + fireCells; it++)


	rrr[0].position = sf::Vector2f(300, 300);
	rrr[1].position = sf::Vector2f(350, 300);
	rrr[2].position = sf::Vector2f(300, 350);
	rrr[3].position = sf::Vector2f(350, 350);

	rrr[0].color = sf::Color::Red;
	rrr[1].color = sf::Color::Blue;
	rrr[2].color = sf::Color::Green;
	rrr[3].color = sf::Color::Yellow;


	sf::Time fireTimer = sf::milliseconds(0);

    while(true) {
        sf::Event event;
        while(window.pollEvent(event)) {
            if(event.type == sf::Event::Closed)
                window.close();
			else if(event.type == sf::Event::Resized) {

			}
			else if(event.type == sf::Event::KeyPressed) {
				if(event.key.code == sf::Keyboard::Key::Up)
					Interpolate(y, -100, sf::seconds(1), Tween::SINE);
				else if(event.key.code == sf::Keyboard::Key::Down)
					Interpolate(y, 100, sf::seconds(1), Tween::SINE);
				else if(event.key.code == sf::Keyboard::Key::Right)
					Interpolate(x, 100, sf::seconds(1), Tween::SINE);
				else if(event.key.code == sf::Keyboard::Key::Left)
					Interpolate(x, -100, sf::seconds(1), Tween::SINE);
			}
        }
        if(!window.isOpen())
			break;


		sf::Time dt = frame.restart();
		framerate.update(dt);

		UpdateInterpolations(dt);


		window.clear();

		sprite.setPosition(x->currValue, y->currValue);
		window.draw(sprite);

		window.draw(debugPanelRect);

		framerateText->message = framerate.current >= 0? "FPS:            " + to_string(framerate.current): "";
		interpolandCountText->message =                  "interpolands:   " + to_string(interpolands.size());
		interpolationCountText->message =                "interpolations: " + to_string(interpolations.size());
		textCountText->message =                         "texts:          " + to_string(texts.size());
		UpdateTexts(&window);

		window.draw(triangle);
		window.draw(rrr);



		fireTimer += dt;
		if(fireTimer > sf::milliseconds(20)) {
			fireTimer -= sf::milliseconds(20);

			for(int x = 0; x < fireWidth; x++)
				for(int y = 0; y < fireHeight; y++) {
					//fireIntensity[x + y * fireHeight] = rand() % 255;
					//continue;

					int average = 0;
					int ct = 0;

					average += fireIntensity[x + y * fireWidth];
					ct++;

					if(x > 0) {
						average += fireIntensity[x - 1 + y * fireWidth];
						ct++;
					}
					if(x < fireWidth - 1) {
						average += fireIntensity[x + 1 + y * fireWidth];
						ct++;
					}
					if(y > 0) {
						average += fireIntensity[x + (y - 1) * fireWidth];
						ct++;
					}
					if(y < fireHeight - 1) {
						average += fireIntensity[x + (y + 1) * fireWidth] * 3;
						ct += 3;
					}
					average /= ct;

					if(y == fireHeight - 1)
						average += rand() % 15;
					else
						average += (rand() % 5 == 0)? (rand() % 60 - 50): 0;
						//average -= rand() % 10;

					if(average > 128)
						average += 2;
					else
						average -= 2;

					if(average > 255)
						average = 255;
					else if(average < 0)
						average = 0;

					fireIntensity[x + y * fireWidth] = average;
				}
		}


		for(int i = 0; i < fireCells; i++)
			fireRects[i].setFillColor(sf::Color(fireIntensity[i], fireIntensity[i] / 2, fireIntensity[i] / 5, fireIntensity[i]));
		for(int i = 0; i < fireCells; i++)
			window.draw(fireRects[i]);

		window.display();
    }

    return 0;
}
