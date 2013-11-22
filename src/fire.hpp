#ifndef FIRE_HPP_INCLUDED
#define FIRE_HPP_INCLUDED

constexpr size_t fireWidth = 100;
constexpr size_t fireHeight = 100;
constexpr size_t fireVertCount = fireWidth * fireHeight;
unsigned char fireIntensity[fireWidth * fireHeight];

Vertex fireVerts[fireVertCount];

constexpr size_t fireCellCount = (fireWidth - 1) * (fireHeight - 1);

GLushort fireIndices[fireCellCount * 4];
constexpr int fireIndexCount = fireCellCount * 4;

sf::Time fireTimer = sf::milliseconds(0);

void MakeFire() {
	GLushort* fireIndex = fireIndices;
	for(int x = 0; x < fireWidth - 1; x++)
		for(int y = 0; y < fireHeight - 1; y++) {

			fireVerts[x + y * fireWidth].position = { float(x) / (fireWidth - 1) - .5, float(y) / (fireHeight - 1) - .5 };
			*fireIndex = x + y * fireWidth;
			fireIndex++;

			fireVerts[x + 1 + y * fireWidth].position = { float(x + 1) / (fireWidth - 1) - .5, float(y) / (fireHeight - 1) - .5 };
			*fireIndex = x + 1 + y * fireWidth;
			fireIndex++;

			fireVerts[x + 1 + (y + 1) * fireWidth].position = { float(x + 1) / (fireWidth - 1) - .5, float(y + 1) / (fireHeight - 1) - .5 };
			*fireIndex = x + 1 + (y + 1) * fireWidth;
			fireIndex++;

			fireVerts[x + (y + 1) * fireWidth].position = { float(x) / (fireWidth - 1) - .5, float(y + 1) / (fireHeight - 1) - .5 };
			*fireIndex = x + (y + 1) * fireWidth;
			fireIndex++;

			ASSERT(fireIndex <= fireIndices + fireIndexCount);
		}
}
void UpdateFire(sf::Time dt) {
	fireTimer += dt;
	if(fireTimer < sf::milliseconds(20))
		return;
	fireTimer -= sf::milliseconds(20);

	for(int x = 0; x < fireWidth; x++)
		for(int y = 0; y < fireHeight; y++) {
			int average = 0;
			int ct = 0;

			average += fireIntensity[x + y * fireWidth];
			ct++;

			if(x > 0) {
				average += fireIntensity[x - 1 + y * fireWidth];
			}
			ct++;
			if(x < fireWidth - 1) {
				average += fireIntensity[x + 1 + y * fireWidth];
			}
			ct++;

			if(y > 0) {
				average += fireIntensity[x + (y - 1) * fireWidth];
				ct++;
			}
			if(y < fireHeight - 1) {
				average += fireIntensity[x + (y + 1) * fireWidth] * 3;
				ct += 3;
			}
			average /= ct;

			if(y == fireHeight - 1)
				average += rand() % 12;
			else {
				average += (rand() % 15 == 0)? (rand() % 90 - 70): 0;

				if(average > 128)
					average += 1;
				else
					average -= 1;
			}

			if(average > 255)
				average = 255;
			else if(average < 0)
				average = 0;

			fireIntensity[x + y * fireWidth] = average;
		}

	for(int i = 0; i < fireVertCount; i++) {
		fireVerts[i].color = {
			fireIntensity[i] / 255.0f,
			fireIntensity[i] / 255.0f / 2,
			fireIntensity[i] / 255.0f / 5,
			fireIntensity[i] / 255.0f
		};
	}
}
void DrawFire() {
	DrawVertices(GL_QUADS, fireVerts, fireIndices, fireIndexCount);
}

#endif // FIRE_HPP_INCLUDED
