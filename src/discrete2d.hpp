#ifndef MATH2D_HPP_INCLUDED
#define MATH2D_HPP_INCLUDED

#pragma GCC diagnostic ignored "-Wunused-local-typedefs" // temporarily disable warnings
	#include <boost/cstdint.hpp>
	#include <boost/operators.hpp>
#pragma GCC diagnostic pop // reenable warnings

struct Vec2i: boost::addable<Vec2i> {
	int16_t x, y;

	Vec2i() {}
	Vec2i(int16_t x, int16_t y): x(x), y(y) {}

	Vec2i& operator+=(const Vec2i& other) {
		this->x += other.x;
		this->y += other.y;
		return *this;
	}
};
namespace std {
	template<>
	struct hash<Vec2i> {
		size_t operator()(const Vec2i& vec) const {
			return static_cast<size_t>(vec.x) ^ (static_cast<size_t>(vec.y) << 16);
		}
	};

	template<>
	struct equal_to<Vec2i> {
		bool operator()(const Vec2i& a, const Vec2i& b) const {
			return a.x == b.x && a.y == b.y;
		}
	};
}


enum class Direction4: unsigned char {
	NORTH, EAST, SOUTH, WEST, NONE
};
Direction4 RandomDirection4() {
	return static_cast<Direction4>(rand() % 4);
}

#endif // MATH2D_HPP_INCLUDED
