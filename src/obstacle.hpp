#ifndef OBSTACLE_HPP_INCLUDED
#define OBSTACLE_HPP_INCLUDED

#include "handledset.hpp"
#include "discrete2d.hpp"
#include "sparsearray3.hpp"
#include "interpolation.hpp"

HandledSet<Vec2i, Vec2iHash, Vec2iEqual> obstacles;
using ObstacleHandle = HandledSet<Vec2i, Vec2iHash, Vec2iEqual>::Handle;


struct Pushable {
	ObstacleHandle obstacle;
	InterpolandHandle x, y;
};
struct PushableHash {
	size_t operator()(const Pushable& pushable) const {
		return boost::hash<ObstacleHandle>()(pushable.obstacle);
	}
};
struct PushableEqual {
	bool operator()(const Pushable& a, const Pushable& b) const {
		if(a.obstacle == b.obstacle) {
			ASSERT(a.x == b.x && a.y == b.y); // these links should always be exclusive
			return true;
		}
		return false;
	}
};

boost::bimap<
	boost::bimaps::unordered_set_of<ObstacleHandle>,
	boost::bimaps::unordered_set_of<Pushable, PushableHash, PushableEqual>
> pushables;

using ObstaclePushableMapping = typename boost::bimap<
	boost::bimaps::unordered_set_of<ObstacleHandle>,
	boost::bimaps::unordered_set_of<Pushable, PushableHash, PushableEqual>
>::value_type;


void MakePushable(ObstacleHandle obstacle, InterpolandHandle x, InterpolandHandle y) {
	pushables.insert(ObstaclePushableMapping(obstacle, {obstacle, x, y}));
}


#endif // OBSTACLE_HPP_INCLUDED
