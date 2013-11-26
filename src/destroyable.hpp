#ifndef DESTROYABLE_HPP_INCLUDED
#define DESTROYABLE_HPP_INCLUDED

#include "sparsearray3.hpp"

struct Destroyable {
	bool alive = true;
	uint8_t refcount = 0;
};
using DestroyableHandle = SparseArray3<Destroyable>::Handle;
SparseArray3<Destroyable> destroyables;

DestroyableHandle MakeDestroyable() {
	return destroyables.add(Destroyable());
}

void ReferenceDestroyable(DestroyableHandle destroyable) {
	destroyable->refcount++;
}

void UnreferenceDestroyable(DestroyableHandle destroyable) {
	ASSERT(destroyable->refcount > 0);

	destroyable->refcount--;
	if(destroyable->refcount == 0)
		destroyables.remove(destroyable);
}

#endif // DESTROYABLE_HPP_INCLUDED
