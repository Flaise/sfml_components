#ifndef WANDERAI_HPP_INCLUDED
#define WANDERAI_HPP_INCLUDED

#include <unordered_map>
#include <SFML/System/Clock.hpp>

#include "sparsearray3.hpp"
#include "agent.hpp"
#include "entity.hpp"
#include "assert.hpp"

struct WanderAI {
	EntityHandle entity;
	AgentHandle agent;
	sf::Time minTime, maxTime, timeUntilNextMove;
};
using WanderAIHandle = SparseArray3<WanderAI, 20>::Handle;
SparseArray3<WanderAI, 20> wanderAIs;

std::unordered_map<EntityHandle, WanderAIHandle> entity_wanderAI;

WanderAIHandle MakeWanderAI(EntityHandle entity, AgentHandle agent, sf::Time minTime, sf::Time maxTime) {
	ASSERT(entity_wanderAI.count(entity) == 0);

	auto handle = wanderAIs.add({entity, agent, minTime, maxTime, sf::milliseconds(0)});
	entity_wanderAI[entity] = handle;
	return handle;
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

void InitWanderAI() {
	destroyFuncs.push_back([](EntityHandle entity) {
		if(entity_wanderAI.count(entity)) {
			wanderAIs.remove(entity_wanderAI[entity]);
			entity_wanderAI.erase(entity);
		}
	});
}

#endif // WANDERAI_HPP_INCLUDED
