#ifndef HANDLEDSET_HPP_INCLUDED
#define HANDLEDSET_HPP_INCLUDED

#pragma GCC diagnostic ignored "-Wunused-local-typedefs" // temporarily disable warnings
	#include <boost/bimap.hpp>
	#include <boost/bimap/unordered_set_of.hpp>
	#include <boost/limits.hpp>
#pragma GCC diagnostic pop // reenable warnings

template< class T, class THash=std::hash<T>, class TEqual=std::equal_to<T> >
class HandledSet {
public:
	using Handle = size_t;

private:
	Handle nextIndex = 0;

	using MapType = boost::bimap<
		boost::bimaps::unordered_set_of<Handle>,
		boost::bimaps::unordered_set_of<T, THash, TEqual>
	>;
	using Mapping = MapType::value_type;

	MapType elements;

public:
	//using Iterator = MapType::left_iterator;
	//Iterator begin() {
	//	return elements.left.begin();
	//}
	//Iterator end() {
	//	return elements.left.end();
	//}


	Handle add(T element) {
		ASSERT(elements.size() < std::numeric_limits<size_t>::max());
		auto result = nextIndex;
		elements.insert(Mapping(result, element));

		do {
			nextIndex++;
		} while(elements.left.count(nextIndex) > 0);

		return result;
	}

	T get(Handle handle) {
		ASSERT(elements.left.count(handle));
		return elements.left.at(handle);
	}
	Handle get(T element) {
		ASSERT(elements.right.count(element));
		return elements.right.at(element);
	}

	void remove(Handle handle) {
		ASSERT(elements.left.count(handle));
		elements.left.erase(handle);
	}

	bool contains(T element) {
		return elements.right.count(element) > 0;
	}

	void modify(Handle handle, T newValue) {
		ASSERT(elements.left.count(handle));
		ASSERT(!contains(newValue));

		elements.left.erase(handle);
		elements.insert(Mapping(handle, newValue));
	}
};

#endif // HANDLEDSET_HPP_INCLUDED
