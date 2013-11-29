#ifndef ANGLE_HPP_INCLUDED
#define ANGLE_HPP_INCLUDED

#include <boost/math/constants/constants.hpp> // for pi

const float PI = boost::math::constants::pi<float>();
const float PI_OVER_TWO = PI / 2;
const float TWO_PI = 2 * PI;


// conversions to/from revolutions

inline float rev2rad(float rev) {
	return rev * TWO_PI;
}
inline float rad2rev(float rad) {
	return rad / TWO_PI;
}
inline float rev2deg(float rev) {
	return rev * 360;
}
inline float deg2rev(float deg) {
	return deg / 360;
}

inline float sinr(float rev) {
	return sinf(rev2rad(rev));
}
inline float cosr(float rev) {
	return cosf(rev2rad(rev));
}
inline float tanr(float rev) {
	return tanf(rev2rad(rev));
}

#endif // ANGLE_HPP_INCLUDED
