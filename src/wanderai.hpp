#ifndef WANDERAI_HPP_INCLUDED
#define WANDERAI_HPP_INCLUDED

#include <SFML/System/Clock.hpp>

#include "sparsearray3.hpp"
#include "agent.hpp"
#include "destroyable.hpp"
#include "assert.hpp"

struct WanderAI {
	DestroyableHandle destroyable;
	AgentHandle agent;
	sf::Time minTime, maxTime, timeUntilNextMove;
};
using WanderAIHandle = SparseArray3<WanderAI, 20>::Handle;
SparseArray3<WanderAI, 20> wanderAIs;

WanderAIHandle MakeWanderAI(DestroyableHandle destroyable, AgentHandle agent, sf::Time minTime, sf::Time maxTime) {
	ReferenceDestroyable(destroyable);
	return wanderAIs.add({destroyable, agent, minTime, maxTime, sf::milliseconds(0)});
}

void UpdateWanderAIs(sf::Time dt) {
	for(auto it = wanderAIs.begin(); it != wanderAIs.end(); it++) {
		if(!it->destroyable->alive) {
			UnreferenceDestroyable(it->destroyable);
			wanderAIs.remove(it);
			continue;
		}

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

#endif // WANDERAI_HPP_INCLUDED
