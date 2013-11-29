#ifndef OBSTACLE_HPP_INCLUDED
#define OBSTACLE_HPP_INCLUDED

#include <unordered_set>
#include <unordered_map>

#include "discrete2d.hpp"
#include "sparsearray3.hpp"
#include "interpolation.hpp"
#include "destroyable.hpp"

struct Body {
	DestroyableHandle destroyable;
	Vec3i position;
};
SparseArray3<Body> bodies;
using BodyHandle = typename SparseArray3<Body>::Handle;

std::unordered_set<BodyHandle> eatables;


struct Pushable {
	InterpolandHandle x, y, z;
};

std::unordered_map<BodyHandle, Pushable> pushables;



SparseArray3<Body>::Iterator GetBodyAt(Vec3i position) {
	auto it = bodies.begin();
	for(; it != bodies.end(); it++) {
		if(!it->destroyable->alive) {
			if(eatables.count(it.getHandle()))
				eatables.erase(it.getHandle());

			if(pushables.count(it.getHandle()))
				pushables.erase(it.getHandle());

			UnreferenceDestroyable(it->destroyable);
			bodies.remove(it);
			continue;
		}
		if(it->position == position)
			return it;
	}
	return it;
}

bool IsBodyAt(Vec3i position) {
	return GetBodyAt(position) != bodies.end();
}

BodyHandle MakeBody(DestroyableHandle destroyable, Vec3i position) {
	ASSERT(!IsBodyAt(position));

	ReferenceDestroyable(destroyable);
	return bodies.add({destroyable, position});
}

void MoveBodyTo(BodyHandle body, Vec3i dest) {
	ASSERT(dest == body->position || !IsBodyAt(dest));

	body->position = dest;
}


void MakePushable(BodyHandle body, InterpolandHandle x, InterpolandHandle y, InterpolandHandle z) {
	ASSERT(pushables.count(body) == 0);

	pushables[body] = {x, y, z};
}

#endif // OBSTACLE_HPP_INCLUDED
