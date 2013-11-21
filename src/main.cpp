#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

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
#include "interpolation.hpp"
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


struct Vertex {
	struct { GLfloat x, y; } position;
	struct { GLfloat r, g, b, a; } color;
};
void DrawVertices(GLenum mode, Vertex* vertices, GLushort* indices, size_t numIndices) {
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glVertexPointer(2, GL_FLOAT, sizeof(Vertex), &vertices->position);
	glColorPointer(4, GL_FLOAT, sizeof(Vertex), &vertices->color);
	glDrawElements(mode, numIndices, GL_UNSIGNED_SHORT, indices);


	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisable(GL_BLEND);
}



struct SpriteVert {
	struct { GLfloat x, y; } position;
	struct { GLfloat u, v; } tex;
};
SpriteVert spriteVerts[] = {
	{ { -.5f, -.5f }, { 0, 0 } },
	{ { .5f, -.5f }, { 1, 0 } },
	{ { .5f, .5f }, { 1, 1 } },
	{ { -.5f, .5f }, { 0, 1 } }
};


struct Sprite {
	struct { InterpolandHandle x, y; } position;
	struct { GLfloat r, g, b, a; } color;
	sf::Texture* texture;
};
using SpriteHandle = SparseArray3<Sprite, 100>::Handle;
using SASprite = SparseArray3<Sprite, 100>;

SASprite sprites;

SpriteHandle MakeSprite(InterpolandHandle x, InterpolandHandle y, sf::Texture* texture) {
	return sprites.add({ { x, y }, { 1, 1, 1, 1 }, texture });
}

void UpdateSprites() {
	glEnable(GL_TEXTURE_2D);

	for(auto it = sprites.begin(); it != sprites.end(); it++) {
		sf::Texture::bind(it->texture);


		glPushMatrix();
		glTranslatef(it->position.x->currValue, it->position.y->currValue, 0);
		glBegin(GL_QUADS);

		glColor4fv(&it->color.r);

		for(int i = 0; i < 4; i++) {
			glTexCoord2fv(&spriteVerts[i].tex.u);
			glVertex2fv(&spriteVerts[i].position.x);
		}

		glEnd();
		glPopMatrix();
	}

	glDisable(GL_TEXTURE_2D);
}



//constexpr sf::Time one_second = sf::milliseconds(1000);

