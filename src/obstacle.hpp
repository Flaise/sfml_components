#ifndef OBSTACLE_HPP_INCLUDED
#define OBSTACLE_HPP_INCLUDED

#include "handledset.hpp"
#include "discrete2d.hpp"
#include "sparsearray3.hpp"

HandledSet<Vec2i, Vec2iHash, Vec2iEqual> obstacles;
using ObstacleHandle = HandledSet<Vec2i, Vec2iHash, Vec2iEqual>::Handle;


using PushableHandle = SparseArray3<ObstacleHandle, 20>::Handle;
SparseArray3<ObstacleHandle, 20> pushables;

#endif // OBSTACLE_HPP_INCLUDED
