#ifndef OBSTACLE_HPP_INCLUDED
#define OBSTACLE_HPP_INCLUDED

#pragma GCC diagnostic ignored "-Wunused-local-typedefs" // temporarily disable warnings
	#include <boost/bimap.hpp>
	#include <boost/bimap/unordered_set_of.hpp>
	#include <boost/limits.hpp>
	#include <boost/cstdint.hpp>
#pragma GCC diagnostic pop // reenable warnings

#include "discrete2d.hpp"

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

ObstacleIndex MakeObstacle(int16_t x, int16_t y) {
	ASSERT(obstacles.size() < std::numeric_limits<size_t>::max());
	auto result = nextObstacleIndex;
	obstacles.insert(obstacle_v(result, {x, y}));
	do {
		nextObstacleIndex++;
	} while(obstacles.left.count(nextObstacleIndex) > 0);

	return result;
}

#endif // OBSTACLE_HPP_INCLUDED
