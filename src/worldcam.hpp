#ifndef WORLDCAM_HPP_INCLUDED
#define WORLDCAM_HPP_INCLUDED

#include <SFML/OpenGL.hpp>
#include <boost/limits.hpp>

#include "sparsearray3.hpp"
#include "interpolation.hpp"
#include "body.hpp"
#include "destroyable.hpp"
#include "assert.hpp"

auto worldCamDestroyable = MakeDestroyable();
auto worldCamX = MakeInterpoland(worldCamDestroyable, 0);
auto worldCamY = MakeInterpoland(worldCamDestroyable, 0);
auto worldCamH = MakeInterpoland(worldCamDestroyable, 6);
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
	if(worldCamFoci.size() == 0)
		return;

	int16_t minX = std::numeric_limits<int16_t>::max();
	int16_t maxX = std::numeric_limits<int16_t>::min();
	int16_t minY = std::numeric_limits<int16_t>::max();
	int16_t maxY = std::numeric_limits<int16_t>::min();

	for(auto it = worldCamFoci.begin(); it != worldCamFoci.end(); it++) {
		if(!it->destroyable->alive) {
			UnreferenceDestroyable(it->destroyable);
			worldCamFoci.remove(it);
			continue;
		}

		auto current = it->body->position;
		if(current.x < minX)
			minX = current.x;
		if(current.x > maxX)
			maxX = current.x;
		if(current.y < minY)
			minY = current.y;
		if(current.y > maxY)
			maxY = current.y;
	}

	float centerX = (minX + maxX) / 2.0f;
	float centerY = (minY + maxY) / 2.0f;

	if(worldCamX->destValue != centerX)
		InterpolateTo(worldCamX, centerX, sf::milliseconds(1500), Tween::SINE_INOUT);
	if(worldCamY->destValue != centerY)
		InterpolateTo(worldCamY, centerY, sf::milliseconds(1500), Tween::SINE_INOUT);


	float height = maxY - minY + worldCamPadding;

	sf::Vector2u wsize = window->getSize();
	float aspect = float(wsize.x) / wsize.y;
	float xHeight = (maxX - minX + worldCamPadding) / aspect;

	float usingHeight = (height > xHeight)? height: xHeight;

	if(worldCamH->destValue != usingHeight)
		InterpolateTo(worldCamH, usingHeight, sf::milliseconds(1500), Tween::SINE_INOUT);
}

void DrawWorldCam(sf::RenderWindow* window) {
	sf::Vector2u wsize = window->getSize();
	float aspect = float(wsize.x) / wsize.y;
	float halfWidth = aspect * worldCamH->currValue / 2;
	float halfHeight = .5 * worldCamH->currValue;

	/*glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(
		worldCamX->currValue - halfWidth, // left
		worldCamX->currValue + halfWidth, // right
		worldCamY->currValue + halfHeight, // bottom
		worldCamY->currValue - halfHeight, // top
		-1, // near
		1 // far
	);
	glMatrixMode(GL_MODELVIEW);*/


	//glFrustum(-1.0, 1.0, -1.0, 1.0, 5, 100);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//void glFrustum(GLdouble  left,  GLdouble  right,  GLdouble  bottom,  GLdouble  top,  GLdouble  nearVal,  GLdouble  farVal);

	glFrustum(
		//worldCamX->currValue - halfWidth, // left
		//worldCamX->currValue + halfWidth, // right
		//worldCamY->currValue + halfHeight, // bottom
		//worldCamY->currValue - halfHeight, // top

		-aspect/2 * .5f, // left
		aspect/2 * .5f, // right
		-.5f * .5f, // bottom
		.5f * .5f, // top

		1, // near
		20 // far

			//left
			//right
			//bottom
			//top
			//near
			//far
	);
	glScalef(1, 1, -1);
	glRotatef(-15, 1, 0, 0);
	glTranslatef(0, -2, 10);

	glMatrixMode(GL_MODELVIEW);
}

#endif // WORLDCAM_HPP_INCLUDED
