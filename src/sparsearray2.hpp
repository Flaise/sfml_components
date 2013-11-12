#ifndef SPARSEARRAY2_HPP_INCLUDED
#define SPARSEARRAY2_HPP_INCLUDED

#include <boost/cstdint.hpp>
#include <vector>


//template<class T>
//typedef typename std::vector<T>::iterator Index

//template<class T>
//using Index = typename std::vector<T>::iterator;


template<class T>
class SparseArray2 {
private:
	//using Index = typename std::vector<T>::iterator;
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
    Index unusedHead;
    Index usedHead;
    Index usedTail;
    size_t currentSize;

public:
	SparseArray2(): currentSize(0), usedTail(-1) { }

	struct Handle {
		Instance instance;
		Index index;

		Handle(Instance instance, Index index): instance(instance), index(index) {}
	};

	T& operator[](Handle arg) {
		if(currentSize == 0)
			throw 1;
		if(elements[arg.index].instance != arg.instance)
			throw 1;
		return elements[arg.index].datum;
	}

	void remove(Handle arg) {
		if(currentSize == 0)
			throw 1;
		if(elements[arg.index].instance != arg.instance)
			throw 1;

		if(elements[arg.index].prev == -1)
			usedHead = elements[arg.index].next;
		else
			elements[elements[arg.index].prev].next = elements[arg.index].next;

		if(elements[arg.index].next == -1)
			usedTail = elements[arg.index].prev;
		else
			elements[elements[arg.index].next].prev = elements[arg.index].prev;

		elements[arg.index].instance++;
		elements[arg.index].prev = -1;
		elements[arg.index].next = unusedHead;
		unusedHead = arg.index;

		currentSize--;
	}

	Handle add(T element) {
		if(currentSize == elements.size()) {
			elements.push_back(Node(element, usedTail, -1));
			if(currentSize != 0)
				elements[usedTail].next = currentSize;
			usedTail = currentSize;
			currentSize++;
			return Handle(0, usedTail);
		}
		else {
			Index nodeIndex = unusedHead;
			unusedHead = elements[nodeIndex].next;

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
