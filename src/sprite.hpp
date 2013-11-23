#ifndef SPRITE_HPP_INCLUDED
#define SPRITE_HPP_INCLUDED

#include <SFML/OpenGL.hpp>
#include <SFML/System/Clock.hpp>
#include "sparsearray3.hpp"
#include "interpolation.hpp"

struct Vertex {
	struct { GLfloat x, y; } position;
	struct { GLfloat r, g, b, a; } color;
};
void DrawVertices(GLenum mode, Vertex* vertices, GLushort* indices, size_t numIndices) {
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glVertexPointer(2, GL_FLOAT, sizeof(Vertex), &vertices->position);
	glColorPointer(4, GL_FLOAT, sizeof(Vertex), &vertices->color);
	glDrawElements(mode, numIndices, GL_UNSIGNED_SHORT, indices);


	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisable(GL_BLEND);
}


struct SpriteVert {
	struct { GLfloat x, y; } position;
	struct { GLfloat u, v; } tex;
};
SpriteVert spriteVerts[] = {
	{ { -.5f, -.5f }, { 0, 0 } },
	{ { .5f, -.5f }, { 1, 0 } },
	{ { .5f, .5f }, { 1, 1 } },
	{ { -.5f, .5f }, { 0, 1 } }
};


struct Sprite {
	struct { InterpolandHandle x, y; } position;
	struct { GLfloat r, g, b, a; } color;
	sf::Texture* texture;
};
using SpriteHandle = SparseArray3<Sprite, 100>::Handle;
using SASprite = SparseArray3<Sprite, 100>;

SASprite sprites;

SpriteHandle MakeSprite(InterpolandHandle x, InterpolandHandle y, sf::Texture* texture) {
	return sprites.add({ { x, y }, { 1, 1, 1, 1 }, texture });
}

void DrawSprites() {
	glEnable(GL_TEXTURE_2D);

	for(auto it = sprites.begin(); it != sprites.end(); it++) {
		sf::Texture::bind(it->texture);


		glPushMatrix();
		glTranslatef(it->position.x->currValue, it->position.y->currValue, 0);
		glBegin(GL_QUADS);

		glColor4fv(&it->color.r);

		for(int i = 0; i < 4; i++) {
			glTexCoord2fv(&spriteVerts[i].tex.u);
			glVertex2fv(&spriteVerts[i].position.x);
		}

		glEnd();
		glPopMatrix();
	}

	glDisable(GL_TEXTURE_2D);
}

#endif // SPRITE_HPP_INCLUDED
