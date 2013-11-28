

template<class T>
bool approximately_equal(T a, T b, T tolerance) {
	auto c = (a - b);
	return c < tolerance && c > -tolerance;
}

#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

#include <iostream>
#include <string>
#include <cstdlib> // for rand()

#pragma GCC diagnostic ignored "-Wunused-local-typedefs" // temporarily disable warnings
	#include <boost/lexical_cast.hpp>
#pragma GCC diagnostic pop // reenable warnings

void _assertFail(const char* file, int line) {
	// mostly for debug breakpoint
	std::cout << "ASSERTION ERROR: file " << file << ":" << line << std::endl;
}
#define ASSERT_FAIL(file, line) _assertFail(file, line)

#include "assert.hpp"
#include "sparsearray3.hpp"

#include "interpolation.hpp"
#include "framerate.hpp"
#include "sprite.hpp"
#include "agent.hpp"
#include "wanderai.hpp"
#include "body.hpp"
#include "worldcam.hpp"

#include "assets/block.h"
#include "assets/block-pushable.h"
#include "assets/longears.h"
#include "assets/sharpears.h"
#include "assets/whiterabbit.h"


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

TextHandle MakeDisplayText(sf::Font* font, sf::String message, float x, float y, float ax, float ay) {
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





void MakeWall(int16_t x, int16_t y, int16_t z, sf::Texture* texture) {
	auto destroyable = MakeDestroyable();
	MakeSprite(
		destroyable, MakeInterpoland(destroyable, x), MakeInterpoland(destroyable, y), MakeInterpoland(destroyable, z), texture
	);
	MakeBody(destroyable, {x, z});
}
void MakePushableBlock(int16_t x, int16_t y, int16_t z, sf::Texture* texture) {
	auto destroyable = MakeDestroyable();
	auto xi = MakeInterpoland(destroyable, x);
	auto yi = MakeInterpoland(destroyable, y);
	auto zi = MakeInterpoland(destroyable, z);
	MakeSprite(destroyable, xi, yi, zi, texture);
	MakePushable(MakeBody(destroyable, {x, z}), xi, zi);
}




int main() {
	srand(0);

	sf::RenderWindow window(sf::VideoMode(800, 600), "/\\/\\/\\/");
	//sf::Window window(sf::VideoMode(800, 600), "/\\/\\/\\/");
	window.setKeyRepeatEnabled(false);

	const char* version = (const char*)glGetString(GL_VERSION);
	std::cout << version << std::endl;


	sf::Texture texture_block;
	texture_block.loadFromMemory(block, sizeof(block));

	sf::Texture texture_block_pushable;
	texture_block_pushable.loadFromMemory(block_pushable, sizeof(block_pushable));

	sf::Texture texture_longears;
	texture_longears.loadFromMemory(longears, sizeof(longears));

	sf::Texture texture_sharpears;
	texture_sharpears.loadFromMemory(sharpears, sizeof(sharpears));

	sf::Font font;
	font.loadFromMemory(whiterabbit, sizeof(whiterabbit));

	MakeWall(-1, 0, -1, &texture_block);
	MakeWall(-1, 0, -2, &texture_block);
	MakeWall(-2, 0, -2, &texture_block);
	MakePushableBlock(0, 0, -1, &texture_block_pushable);

	{
		auto destroyable = MakeDestroyable();
		auto x = MakeInterpoland(destroyable, 2);
		auto y = MakeInterpoland(destroyable, 0);
		auto z = MakeInterpoland(destroyable, 1);
		MakeSprite(destroyable, x, y, z, &texture_sharpears);
		auto obstacle = MakeBody(destroyable, {2, 1});
		auto agent = MakeAgent(destroyable, obstacle, x, z, sf::milliseconds(800));
		MakeWanderAI(destroyable, agent, sf::milliseconds(2500), sf::milliseconds(4500));
		MakeWorldCamFocus(destroyable, obstacle);
	}

	{
		auto destroyable = MakeDestroyable();
		auto x = MakeInterpoland(destroyable, 1);
		auto y = MakeInterpoland(destroyable, 0);
		auto z = MakeInterpoland(destroyable, -1);
		MakeSprite(destroyable, x, y, z, &texture_longears);
		auto obstacle = MakeBody(destroyable, {1, -1});
		auto agent = MakeAgent(destroyable, obstacle, x, z, sf::milliseconds(750));
		MakeWanderAI(destroyable, agent, sf::milliseconds(1500), sf::milliseconds(5000));
		MakeWorldCamFocus(destroyable, obstacle);
		MakePushable(obstacle, x, z);

		eatables.insert(obstacle);
	}

	AgentHandle playerAgent; {
		auto destroyable = MakeDestroyable();
		auto x = MakeInterpoland(destroyable, 0);
		auto y = MakeInterpoland(destroyable, 0);
		auto z = MakeInterpoland(destroyable, 1);
		auto sprite = MakeSprite(destroyable, x, y, z, &texture_sharpears);
		sprite->color.b = .35f;
		auto obstacle = MakeBody(destroyable, {0, 1});
		playerAgent = MakeAgent(destroyable, obstacle, x, z, sf::milliseconds(800));
		MakeWorldCamFocus(destroyable, obstacle);

		eaters.insert(playerAgent);
	}

	bool up = false;
	bool down = false;
	bool right = false;
	bool left = false;



	auto framerateText = MakeDisplayText(&font, "", 0, window.getSize().y - 60, 0, 0);
	auto interpolandCountText = MakeDisplayText(&font, "", 0, window.getSize().y - 45, 0, 0);
	auto interpolationCountText = MakeDisplayText(&font, "", 0, window.getSize().y - 30, 0, 0);
	auto textCountText = MakeDisplayText(&font, "", 0, window.getSize().y - 15, 0, 0);
	sf::RectangleShape debugPanelRect;
	debugPanelRect.setPosition(sf::Vector2f(0, window.getSize().y - 60));
	debugPanelRect.setSize(sf::Vector2f(300, 60));
	debugPanelRect.setFillColor(sf::Color(128, 128, 128, 128));



	glViewport(0, 0, window.getSize().x, window.getSize().y);


	sf::Clock frame;
	Framerate framerate;
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
					up = true;
				else if(event.key.code == sf::Keyboard::Key::Down)
					down = true;
				else if(event.key.code == sf::Keyboard::Key::Right)
					right = true;
				else if(event.key.code == sf::Keyboard::Key::Left)
					left = true;
				SetAgentDirection(playerAgent, up, right, down, left);
			}
			else if(event.type == sf::Event::KeyReleased) {
				if(event.key.code == sf::Keyboard::Key::Up)
					up = false;
				else if(event.key.code == sf::Keyboard::Key::Down)
					down = false;
				else if(event.key.code == sf::Keyboard::Key::Right)
					right = false;
				else if(event.key.code == sf::Keyboard::Key::Left)
					left = false;
				SetAgentDirection(playerAgent, up, right, down, left);
			}
		}
		if(!window.isOpen())
			break;


		sf::Time dt = frame.restart();
		framerate.update(dt);

		UpdateWanderAIs(dt);
		UpdateAgents(dt);
		UpdateWorldCam(&window);
		UpdateInterpolations(dt);



		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		window.pushGLStates();
			window.draw(debugPanelRect);

			framerateText->message =
			    framerate.current >= 0? "FPS:            " + boost::lexical_cast<std::string>(framerate.current): "";
			interpolandCountText->message =
			                            "interpolands:   " + boost::lexical_cast<std::string>(interpolands.size());
			interpolationCountText->message =
			                            "interpolations: " + boost::lexical_cast<std::string>(interpolations.size());
			textCountText->message =
			                            "texts:          " + boost::lexical_cast<std::string>(texts.size());
			UpdateTexts(&window);

		window.popGLStates();

		DrawWorldCam(&window);
		DrawSprites();


		window.display();
	}

	return 0;
}
