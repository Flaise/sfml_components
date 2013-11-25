#ifndef OBSTACLE_HPP_INCLUDED
#define OBSTACLE_HPP_INCLUDED

#include <unordered_map>

#include "handledset.hpp"
#include "discrete2d.hpp"
#include "sparsearray3.hpp"
#include "interpolation.hpp"
#include "entity.hpp"

HandledSet<Vec2i> obstacles;
using ObstacleHandle = HandledSet<Vec2i>::Handle;

std::unordered_map<EntityHandle, ObstacleHandle> entity_obstacle;

ObstacleHandle MakeObstacle(EntityHandle entity, Vec2i position) {
	ASSERT(entity_obstacle.count(entity) == 0);
	ASSERT(!obstacles.contains(position));

	ObstacleHandle obstacle = obstacles.add(position);
	entity_obstacle[entity] = obstacle;
	return obstacle;
}


struct Pushable {
	EntityHandle entity;
	ObstacleHandle obstacle;
	InterpolandHandle x, y;
};
namespace std {
	template<>
	struct hash<Pushable> {
		size_t operator()(const Pushable& pushable) const {
			return hash<ObstacleHandle>()(pushable.obstacle);
		}
	};

	template<>
	struct equal_to<Pushable> {
		bool operator()(const Pushable& a, const Pushable& b) const {
			if(a.obstacle == b.obstacle) {
				ASSERT(a.x == b.x && a.y == b.y); // these links should always be exclusive
				return true;
			}
			return false;
		}
	};
}

boost::bimap<
	boost::bimaps::unordered_set_of<ObstacleHandle>,
	boost::bimaps::unordered_set_of<Pushable, std::hash<Pushable>>
> pushables;

using ObstaclePushableMapping = typename boost::bimap<
	boost::bimaps::unordered_set_of<ObstacleHandle>,
	boost::bimaps::unordered_set_of<Pushable, std::hash<Pushable>>
>::value_type;


void MakePushable(EntityHandle entity, ObstacleHandle obstacle, InterpolandHandle x, InterpolandHandle y) {
	pushables.insert(ObstaclePushableMapping(obstacle, {entity, obstacle, x, y}));
}

void InitObstacles() {
	destroyFuncs.push_back([](EntityHandle entity) {
		if(entity_obstacle.count(entity)) {
			pushables.left.erase(entity_obstacle[entity]);
			obstacles.remove(entity_obstacle[entity]);
			entity_obstacle.erase(entity);
		}
	});
}


#endif // OBSTACLE_HPP_INCLUDED
