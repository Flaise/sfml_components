#ifndef AGENT_HPP_INCLUDED
#define AGENT_HPP_INCLUDED

#include <SFML/System/Clock.hpp>
#include "sparsearray3.hpp"
#include "interpolation.hpp"
#include "discrete2d.hpp"
#include "obstacle.hpp"

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

#endif // AGENT_HPP_INCLUDED