int main() {
	srand(0);

	sf::RenderWindow window(sf::VideoMode(800, 600), "/\\/\\/\\/");
	//sf::Window window(sf::VideoMode(800, 600), "/\\/\\/\\/");
	window.setKeyRepeatEnabled(false);

	const char* version = (const char*)glGetString(GL_VERSION);
	std::cout << version << std::endl;


	sf::Texture texture;
	texture.loadFromMemory(block, sizeof(block));

	sf::Font font;
	font.loadFromMemory(whiterabbit, sizeof(whiterabbit));


	auto x = MakeInterpoland(0);
	auto y = MakeInterpoland(0);
	auto sprite = MakeSprite(x, y, &texture);

	InterpolateTo(x, 1, sf::seconds(1), Tween::SINE);

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


	size_t fireWidth = 100;
	size_t fireHeight = 100;
	size_t fireVertCount = fireWidth * fireHeight;
	unsigned char fireIntensity[fireWidth * fireHeight];

	Vertex fireVerts[fireVertCount];

	size_t fireCellCount = (fireWidth - 1) * (fireHeight - 1);

	GLushort fireIndices[fireCellCount * 4];
	int fireIndexCount = fireCellCount * 4;
	{
		GLushort* fireIndex = fireIndices;
		for(int x = 0; x < fireWidth - 1; x++)
			for(int y = 0; y < fireHeight - 1; y++) {

				fireVerts[x + y * fireWidth].position = { float(x) / (fireWidth - 1) - .5, float(y) / (fireHeight - 1) - .5 };
				*fireIndex = x + y * fireWidth;
				fireIndex++;

				fireVerts[x + 1 + y * fireWidth].position = { float(x + 1) / (fireWidth - 1) - .5, float(y) / (fireHeight - 1) - .5 };
				*fireIndex = x + 1 + y * fireWidth;
				fireIndex++;

				fireVerts[x + 1 + (y + 1) * fireWidth].position = { float(x + 1) / (fireWidth - 1) - .5, float(y + 1) / (fireHeight - 1) - .5 };
				*fireIndex = x + 1 + (y + 1) * fireWidth;
				fireIndex++;

				fireVerts[x + (y + 1) * fireWidth].position = { float(x) / (fireWidth - 1) - .5, float(y + 1) / (fireHeight - 1) - .5 };
				*fireIndex = x + (y + 1) * fireWidth;
				fireIndex++;

				ASSERT(fireIndex <= fireIndices + fireIndexCount);
			}
	}








	sf::Time fireTimer = sf::milliseconds(0);

	glViewport(0, 0, window.getSize().x, window.getSize().y);
    while(true) {
        sf::Event event;
        while(window.pollEvent(event)) {
            if(event.type == sf::Event::Closed)
                window.close();
			else if(event.type == sf::Event::Resized) {
				glViewport(0, 0, event.size.width, event.size.height);
			}
			else if(event.type == sf::Event::KeyPressed) {
				if(event.key.code == sf::Keyboard::Key::Up)
					Interpolate(y, -1, sf::seconds(1), Tween::SINE);
				else if(event.key.code == sf::Keyboard::Key::Down)
					Interpolate(y, 1, sf::seconds(1), Tween::SINE);
				else if(event.key.code == sf::Keyboard::Key::Right)
					Interpolate(x, 1, sf::seconds(1), Tween::SINE);
				else if(event.key.code == sf::Keyboard::Key::Left)
					Interpolate(x, -1, sf::seconds(1), Tween::SINE);
			}
        }
        if(!window.isOpen())
			break;


		sf::Time dt = frame.restart();
		framerate.update(dt);

		UpdateInterpolations(dt);

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
					}
					ct++;
					if(x < fireWidth - 1) {
						average += fireIntensity[x + 1 + y * fireWidth];
					}
					ct++;

					if(y > 0) {
						average += fireIntensity[x + (y - 1) * fireWidth];
						ct++;
					}
					if(y < fireHeight - 1) {
						average += fireIntensity[x + (y + 1) * fireWidth] * 3;
						ct += 3;
					}
					average /= ct;

					if(y == fireHeight - 1)// && x > fireWidth / 5 && x < fireWidth * 4 / 5)
						average += rand() % 12;
					else {
						average += (rand() % 15 == 0)? (rand() % 90 - 70): 0;
						//average -= rand() % 10;

						if(average > 128)
							average += 1;
						else
							average -= 1;
					}

					if(average > 255)
						average = 255;
					else if(average < 0)
						average = 0;

					fireIntensity[x + y * fireWidth] = average;
				}
		}
		for(int i = 0; i < fireVertCount; i++) {
			fireVerts[i].color = {
				fireIntensity[i] / 255.0f,
				fireIntensity[i] / 255.0f / 2,
				fireIntensity[i] / 255.0f / 5,
				fireIntensity[i] / 255.0f
			};
		}



		glClear(GL_COLOR_BUFFER_BIT);


		window.pushGLStates();
			window.draw(debugPanelRect);

			framerateText->message = framerate.current >= 0? "FPS:            " + to_string(framerate.current): "";
			interpolandCountText->message =                  "interpolands:   " + to_string(interpolands.size());
			interpolationCountText->message =                "interpolations: " + to_string(interpolations.size());
			textCountText->message =                         "texts:          " + to_string(texts.size());
			UpdateTexts(&window);

		window.popGLStates();

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(
			-float(window.getSize().x) / window.getSize().y / 2 * 6, // left
			float(window.getSize().x) / window.getSize().y / 2 * 6, // right
			.5 * 6, // bottom
			-.5 * 6, // top
			-1, // near
			1 // far
		);
		glMatrixMode(GL_MODELVIEW);

		UpdateSprites();
		DrawVertices(GL_QUADS, fireVerts, fireIndices, fireIndexCount);


		window.display();
    }

    return 0;
}
