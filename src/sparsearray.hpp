#ifndef SPARSEARRAY_HPP_INCLUDED
#define SPARSEARRAY_HPP_INCLUDED

#include "assert.hpp"
#include <boost/cstdint.hpp>
#include <vector>

template <class T>
class SparseArray {
public:
	struct Handle {
		uint16_t instance;
		uint16_t index;

		Handle(uint16_t instance, uint16_t index): instance(instance), index(index) {}
	};

private:
	struct HandleInfo {
		Handle handle;

		// Does not need to be initialized because it is not relevant until the HandleInfo goes unused
		size_t nextUnused;

		HandleInfo(uint16_t instance, uint16_t index): handle(instance, index) {}
	};

    std::vector<T> elements;
    std::vector<HandleInfo> handles;
    size_t nextUnusedHandle;

public:

	T& operator[](Handle arg) {
		ASSERT(!handles.empty());
		ASSERT(arg.index < handles.size());
		ASSERT(handles[arg.index].handle.instance == arg.instance);
		return elements[handles[arg.index].handle.index];
	}

	void remove(Handle arg) {
		ASSERT(!handles.empty());
		ASSERT(arg.index < handles.size());
		ASSERT(handles[arg.index].handle.instance == arg.instance);

		uint16_t handleInfoIndex = arg.index;
		uint16_t elementIndex = handles[handleInfoIndex].handle.index;

		ASSERT(handleInfoIndex < handles.size());
		handles[handleInfoIndex].handle.instance++; // subsequent access should fail

		if(elements.size() < handles.size())
			handles[handleInfoIndex].nextUnused = nextUnusedHandle;
		nextUnusedHandle = handleInfoIndex;

		ASSERT(elementIndex < elements.size());
		elements.erase(elements.begin() + elementIndex);

		for(typename std::vector<HandleInfo>::iterator it = handles.begin(); it != handles.end(); it++)
			if(it->handle.index > elementIndex)
				it->handle.index--;
	}

	Handle add(T element) {
		size_t index = elements.size();
		elements.push_back(element);

		if(elements.size() > handles.size()) {
			handles.push_back(HandleInfo(0, index));
			return Handle(0, index);
		}

		size_t handleIndex = nextUnusedHandle;
		ASSERT(nextUnusedHandle < handles.size());

		nextUnusedHandle = handles[handleIndex].nextUnused;
		uint16_t instance = handles[handleIndex].handle.instance;
		handles[handleIndex].handle.index = index;
		return Handle(instance, handleIndex);
	}

	size_t size() const {
		return elements.size();
	}

    bool empty() const {
		return elements.empty();
	}

	typename std::vector<T>::iterator begin() {
		return elements.begin();
	}
	typename std::vector<T>::iterator end() {
		return elements.end();
	}
};


#endif // SPARSEARRAY_HPP_INCLUDED











/*template <class T, class A = std::allocator<T> >
class X {
public:
    typedef A allocator_type;
    typedef typename A::value_type value_type;
    typedef typename A::reference reference;
    typedef typename A::const_reference const_reference;
    typedef typename A::difference_type difference_type;
    typedef typename A::size_type size_type;

    class iterator {
    public:
        typedef typename A::difference_type difference_type;
        typedef typename A::value_type value_type;
        typedef typename A::reference reference;
        typedef typename A::pointer pointer;
        typedef std::random_access_iterator_tag iterator_category; //or another tag

        iterator();
        iterator(const iterator&);
        ~iterator();

        iterator& operator=(const iterator&);
        bool operator==(const iterator&) const;
        bool operator!=(const iterator&) const;
        iterator& operator++();
        reference operator*() const;
        pointer operator->() const;


        bool operator<(const iterator&) const; //optional
        bool operator>(const iterator&) const; //optional
        bool operator<=(const iterator&) const; //optional
        bool operator>=(const iterator&) const; //optional

        iterator operator++(int); //optional
        iterator& operator--(); //optional
        iterator operator--(int); //optional
        iterator& operator+=(size_type); //optional
        iterator operator+(size_type) const; //optional
        friend iterator operator+(size_type, const iterator&); //optional
        iterator& operator-=(size_type); //optional
        iterator operator-(size_type) const; //optional
        difference_type operator-(iterator) const; //optional

        reference operator[](size_type) const; //optional
    };

    X();
    X(const X&);
    ~X();

    X& operator=(const X&);
    bool operator==(const X&) const;
    bool operator!=(const X&) const;

    iterator begin();
    const_iterator begin() const;
    const_iterator cbegin() const;
    iterator end();
    const_iterator end() const;
    const_iterator cend() const;


    void swap(const X&);
    size_type size();
    size_type max_size();
    bool empty();
};*/
