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


void _assertFail(const char* file, int line) {
	; // for debug breakpoint
}
#define ASSERT_FAIL(file, line) _assertFail(file, line)

#include "assert.hpp"
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

void DrawSprites() {
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


#include <boost/cstdint.hpp>
#include <boost/operators.hpp>
struct Vec2i: boost::addable<Vec2i> {
	int16_t x, y;

	Vec2i() {}
	Vec2i(int16_t x, int16_t y): x(x), y(y) {}

	Vec2i& operator+=(const Vec2i& other) {
		this->x += other.x;
		this->y += other.y;
		return *this;
	}
};
struct Vec2iHash {
	size_t operator()(const Vec2i& vec) const {
		return static_cast<size_t>(vec.x) ^ (static_cast<size_t>(vec.y) << 16);
	}
};
struct Vec2iEqual {
	bool operator()(const Vec2i& a, const Vec2i& b) const {
		return a.x == b.x && a.y == b.y;
	}
};

#include <boost/bimap.hpp>
#include <boost/bimap/unordered_set_of.hpp>
using ObstacleIndex = size_t;
ObstacleIndex nextObstacleIndex = 0;
boost::bimap<
	boost::bimaps::unordered_set_of<ObstacleIndex>,
	boost::bimaps::unordered_set_of<Vec2i, Vec2iHash, Vec2iEqual>
> obstacles;
typedef boost::bimap<
	boost::bimaps::unordered_set_of<ObstacleIndex>,
	boost::bimaps::unordered_set_of<Vec2i, Vec2iHash, Vec2iEqual>
>::value_type obstacle_v;

#include <boost/limits.hpp>
ObstacleIndex MakeObstacle(uint16_t x, uint16_t y) {
	ASSERT(obstacles.size() < std::numeric_limits<size_t>::max());
	auto result = nextObstacleIndex;
	obstacles.insert(obstacle_v(result, {x, y}));
	do {
		nextObstacleIndex++;
	} while(obstacles.left.count(nextObstacleIndex) > 0);

	return result;
}


void MakeWall(int16_t x, int16_t y, sf::Texture* texture) {
	MakeSprite(MakeInterpoland(x), MakeInterpoland(y), texture);
	MakeObstacle(x, y);
}

enum class Direction4: unsigned char {
	NORTH, EAST, SOUTH, WEST, NONE
};
Direction4 RandomDirection4() {
	return static_cast<Direction4>(rand() % 4);
}

struct Agent {
	ObstacleIndex obstacle;
	InterpolandHandle x, y;
	Direction4 direction;
	sf::Time timePerMove;
	sf::Time timeUntilNextMove;
};
using AgentHandle = SparseArray3<Agent, 20>::Handle;

SparseArray3<Agent, 20> agents;

AgentHandle MakeAgent(InterpolandHandle x, InterpolandHandle y, sf::Time timePerMove) {
	return agents.add({ MakeObstacle(x->currValue, y->currValue), x, y, Direction4::NONE, timePerMove, sf::milliseconds(0) });
}

bool _moveAgent(Agent* agent, int16_t dx, int16_t dy) {
	Vec2i dest = obstacles.left.at(agent->obstacle);
	dest += Vec2i(dx, dy);

	if(obstacles.right.count(dest)) {
		agent->timeUntilNextMove = sf::microseconds(0);
		return false;
	}

	obstacles.left.erase(agent->obstacle);
	obstacles.insert(obstacle_v(agent->obstacle, dest));

	agent->timeUntilNextMove += agent->timePerMove;
	return true;
}

void UpdateAgents(sf::Time dt) {
	for(auto it = agents.begin(); it != agents.end(); it++) {
		it->timeUntilNextMove -= dt;
		if(it->timeUntilNextMove <= sf::milliseconds(0)) {
			auto duration = it->timePerMove + it->timeUntilNextMove;

			switch(it->direction) {
				case Direction4::NONE:
					it->timeUntilNextMove = sf::microseconds(0);
					break;
				case Direction4::NORTH:
					if(_moveAgent(&(*it), 0, -1))
						Interpolate(it->y, -1, duration, Tween::Linear);
					break;
				case Direction4::EAST:
					if(_moveAgent(&(*it), 1, 0))
						Interpolate(it->x, 1, duration, Tween::Linear);
					break;
				case Direction4::SOUTH:
					if(_moveAgent(&(*it), 0, 1))
						Interpolate(it->y, 1, duration, Tween::Linear);
					break;
				case Direction4::WEST:
					if(_moveAgent(&(*it), -1, 0))
						Interpolate(it->x, -1, duration, Tween::Linear);
					break;
				default:
					ASSERT(false);
			}
		}
	}
}

void SetAgentDirection(AgentHandle agent, bool up, bool right, bool down, bool left) {
	// Only change direction when exactly one button is pressed. Stop when nothing is pressed.
	if(!up && !down && !right && !left) {
		agent->direction = Direction4::NONE;
	}
	else if(up && !down && !right && !left) {
		agent->direction = Direction4::NORTH;
	}
	else if(down && !up && !right && !left) {
		agent->direction = Direction4::SOUTH;
	}
	else if(right && !left && !up && !down) {
		agent->direction = Direction4::EAST;
	}
	else if(left && !right && !up && !down) {
		agent->direction = Direction4::WEST;
	}
}




struct WanderAI {
	AgentHandle agent;
	sf::Time minTime, maxTime, timeUntilNextMove;
};
using WanderAIHandle = SparseArray3<WanderAI, 20>::Handle;

SparseArray3<WanderAI, 20> wanderAIs;

WanderAIHandle MakeWanderAI(AgentHandle agent, sf::Time minTime, sf::Time maxTime) {
	return wanderAIs.add({ agent, minTime, maxTime, sf::milliseconds(0) });
}

void UpdateWanderAIs(sf::Time dt) {
	for(auto it = wanderAIs.begin(); it != wanderAIs.end(); it++) {
		it->timeUntilNextMove -= dt;
		if(it->timeUntilNextMove <= sf::milliseconds(0)) {
			it->timeUntilNextMove += it->minTime + sf::milliseconds(rand() % it->maxTime.asMilliseconds());
			it->agent->direction = RandomDirection4();
		}
		else {
			it->agent->direction = Direction4::NONE;
		}
	}
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

	MakeWall(-1, -1, &texture);
	MakeWall(-1, -2, &texture);
	MakeWall(-2, -2, &texture);

	{
		auto x = MakeInterpoland(2);
		auto y = MakeInterpoland(1);
		MakeSprite(x, y, &texture);
		auto agent = MakeAgent(x, y, sf::milliseconds(1000));
		MakeWanderAI(agent, sf::milliseconds(1500), sf::milliseconds(3500));
	}

	auto x = MakeInterpoland(0);
	auto y = MakeInterpoland(1);
	MakeSprite(x, y, &texture);
	auto playerAgent = MakeAgent(x, y, sf::milliseconds(1000));

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
	Framerate<sf::Time> framerate(sf::seconds(1));
	//Framerate<sf::Time, one_second> framerate;
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
		UpdateInterpolations(dt);



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

		DrawSprites();


		window.display();
	}

	return 0;
}
