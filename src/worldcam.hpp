#ifndef WORLDCAM_HPP_INCLUDED
#define WORLDCAM_HPP_INCLUDED

#include <SFML/OpenGL.hpp>
#include <boost/limits.hpp>
#include <boost/math/constants/constants.hpp> // for pi

#include "sparsearray3.hpp"
#include "interpolation.hpp"
#include "body.hpp"
#include "destroyable.hpp"
#include "assert.hpp"

auto worldCamDestroyable = MakeDestroyable();
auto worldCamX = MakeInterpoland(worldCamDestroyable, 0);
auto worldCamY = MakeInterpoland(worldCamDestroyable, 0);
auto worldCamZ = MakeInterpoland(worldCamDestroyable, 0);
auto worldCamH = MakeInterpoland(worldCamDestroyable, 6);
auto worldCamFOV = MakeInterpoland(worldCamDestroyable, .1f); // smooth out instant window changes
auto worldCamDistance = MakeInterpoland(worldCamDestroyable, 10);
uint8_t worldCamPadding = 7;

struct WorldCamFocus {
	DestroyableHandle destroyable;
	BodyHandle body;
};
using WorldCamFocusHandle = SparseArray3<WorldCamFocus, 5>::Handle;
SparseArray3<WorldCamFocus, 5> worldCamFoci;

WorldCamFocusHandle MakeWorldCamFocus(DestroyableHandle destroyable, BodyHandle body) {
	ReferenceDestroyable(destroyable);
	return worldCamFoci.add({destroyable, body});
}

void UpdateWorldCam(sf::RenderWindow* window) {
	sf::Vector3<int16_t> minBounds(
		std::numeric_limits<int16_t>::max(),
		std::numeric_limits<int16_t>::max(),
		std::numeric_limits<int16_t>::max()
	);
	sf::Vector3<int16_t> maxBounds(
		std::numeric_limits<int16_t>::min(),
		std::numeric_limits<int16_t>::min(),
		std::numeric_limits<int16_t>::min()
	);

	for(auto it = worldCamFoci.begin(); it != worldCamFoci.end(); it++) {
		if(!it->destroyable->alive) {
			UnreferenceDestroyable(it->destroyable);
			worldCamFoci.remove(it);
			continue;
		}

		auto current = it->body->position;
		if(current.x < minBounds.x)
			minBounds.x = current.x;
		if(current.x > maxBounds.x)
			maxBounds.x = current.x;
		//if(current.y < minY)
		//	minY = current.y;
		//if(current.y > maxY)
		//	maxY = current.y;
		minBounds.y = maxBounds.y = 0;
		if(current.y < minBounds.z)
			minBounds.z = current.y;
		if(current.y > maxBounds.z)
			maxBounds.z = current.y;
	}
	if(worldCamFoci.size() == 0)
		return;

	sf::Vector3f center = sf::Vector3f(minBounds + maxBounds) / 2.0f;

	float boundingRadius = 0;
	for(auto it = worldCamFoci.begin(); it != worldCamFoci.end(); it++) {
		auto diff = sf::Vector3f(it->body->position.x, 0, it->body->position.y) - center;
		auto mag = sqrtf(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);
		boundingRadius = std::max(boundingRadius, mag);
	}

	float camDistance = boundingRadius / sinf(worldCamFOV->destValue * boost::math::constants::pi<float>());

	if(worldCamX->destValue != center.x)
		InterpolateTo(worldCamX, center.x, sf::milliseconds(1500), Tween::SINE_INOUT);
	if(worldCamY->destValue != center.y)
		InterpolateTo(worldCamY, center.y, sf::milliseconds(1500), Tween::SINE_INOUT);
	if(worldCamZ->destValue != center.z)
		InterpolateTo(worldCamZ, center.z, sf::milliseconds(1500), Tween::SINE_INOUT);
	if(worldCamDistance->destValue != camDistance)
		InterpolateTo(worldCamDistance, camDistance, sf::milliseconds(1500), Tween::SINE_INOUT);
}

void DrawWorldCam(sf::RenderWindow* window) {
	sf::Vector2u wsize = window->getSize();
	float aspect = float(wsize.x) / wsize.y;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(worldCamFOV->currValue * 360, aspect, 1, worldCamDistance->currValue * 2);

	glScalef(1, 1, -1);
	glTranslatef(0, 0, worldCamDistance->currValue);
	glRotatef(-15, 1, 0, 0);
	glTranslatef(-worldCamX->currValue, -worldCamY->currValue, -worldCamZ->currValue);

	glMatrixMode(GL_MODELVIEW);
}

#endif // WORLDCAM_HPP_INCLUDED
