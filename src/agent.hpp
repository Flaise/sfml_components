#ifndef AGENT_HPP_INCLUDED
#define AGENT_HPP_INCLUDED

#include <unordered_map>
#include <SFML/System/Clock.hpp>

#include "sparsearray3.hpp"
#include "interpolation.hpp"
#include "discrete2d.hpp"
#include "obstacle.hpp"
#include "entity.hpp"
#include "assert.hpp"


struct Agent {
	EntityHandle entity;
	ObstacleHandle obstacle;
	InterpolandHandle x, y;
	Direction4 direction;
	sf::Time timePerMove;
	sf::Time timeUntilNextMove;
};
using AgentHandle = SparseArray3<Agent, 20>::Handle;
SparseArray3<Agent, 20> agents;

std::unordered_map<EntityHandle, AgentHandle> entity_agent;


AgentHandle MakeAgent(EntityHandle entity, ObstacleHandle obstacle, InterpolandHandle x, InterpolandHandle y, sf::Time timePerMove) {
	ASSERT(entity_agent.count(entity) == 0);

	AgentHandle agent = agents.add({entity, obstacle, x, y, Direction4::NONE, timePerMove, sf::milliseconds(0)});
	entity_agent[entity] = agent;
	return agent;
}

bool _moveAgent(Agent* agent, Vec2i delta, sf::Time duration) {
	Vec2i dest = obstacles.get(agent->obstacle);
	dest += delta;

	if(obstacles.contains(dest)) {
		auto obstructor = obstacles.get(dest);
		if(pushables.left.count(obstructor)) {
			// the obstacle can be pushed
			if(obstacles.contains(dest + delta)) {
				// something on other side
				agent->timeUntilNextMove = sf::milliseconds(0);
				return false;
			}
			else {
				// path is clear
				obstacles.modify(obstructor, dest + delta);
				Interpolate(pushables.left.at(obstructor).x, delta.x, duration, Tween::Linear);
				Interpolate(pushables.left.at(obstructor).y, delta.y, duration, Tween::Linear);
			}
		}
		else {
			// too heavy
			agent->timeUntilNextMove = sf::milliseconds(0);
			return false;
		}
	}

	obstacles.modify(agent->obstacle, dest);

	agent->timeUntilNextMove += agent->timePerMove;
	return true;
}

void UpdateAgents(sf::Time dt) {
	for(auto it = agents.begin(); it != agents.end(); it++) {
		it->timeUntilNextMove -= dt;
		if(it->timeUntilNextMove > sf::milliseconds(0))
			continue;
		if(it->x->isMoving() || it->y->isMoving()) {
			it->timeUntilNextMove = sf::milliseconds(0);
			continue;
		}

		auto duration = it->timePerMove + it->timeUntilNextMove;

		switch(it->direction) {
			case Direction4::NONE:
				it->timeUntilNextMove = sf::milliseconds(0);
				break;
			case Direction4::NORTH:
				if(_moveAgent(&(*it), {0, -1}, duration))
					Interpolate(it->y, -1, duration, Tween::Linear);
				break;
			case Direction4::EAST:
				if(_moveAgent(&(*it), {1, 0}, duration))
					Interpolate(it->x, 1, duration, Tween::Linear);
				break;
			case Direction4::SOUTH:
				if(_moveAgent(&(*it), {0, 1}, duration))
					Interpolate(it->y, 1, duration, Tween::Linear);
				break;
			case Direction4::WEST:
				if(_moveAgent(&(*it), {-1, 0}, duration))
					Interpolate(it->x, -1, duration, Tween::Linear);
				break;
			default:
				ASSERT(false);
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

void InitAgents() {
	destroyFuncs.push_back([](EntityHandle entity) {
		if(entity_agent.count(entity)) {
			agents.remove(entity_agent[entity]);
			entity_agent.erase(entity);
		}
	});
}

#endif // AGENT_HPP_INCLUDED
