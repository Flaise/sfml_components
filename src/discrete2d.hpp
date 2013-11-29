#ifndef MATH2D_HPP_INCLUDED
#define MATH2D_HPP_INCLUDED

enum class Direction4: unsigned char {
	NORTH, EAST, SOUTH, WEST, NONE
};
Direction4 RandomDirection4() {
	return static_cast<Direction4>(rand() % 4);
}

enum class Direction2: unsigned char {
	LEFT, RIGHT, NONE
};

#endif // MATH2D_HPP_INCLUDED
