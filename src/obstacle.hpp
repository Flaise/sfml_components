#ifndef OBSTACLE_HPP_INCLUDED
#define OBSTACLE_HPP_INCLUDED

#pragma GCC diagnostic ignored "-Wunused-local-typedefs" // temporarily disable warnings
	#include <boost/bimap.hpp>
	#include <boost/bimap/unordered_set_of.hpp>
#pragma GCC diagnostic pop // reenable warnings

//#include "handledset.hpp"
#include "discrete2d.hpp"
#include "sparsearray3.hpp"
#include "interpolation.hpp"
#include "destroyable.hpp"

struct Obstacle {
	DestroyableHandle destroyable;
	Vec2i position;
};
SparseArray3<Obstacle> obstacles;
using ObstacleHandle = typename SparseArray3<Obstacle>::Handle;

struct Pushable {
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
			return a.obstacle->position == b.obstacle->position;
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

SparseArray3<Obstacle>::Iterator GetObstacleAt(Vec2i position) {
	auto it = obstacles.begin();
	for(; it != obstacles.end(); it++) {
		if(!it->destroyable->alive) {
			if(pushables.left.count(it.getHandle()))
				pushables.left.erase(it.getHandle());

			UnreferenceDestroyable(it->destroyable);
			obstacles.remove(it);
			continue;
		}
		if(it->position == position)
			return it;
	}
	return it;
}

bool IsObstacleAt(Vec2i position) {
	return GetObstacleAt(position) != obstacles.end();
}

ObstacleHandle MakeObstacle(DestroyableHandle destroyable, Vec2i position) {
	ASSERT(!IsObstacleAt(position));

	ReferenceDestroyable(destroyable);
	return obstacles.add({destroyable, position});
}

void MoveObstacleTo(ObstacleHandle obstacle, Vec2i dest) {
	ASSERT(dest == obstacle->position || !IsObstacleAt(dest));

	obstacle->position = dest;
}

/*HandledSet<Obstacle> obstacles;
using ObstacleHandle = HandledSet<Obstacle>::Handle;

ObstacleHandle MakeObstacle(DestroyableHandle destroyable, Vec2i position) {
	ASSERT(!obstacles.contains(position));

	ReferenceDestroyable(destroyable);
	return obstacles.add({destroyable, position});
}

void MoveObstacleTo(ObstacleHandle obstacle, Vec2i dest) {
	ASSERT(!obstacles.contains())
}*/



void MakePushable(ObstacleHandle obstacle, InterpolandHandle x, InterpolandHandle y) {
	pushables.insert(ObstaclePushableMapping(obstacle, {obstacle, x, y}));
}


#endif // OBSTACLE_HPP_INCLUDED
