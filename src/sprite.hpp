#ifndef SPRITE_HPP_INCLUDED
#define SPRITE_HPP_INCLUDED

#include <SFML/OpenGL.hpp>
#include <SFML/System/Clock.hpp>

#include "sparsearray3.hpp"
#include "interpolation.hpp"
#include "destroyable.hpp"
#include "assert.hpp"
#include "discrete2d.hpp"



struct SpriteVert {
	struct { GLfloat x, y, z; } position;
	struct { GLfloat u, v; } tex;
};
SpriteVert spriteVerts[] = {
	{ { -.5f, 1, 0 }, { 0, 0 } },
	{ {  .5f, 1, 0 }, { 1, 0 } },
	{ {  .5f, 0, 0 }, { 1, 1 } },
	{ { -.5f, 0, 0 }, { 0, 1 } }
};


struct Sprite {
	DestroyableHandle destroyable;
	struct { InterpolandHandle x, y, z; } position;
	InterpolandHandle xScale;
	struct { GLfloat r, g, b, a; } color;
	sf::Texture* texture;
};
using SpriteHandle = SparseArray3<Sprite, 100>::Handle;
SparseArray3<Sprite, 100> sprites;

SpriteHandle MakeSprite(
	DestroyableHandle destroyable,
	InterpolandHandle x, InterpolandHandle y, InterpolandHandle z,
	InterpolandHandle xScale,
	sf::Texture* texture
) {
	ReferenceDestroyable(destroyable);
	return sprites.add({destroyable, {x, y, z}, xScale, {1, 1, 1, 1}, texture});
}

struct Cube {
	DestroyableHandle destroyable;
	struct { InterpolandHandle x, y, z; } position;
	struct { GLfloat r, g, b, a; } color;
	sf::Texture* wallTexture;
	sf::Texture* roofTexture;
};
using CubeHandle = SparseArray3<Cube, 100>::Handle;
SparseArray3<Cube, 100> cubes;

CubeHandle MakeCube(
	DestroyableHandle destroyable,
	InterpolandHandle x, InterpolandHandle y, InterpolandHandle z,
	sf::Texture* wallTexture, sf::Texture* topTexture
) {
	ReferenceDestroyable(destroyable);
	return cubes.add({destroyable, {x, y, z}, {1, 1, 1, 1}, wallTexture, topTexture});
}




SpriteVert cubeWallVerts[] = {
	// south
	{{-.5f, 1, -.5f}, {0, 0}},
	{{ .5f, 1, -.5f}, {1, 0}},
	{{ .5f, 0, -.5f}, {1, 1}},
	{{-.5f, 0, -.5f}, {0, 1}},

	// east
	{{.5f, 1, -.5f}, {0, 0}},
	{{.5f, 1,  .5f}, {1, 0}},
	{{.5f, 0,  .5f}, {1, 1}},
	{{.5f, 0, -.5f}, {0, 1}},

	// west
	{{-.5f, 1,  .5f}, {1, 0}},
	{{-.5f, 1, -.5f}, {0, 0}},
	{{-.5f, 0, -.5f}, {0, 1}},
	{{-.5f, 0,  .5f}, {1, 1}},

	//{{.5f, 1, .5f}, {1, 0}},
	//{{-.5f, 1, .5f}, {0, 0}},
	//{{-.5f, 0, .5f}, {0, 1}},
	//{{.5f, 0, .5f}, {1, 1}}

};

SpriteVert cubeTopVerts[] = {
	{{-.5f, 1, .5f}, {0, 0}},
	{{.5f, 1, .5f}, {1, 0}},
	{{.5f, 1, -.5f}, {1, 1}},
	{{-.5f, 1, -.5f}, {0, 1}}
};

void DrawWorld() {
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	for(auto it = sprites.begin(); it != sprites.end(); it++) {
		if(!it->destroyable->alive) {
			UnreferenceDestroyable(it->destroyable);
			sprites.remove(it);
			continue;
		}

		sf::Texture::bind(it->texture);

		glPushMatrix();
		glTranslatef(it->position.x->currValue, it->position.y->currValue, it->position.z->currValue);
		glScalef(it->xScale->currValue, 1, 1);

		glBegin(GL_QUADS);

		glColor4fv(&it->color.r);

		for(int i = 0; i < 4; i++) {
			glTexCoord2fv(&spriteVerts[i].tex.u);
			glVertex3fv(&spriteVerts[i].position.x);
		}

		glEnd();
		glPopMatrix();
	}

	for(auto it = cubes.begin(); it != cubes.end(); it++) {
		if(!it->destroyable->alive) {
			UnreferenceDestroyable(it->destroyable);
			cubes.remove(it);
			continue;
		}

		sf::Texture::bind(it->wallTexture);

		glPushMatrix();
		glTranslatef(it->position.x->currValue, it->position.y->currValue, it->position.z->currValue);
		glBegin(GL_QUADS);

		glColor4fv(&it->color.r);

		for(int i = 0; i < 12; i++) {
			glTexCoord2fv(&cubeWallVerts[i].tex.u);
			glVertex3fv(&cubeWallVerts[i].position.x);
		}
		for(int i = 0; i < 4; i++) {
			glTexCoord2fv(&cubeTopVerts[i].tex.u);
			glVertex3fv(&cubeTopVerts[i].position.x);
		}

		glEnd();
		glPopMatrix();
	}

	glDisable(GL_ALPHA_TEST);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_TEXTURE_2D);
}

#endif // SPRITE_HPP_INCLUDED
