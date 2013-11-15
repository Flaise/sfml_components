#ifndef SPARSEARRAY2_HPP_INCLUDED
#define SPARSEARRAY2_HPP_INCLUDED

#include <boost/cstdint.hpp>
#include <vector>

template<class T>
class SparseArray2 {
private:
	using Index = int16_t;
	using Instance = uint16_t;

    struct Node {
    	T datum;
    	Instance instance;
    	Index prev;
    	Index next;

    	Node(T datum): datum(datum), instance(0), prev(-1), next(-1) {}

    	Node(T datum, Index prev, Index next): datum(datum), instance(0), prev(prev), next(next) {}
    };

    std::vector<Node> elements;
    size_t currentSize;
    Index unusedHead;
    Index usedHead;
    Index usedTail;

public:
	SparseArray2(): currentSize(0), usedTail(-1) { }

	struct Handle {
		Instance instance;
		Index index;

		Handle(Instance instance, Index index): instance(instance), index(index) {}

		bool operator==(Handle other) {
			return instance == other.instance && index == other.index;
		}
	};

	struct Iterator {
		SparseArray2* arr;
		Handle handle;

		Iterator(SparseArray2* arr, Instance instance, Index index): arr(arr), handle(Handle(instance, index)) {}

		Iterator operator++(int) {
			return ++(*this);
		}
		Iterator operator++() {
			#ifdef DEBUG
				if(handle.index < 0)
					throw 1;
				if(arr->elements[handle.index].instance != handle.instance)
					throw 1;
			#endif
			handle.index = arr->elements[handle.index].next;
			if(handle.index >= 0)
				handle.instance = arr->elements[handle.index].instance;
			else
				handle.instance = 0;
			return *this;
		}
		bool operator==(Iterator other) {
			return arr == other.arr && handle == other.handle;
		}
		bool operator!=(Iterator other) {
			return !(*this == other);
		}
		T& operator*() {
			if(arr->currentSize == 0)
				throw 1;
			return (*arr)[handle];
		}
	};

	Iterator begin() {
		if(currentSize == 0)
			return Iterator(this, 0, -1);
		else
			return Iterator(this, elements[usedHead].instance, usedHead);
	}
	Iterator end() {
		return Iterator(this, 0, -1);
	}


	T& operator[](Handle arg) {
		#ifdef DEBUG
			if(elements[arg.index].instance != arg.instance)
				throw 1;
		#endif
		return elements[arg.index].datum;
	}

	void remove(Handle arg) {
		#ifdef DEBUG
			if(currentSize == 0)
				throw 1;
			if(elements[arg.index].instance != arg.instance)
				throw 1;
		#endif

		if(elements[arg.index].prev == -1)
			usedHead = elements[arg.index].next;
		else
			elements[elements[arg.index].prev].next = elements[arg.index].next;

		if(elements[arg.index].next == -1)
			usedTail = elements[arg.index].prev;
		else
			elements[elements[arg.index].next].prev = elements[arg.index].prev;

		elements[arg.index].instance++;
		//elements[arg.index].prev = -1; ///////////////////////////////////////// necessary?
		elements[arg.index].next = unusedHead;
		unusedHead = arg.index;

		currentSize--;
	}

	Handle add(T element) {
		if(currentSize == elements.size()) {
			elements.push_back(Node(element, usedTail, -1));
			if(currentSize != 0)
				elements[usedTail].next = currentSize;
			else
				usedHead = 0;
			usedTail = currentSize;
			currentSize++;
			return Handle(0, usedTail);
		}
		else {
			Index nodeIndex = unusedHead;
			unusedHead = elements[nodeIndex].next;

			if(currentSize == 0)
				usedHead = nodeIndex;

			elements[nodeIndex].datum = element;
			elements[nodeIndex].next = -1;
			elements[nodeIndex].prev = usedTail;
			elements[usedTail].next = nodeIndex;
			usedTail = nodeIndex;

			currentSize++;

			return Handle(elements[nodeIndex].instance, nodeIndex);
		}
	}


	size_t size() {
		return currentSize;
	}

	bool empty() {
		return size() == 0;
	}
};

#endif // SPARSEARRAY2_HPP_INCLUDED
