#ifndef OBSTACLE_HPP_INCLUDED
#define OBSTACLE_HPP_INCLUDED

#include <unordered_set>

#pragma GCC diagnostic ignored "-Wunused-local-typedefs" // temporarily disable warnings
	#include <boost/bimap.hpp>
	#include <boost/bimap/unordered_set_of.hpp>
#pragma GCC diagnostic pop // reenable warnings

#include "discrete2d.hpp"
#include "sparsearray3.hpp"
#include "interpolation.hpp"
#include "destroyable.hpp"

struct Body {
	DestroyableHandle destroyable;
	Vec2i position;
};
SparseArray3<Body> bodies;
using BodyHandle = typename SparseArray3<Body>::Handle;




struct Pushable {
	BodyHandle body;
	InterpolandHandle x, y;
};
namespace std {
	template<>
	struct hash<Pushable> {
		size_t operator()(const Pushable& pushable) const {
			return hash<BodyHandle>()(pushable.body);
		}
	};

	template<>
	struct equal_to<Pushable> {
		bool operator()(const Pushable& a, const Pushable& b) const {
			return a.body->position == b.body->position;
		}
	};
}

boost::bimap<
	boost::bimaps::unordered_set_of<BodyHandle>,
	boost::bimaps::unordered_set_of<Pushable, std::hash<Pushable>>
> pushables;

using BodyPushableMapping = typename boost::bimap<
	boost::bimaps::unordered_set_of<BodyHandle>,
	boost::bimaps::unordered_set_of<Pushable, std::hash<Pushable>>
>::value_type;


std::unordered_set<BodyHandle> eatables;

SparseArray3<Body>::Iterator GetBodyAt(Vec2i position) {
	auto it = bodies.begin();
	for(; it != bodies.end(); it++) {
		if(!it->destroyable->alive) {
			if(eatables.count(it.getHandle()))
				eatables.erase(it.getHandle());

			if(pushables.left.count(it.getHandle()))
				pushables.left.erase(it.getHandle());

			UnreferenceDestroyable(it->destroyable);
			bodies.remove(it);
			continue;
		}
		if(it->position == position)
			return it;
	}
	return it;
}

bool IsBodyAt(Vec2i position) {
	return GetBodyAt(position) != bodies.end();
}

BodyHandle MakeBody(DestroyableHandle destroyable, Vec2i position) {
	ASSERT(!IsBodyAt(position));

	ReferenceDestroyable(destroyable);
	return bodies.add({destroyable, position});
}

void MoveBodyTo(BodyHandle body, Vec2i dest) {
	ASSERT(dest == body->position || !IsBodyAt(dest));

	body->position = dest;
}


void MakePushable(BodyHandle body, InterpolandHandle x, InterpolandHandle y) {
	pushables.insert(BodyPushableMapping(body, {body, x, y}));
}


#endif // OBSTACLE_HPP_INCLUDED
