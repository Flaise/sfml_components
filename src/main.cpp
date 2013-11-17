#include <SFML/Graphics.hpp>

#include <iostream>
#include <string>
#include <sstream>

#pragma GCC diagnostic ignored "-Wunused-local-typedefs" // temporarily disable warnings
	#include <boost/math/constants/constants.hpp>
	#include <boost/foreach.hpp>
	#define bforeach BOOST_FOREACH
	#define bforeach_r BOOST_REVERSE_FOREACH
#pragma GCC diagnostic pop // reenable warnings

#include "sparsearray3.hpp"
#include "framerate.hpp"

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
		(*it).currValue = (*it).baseValue;

	for(auto it = interpolations.begin(); it != interpolations.end(); it++) {
		(*it).elapsed += dt;
		if((*it).elapsed >= (*it).duration) {
			(*(*it).interpoland).currValue += (*it).destDelta;
			(*(*it).interpoland).baseValue += (*it).destDelta;
			interpolations.remove(it);
		}
		else {
			(*(*it).interpoland).currValue += GetDelta(*it);
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
		text.setFont(*((*it).font));
		text.setString((*it).message);
		text.setPosition((*it).x, (*it).y);
		window->draw(text);
	}
}

//constexpr sf::Time one_second = sf::milliseconds(1000);

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "/\\/\\/\\/");
    window.setKeyRepeatEnabled(false);


    sf::Texture texture;
    if(!texture.loadFromFile("src/assets/block.png")) {
        return 1;
    }
    sf::Font font;
	if(!font.loadFromFile("src/assets/whiterabbit.ttf")) {
		return 2;
	}


    sf::Sprite sprite;
    sprite.setTexture(texture);

    auto x = MakeInterpoland(0);
    auto y = MakeInterpoland(200);

    InterpolateTo(x, 100, sf::seconds(1), Tween::SINE);

    auto framerateText = MakeDisplayText(&font, "", 0, 0, 0, 0);
    auto interpolandCountText = MakeDisplayText(&font, "", 0, 15, 0, 0);
    auto interpolationCountText = MakeDisplayText(&font, "", 0, 30, 0, 0);
    auto textCountText = MakeDisplayText(&font, "", 0, 45, 0, 0);


    sf::Clock frame;
    Framerate<sf::Time> framerate(sf::seconds(1));
    //Framerate<sf::Time, one_second> framerate;

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

		sprite.setPosition((*x).currValue, (*y).currValue);
		window.draw(sprite);

		(*framerateText).message = framerate.current >= 0? "FPS:             " + to_string(framerate.current): "";
		(*interpolandCountText).message =                  "interpolands:    " + to_string(interpolands.size());
		(*interpolationCountText).message =                "interpolantions: " + to_string(interpolations.size());
		(*textCountText).message =                         "texts:           " + to_string(texts.size());
		UpdateTexts(&window);


		window.display();
    }

    return 0;
}
