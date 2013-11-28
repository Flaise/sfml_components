#ifndef SPRITE_HPP_INCLUDED
#define SPRITE_HPP_INCLUDED

#include <SFML/OpenGL.hpp>
#include <SFML/System/Clock.hpp>

#include "sparsearray3.hpp"
#include "interpolation.hpp"
#include "destroyable.hpp"
#include "assert.hpp"


struct SpriteVert {
	struct { GLfloat x, y, z; } position;
	struct { GLfloat u, v; } tex;
};
SpriteVert spriteVerts[] = {
	{ { -.5f, 1, .5f }, { 0, 0 } },
	{ { .5f, 1, .5f }, { 1, 0 } },
	{ { .5f, 0, .5f }, { 1, 1 } },
	{ { -.5f, 0, .5f }, { 0, 1 } }
};


struct Sprite {
	DestroyableHandle destroyable;
	struct { InterpolandHandle x, y, z; } position;
	struct { GLfloat r, g, b, a; } color;
	sf::Texture* texture;
};
using SpriteHandle = SparseArray3<Sprite, 100>::Handle;
using SASprite = SparseArray3<Sprite, 100>;

SASprite sprites;

SpriteHandle MakeSprite(
	DestroyableHandle destroyable, InterpolandHandle x, InterpolandHandle y, InterpolandHandle z, sf::Texture* texture
) {
	ReferenceDestroyable(destroyable);
	return sprites.add({destroyable, { x, y, z }, { 1, 1, 1, 1 }, texture});
}

void DrawSprites() {
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0);

	for(auto it = sprites.begin(); it != sprites.end(); it++) {
		if(!it->destroyable->alive) {
			UnreferenceDestroyable(it->destroyable);
			sprites.remove(it);
			continue;
		}

		sf::Texture::bind(it->texture);

		glPushMatrix();
		glTranslatef(it->position.x->currValue, it->position.y->currValue, it->position.z->currValue);
		glBegin(GL_QUADS);

		glColor4fv(&it->color.r);

		for(int i = 0; i < 4; i++) {
			glTexCoord2fv(&spriteVerts[i].tex.u);
			glVertex3fv(&spriteVerts[i].position.x);
		}

		glEnd();
		glPopMatrix();
	}

	glDisable(GL_ALPHA_TEST);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_TEXTURE_2D);
}

#endif // SPRITE_HPP_INCLUDED
