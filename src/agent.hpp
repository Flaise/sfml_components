#ifndef AGENT_HPP_INCLUDED
#define AGENT_HPP_INCLUDED

#include <unordered_set>
#include <SFML/System/Clock.hpp>

#include "sparsearray3.hpp"
#include "interpolation.hpp"
#include "discrete2d.hpp"
#include "body.hpp"
#include "destroyable.hpp"
#include "assert.hpp"


struct Agent {
	DestroyableHandle destroyable;
	BodyHandle body;
	InterpolandHandle x, y, z, xScale;
	Direction4 direction;
	sf::Time timePerMove;
	sf::Time timeUntilNextMove;
};
using AgentHandle = SparseArray3<Agent, 20>::Handle;
SparseArray3<Agent, 20> agents;


std::unordered_set<AgentHandle> eaters;


AgentHandle MakeAgent(
	DestroyableHandle destroyable, BodyHandle body,
	InterpolandHandle x, InterpolandHandle y, InterpolandHandle z, InterpolandHandle xScale,
	sf::Time timePerMove
) {
	ReferenceDestroyable(destroyable);
	return agents.add({destroyable, body, x, y, z, xScale, Direction4::NONE, timePerMove, sf::milliseconds(0)});
}

bool _moveAgent(AgentHandle agent, Vec3i delta, sf::Time duration) {
	auto dest = agent->body->position + delta;

	auto obstructorIt = GetBodyAt(dest);
	if(obstructorIt != bodies.end()) {
		auto obstructor = obstructorIt.getHandle();

		if(eatables.count(obstructor) && eaters.count(agent)) {
			obstructor->destroyable->alive = false;
		}
		else if(pushables.count(obstructor)) {
			// the obstacle can be pushed
			if(IsBodyAt(dest + delta)) {
				// something on other side
				agent->timeUntilNextMove = sf::milliseconds(0);
				return false;
			}
			else {
				// path is clear
				MoveBodyTo(obstructor, dest + delta);
				if(delta.x)
					Interpolate(pushables[obstructor].x, delta.x, duration, Tween::Linear);
				if(delta.y)
					Interpolate(pushables[obstructor].y, delta.y, duration, Tween::Linear);
				if(delta.z)
					Interpolate(pushables[obstructor].z, delta.z, duration, Tween::Linear);
			}
		}
		else {
			// too heavy
			agent->timeUntilNextMove = sf::milliseconds(0);
			return false;
		}
	}

	MoveBodyTo(agent->body, dest);

	agent->timeUntilNextMove += agent->timePerMove;
	return true;
}

void UpdateAgents(sf::Time dt) {
	for(auto it = agents.begin(); it != agents.end(); it++) {
		if(!it->destroyable->alive) {
			if(eaters.count(it.getHandle()))
				eaters.erase(it.getHandle());

			UnreferenceDestroyable(it->destroyable);
			agents.remove(it);
			continue;
		}

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
				if(_moveAgent(it.getHandle(), {0, 0, 1}, duration))
					Interpolate(it->z, 1, duration, Tween::Linear);
				break;
			case Direction4::EAST:
				if(_moveAgent(it.getHandle(), {1, 0, 0}, duration)) {
					Interpolate(it->x, 1, duration, Tween::Linear);
					if(it->xScale->destValue != 1)
						InterpolateTo(it->xScale, 1, sf::milliseconds(300), Tween::SINE);
				}
				break;
			case Direction4::SOUTH:
				if(_moveAgent(it.getHandle(), {0, 0, -1}, duration))
					Interpolate(it->z, -1, duration, Tween::Linear);
				break;
			case Direction4::WEST:
				if(_moveAgent(it.getHandle(), {-1, 0, 0}, duration)) {
					Interpolate(it->x, -1, duration, Tween::Linear);
					if(it->xScale->destValue != -1)
						InterpolateTo(it->xScale, -1, sf::milliseconds(300), Tween::SINE);
				}
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

#endif // AGENT_HPP_INCLUDED
