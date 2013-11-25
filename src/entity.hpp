#ifndef ENTITY_HPP_INCLUDED
#define ENTITY_HPP_INCLUDED

#include <unordered_set>
#include <vector>

#include <boost/cstdint.hpp>
#include <boost/limits.hpp>

using EntityHandle = uint16_t;
std::unordered_set<EntityHandle> entities;

EntityHandle nextEntity = 0;

EntityHandle MakeEntity() {
	ASSERT(entities.size() < std::numeric_limits<EntityHandle>::max());

	EntityHandle result = nextEntity;
	entities.insert(result);

	while(entities.count(++nextEntity)); // find next unused

	ASSERT(nextEntity != result);
	ASSERT(entities.count(nextEntity) == 0);

	return result;
}

std::vector< std::function<void(EntityHandle)> > destroyFuncs;

void DestroyEntity(EntityHandle entity) {
	ASSERT(entities.count(entity));

	for(auto it = destroyFuncs.begin(); it != destroyFuncs.end(); it++)
		(*it)(entity);
	entities.erase(entity);
}

#endif // ENTITY_HPP_INCLUDED
