#ifndef SPARSEARRAY3_HPP_INCLUDED
#define SPARSEARRAY3_HPP_INCLUDED

#include <boost/operators.hpp>
#include "assert.hpp"


using SparseArray3_Instance = uint16_t;

template<class T>
struct SparseArray3_Node {
	T datum;
	SparseArray3_Instance instance;
	SparseArray3_Node* prev;
	SparseArray3_Node* next;

	SparseArray3_Node() {}

	SparseArray3_Node(T datum): datum(datum), instance(0), prev(nullptr), next(nullptr) {}

	SparseArray3_Node(T datum, SparseArray3_Node* prev, SparseArray3_Node* next): datum(datum), instance(0), prev(prev), next(next) {}
};

template<class T>
struct SparseArray3_Handle {
	SparseArray3_Instance instance;
	SparseArray3_Node<T>* node;

	SparseArray3_Handle() {}

	SparseArray3_Handle(SparseArray3_Instance instance, SparseArray3_Node<T>* node): instance(instance), node(node) {}

	bool operator==(SparseArray3_Handle other) const {
		return instance == other.instance && node == other.node;
	}
	bool operator!=(SparseArray3_Handle other) const {
		return !(*this == other);
	}

	T& operator*() {
		ASSERT(node != nullptr);
		ASSERT(node->instance == instance);
		return node->datum;
	}

	T* operator->() {
		ASSERT(node != nullptr);
		return &(node->datum);
	}

	// TODO: Not certain this is necessary or accomplishes anything
	T* operator->() const {
		ASSERT(node != nullptr);
		return &(node->datum);
	}
};


template<class T, int allocationSize=50>
class SparseArray3 {
private:
	using Node = SparseArray3_Node<T>;

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

    void pushUnused(Node* node) {
    	ASSERT(node != nullptr);
    	ASSERT(unusedHead == nullptr || node->next != unusedHead);

		node->instance++;
		node->next = unusedHead;
		unusedHead = node;
    }
    Node* popUnused() {
		ASSERT(unusedHead != nullptr);
		ASSERT(unusedHead != usedHead);
		ASSERT(unusedHead != usedTail);

		Node* node = unusedHead;
		unusedHead = node->next;
		return node;
    }

public:
	SparseArray3():
		arr(nullptr), currentCapacity(0), nodeCount(0), elementCount(0),
		unusedHead(nullptr), usedTail(nullptr) { }
	~SparseArray3() {
		delete arr;
	}

	using Handle = SparseArray3_Handle<T>;

	class Iterator {
		friend class SparseArray3;

		private:
			Handle current;
			Handle next;

			Iterator(Handle current): current(current) {
				updateNext();
			}

			void updateNext() {
				if(current.node == nullptr)
					return;

				next.node = current.node->next;
				if(next.node != nullptr)
					next.instance = next.node->instance;
				else
					next.instance = 0;
			}

		public:
			T& operator*() {
				return *current;
			}
			T* operator->() {
				return current.operator->();
			}

			Iterator operator++(int) {
				return ++(*this);
			}
			Iterator operator++() {
				ASSERT(current.node != nullptr);
				ASSERT(next.node == nullptr || next.node->instance == next.instance);

				current = next;
				updateNext();

				return *this;
			}

			bool operator==(Iterator other) const {
				ASSERT(
					(current.node != nullptr && other.current.node != nullptr)?
						(current.node->instance == other.current.node->instance): true
				);
				return current.node == other.current.node;
			}
			bool operator!=(Iterator other) const {
				return !(*this == other);
			}

			Handle getHandle() {
				return current;
			}
	};

	T& operator[](Handle arg) {
		return *arg;
	}

	Iterator begin() {
		if(elementCount == 0)
			return Iterator(Handle(0, nullptr));
		else
			return Iterator(Handle(usedHead->instance, usedHead));
	}
	Iterator end() {
		return Iterator(Handle(0, nullptr));
	}

	void remove(Iterator arg) {
		remove(arg.current);
	}
	void remove(Handle arg) {
		ASSERT(elementCount > 0);
		ASSERT(arg.node->instance == arg.instance);

		ASSERT((usedHead == arg.node) == (arg.node->prev == nullptr));
		ASSERT((usedTail == arg.node) == (arg.node->next == nullptr));

		if(arg.node->prev == nullptr) {
			ASSERT(usedHead == arg.node);
			usedHead = arg.node->next;
			if(usedHead != nullptr)
				usedHead->prev = nullptr;
			ASSERT(!(elementCount >= 2 && usedHead == nullptr));
		}
		else {
			arg.node->prev->next = arg.node->next;
		}

		if(arg.node->next == nullptr) {
			ASSERT(usedTail == arg.node);
			usedTail = arg.node->prev;
			if(usedTail != nullptr)
				usedTail->next = nullptr;
			ASSERT(!(elementCount >= 2 && usedTail == nullptr));
		}
		else {
			arg.node->next->prev = arg.node->prev;
		}

		ASSERT(arg.node->prev != nullptr || usedHead == arg.node->next);
		ASSERT(arg.node->next != nullptr || usedTail == arg.node->prev);

		pushUnused(arg.node);

		elementCount--;
		ASSERT((elementCount == 1)? (usedHead == usedTail): true);
	}

	Handle add(T element) {
		if(nodeCount == currentCapacity) {
			arr = new NodeArray(arr);
			currentCapacity += allocationSize;
		}

		if(elementCount == nodeCount) {
			ASSERT(arr != nullptr);
			Node* node = arr->data + (elementCount % allocationSize);
			node->instance = 0;
			node->datum = element;
			node->prev = usedTail;
			node->next = nullptr;

			ASSERT((elementCount == 0) == (usedTail == nullptr));

			if(usedTail != nullptr)
				usedTail->next = node;
			usedTail = node;

			if(elementCount == 0)
				usedHead = node;
			elementCount++;
			nodeCount++;

			ASSERT((elementCount == 1) == (node->prev == nullptr));

			return Handle(0, node);
		}
		else {
			Node* node = popUnused();

			if(elementCount == 0) {
				ASSERT(usedTail == nullptr);
				usedHead = node;
			}
			else {
				ASSERT(usedTail != nullptr);
				usedTail->next = node;
			}

			node->datum = element;
			node->next = nullptr;
			node->prev = usedTail;

			usedTail = node;

			elementCount++;
			ASSERT(elementCount != 1 || usedHead == usedTail);

			ASSERT((elementCount == 1) == (node->prev == nullptr));

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

namespace std {
	template<class T>
	struct hash< SparseArray3_Handle<T> > {
		size_t operator()(const SparseArray3_Handle<T>& handle) const {
			return hash<SparseArray3_Node<T>*>()(handle.node);
		}
	};

	/*template<class T, int allocationSize>
	template< typename SparseArray3<T, allocationSize>::Handle >
	struct equal_to< typename SparseArray3<T, allocationSize>::Handle > {
		bool operator()(const SparseArray3<T, allocationSize>::Handle& a, const SparseArray3<T, allocationSize>::Handle& b) const {
			return a == b;
		}
	};*/

	template<class T>
	struct equal_to< SparseArray3_Handle<T> > {
		bool operator()(const SparseArray3_Handle<T>& a, const SparseArray3_Handle<T>& b) const {
			return a == b;
		}
	};
}

template<class T>
size_t hash_value(const SparseArray3_Handle<T>& handle) {
	return std::hash<SparseArray3_Node<T>*>()(handle.node);
}

#endif // SPARSEARRAY3_HPP_INCLUDED
