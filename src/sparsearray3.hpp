#ifndef SPARSEARRAY3_HPP_INCLUDED
#define SPARSEARRAY3_HPP_INCLUDED

#include "assert.hpp"


template<class T, int allocationSize=50>
class SparseArray3 {
private:
	using Index = int16_t;
	using Instance = uint16_t;

    struct Node {
    	T datum;
    	Instance instance;
    	Node* prev;
    	Node* next;

    	Node() {}

    	Node(T datum): datum(datum), instance(0), prev(nullptr), next(nullptr) {}

    	Node(T datum, Node* prev, Node* next): datum(datum), instance(0), prev(prev), next(next) {}
    };

    struct NodeArray {
    	Node data[allocationSize];
    	NodeArray* next;

    	NodeArray(NodeArray* next): next(next) {}

    	~NodeArray() {
			if(next)
				delete next;
    	}
    };

    NodeArray* arr;
    size_t currentCapacity;
    size_t nodeCount;
    size_t elementCount;
    Node* unusedHead;
    Node* usedHead;
    Node* usedTail;

public:
	SparseArray3():
		arr(nullptr), currentCapacity(0), nodeCount(0), elementCount(0), usedTail(nullptr) { }
	~SparseArray3() {
		delete arr;
	}

	struct Handle {
		Instance instance;
		Node* node;

		Handle(Instance instance, Node* node): instance(instance), node(node) {}

		bool operator==(Handle other) {
			return instance == other.instance && node == other.node;
		}
		bool operator!=(Handle other) {
			return !(*this == other);
		}

		T& operator*() {
			ASSERT(node != nullptr);
			ASSERT(node->instance == instance);
			return node->datum;
		}

		Handle operator++(int) {
			return ++(*this);
		}
		Handle operator++() {
			ASSERT(node != nullptr);
			ASSERT(node->instance == instance);

			node = node->next;
			if(node != nullptr)
				instance = node->instance;
			else
				instance = 0;
			return *this;
		}
	};

	T& operator[](Handle arg) {
		return *arg;
	}

	Handle begin() {
		if(elementCount == 0)
			return Handle(0, nullptr);
		else
			return Handle(usedHead->instance, usedHead);
	}
	Handle end() {
		return Handle(0, nullptr);
	}

	void remove(Handle arg) {
		ASSERT(elementCount > 0);
		ASSERT(arg.node->instance == arg.instance);

		if(arg.node->prev == nullptr) {
			usedHead = arg.node->next;
			ASSERT(!(elementCount >= 2 && usedHead == nullptr));
		}
		else {
			ASSERT(arg.node->prev != nullptr);
			arg.node->prev->next = arg.node->next;
		}

		if(arg.node->next == nullptr) {
			usedTail = arg.node->prev;
			ASSERT(!(elementCount >= 2 && usedTail == nullptr));
		}
		else {
			ASSERT(arg.node->next != nullptr);
			arg.node->next->prev = arg.node->prev;
		}

		arg.node->instance++;
		arg.node->next = unusedHead;
		unusedHead = arg.node;

		elementCount--;
	}

	Handle add(T element) {
		if(nodeCount == currentCapacity) {
			arr = new NodeArray(arr);
			currentCapacity += allocationSize;
		}

		if(elementCount == nodeCount) {
			Node* node = arr->data + (elementCount % allocationSize);
			node->instance = 0;
			node->datum = element;
			node->prev = usedTail;
			node->next = nullptr;
			if(usedTail != nullptr)
				usedTail->next = node;
			usedTail = node;

			if(elementCount == 0)
				usedHead = node;
			elementCount++;
			nodeCount++;

			return Handle(0, node);
		}
		else {
			ASSERT(unusedHead != nullptr);

			Node* node = unusedHead;
			unusedHead = node->next;

			if(elementCount == 0) {
				usedHead = node;
			}
			else {
				usedTail->next = node;
			}
			usedTail = node;

			node->datum = element;
			node->next = nullptr;
			node->prev = usedTail;

			elementCount++;

			return Handle(node->instance, node);
		}
	}


	size_t size() const {
		return elementCount;
	}

	bool empty() const {
		return size() == 0;
	}
};

#endif // SPARSEARRAY3_HPP_INCLUDED
