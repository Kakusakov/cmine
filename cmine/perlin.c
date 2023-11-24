#include "perlin.h"
#include <stdlib.h>

static inline GLfloat fade(GLfloat t) {
	return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
}

static inline GLfloat lerp(GLfloat t, GLfloat a, GLfloat b) {
	return a + t * (b - a);
}

static inline GLfloat grad3(uint8_t hash, GLfloat x, GLfloat y, GLfloat z) {
	uint8_t h = hash & 15;
	GLfloat u = h < 8 ? x : y;
	GLfloat v = h < 4 ? y : (h == 12 || h == 14 ? x : z);
	return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

static inline GLfloat grad2(uint8_t hash, GLfloat x, GLfloat y) {
	//     (if (eq (bitand hash 1) 0) x (neg x))
	//     hash & 1 == 0 ? x : -x
	return ((hash & 1) == 0 ? x : -x) + ((hash & 2) == 0 ? y : -y);
}

void perlin_init_from_stdrand(PerlinSeed* seed, unsigned rand_seed) {
	for (size_t i = 0; i < 256; i++) {
		seed->p[i] = i;
	}
	srand(rand_seed);
	for (size_t i = 0; i < 255; i++) {
		size_t j = i + rand() / (RAND_MAX / (256 - i) + 1);
		uint8_t temp = seed->p[j];
		seed->p[j] = seed->p[i];
		seed->p[i] = temp;
	}
	for (size_t i = 0; i < 256; i++) {
		seed->p[i + 256] = seed->p[i];
	}
}

GLfloat perlin3(const PerlinSeed* seed, const Vec3f pos) {
	size_t xi = ((int32_t)pos.vs[0]) & 255;
	size_t yi = ((int32_t)pos.vs[1]) & 255;
	size_t zi = ((int32_t)pos.vs[2]) & 255;
	GLfloat x = pos.vs[0] - (int32_t)pos.vs[0];
	GLfloat y = pos.vs[1] - (int32_t)pos.vs[1];
	GLfloat z = pos.vs[2] - (int32_t)pos.vs[2];
	GLfloat u = fade(x);
	GLfloat v = fade(y);
	GLfloat w = fade(z);
	size_t a = seed->p[xi] + yi;
	size_t b = seed->p[xi + 1] + yi;
	size_t aa = seed->p[a] + zi;
	size_t ab = seed->p[a + 1] + zi;
	size_t ba = seed->p[b] + zi;
	size_t bb = seed->p[b + 1] + zi;

	return lerp(w,
		lerp(v,
			lerp(u, 
				grad3(seed->p[aa], x, y, z),
				grad3(seed->p[ba], x - 1, y, z)
			),
			lerp(u, 
				grad3(seed->p[ab], x, y - 1, z),
				grad3(seed->p[bb], x - 1, y - 1, z)
			)
		),
		lerp(v,
			lerp(u, 
				grad3(seed->p[aa + 1], x, y, z - 1),
				grad3(seed->p[ba + 1], x - 1, y, z - 1)
			),
			lerp(u, 
				grad3(seed->p[ab + 1], x, y - 1, z - 1),
				grad3(seed->p[bb + 1], x - 1, y - 1, z - 1)
			)
		)
	);
}

GLfloat perlin2(const PerlinSeed* seed, const Vec2f pos)
{
	size_t xi = ((int32_t)pos.vs[0]) & 255;
	size_t yi = ((int32_t)pos.vs[1]) & 255;
	GLfloat x = pos.vs[0] - (int32_t)pos.vs[0];
	GLfloat y = pos.vs[1] - (int32_t)pos.vs[1];
	GLfloat u = fade(x);
	GLfloat v = fade(y);
	size_t a = seed->p[xi] + yi;
	size_t b = seed->p[xi + 1] + yi;

	return lerp(v,
		lerp(u, 
			grad2(seed->p[a], x, y),
			grad2(seed->p[b], x - 1, y)
		),
		lerp(u, 
			grad2(seed->p[a + 1], x, y - 1),
			grad2(seed->p[b + 1], x - 1, y - 1)
		)
	);
}

GLfloat fbm3(
	const PerlinSeed* seed,
	const FBMSettings* settings,
	const Vec3f pos
) {
	GLfloat result = 0.0f;
	GLfloat frequency = settings->frequency;
	GLfloat intensity = settings->intensity;
	for (uint8_t octave = 0; octave < settings->octave_count; octave++) {
		result += perlin3(
			seed, 
			vec3f_mul(pos, frequency)
		) * intensity;
		frequency *= settings->lacunarity;
		intensity *= settings->persistance;
	}
	return result;
}

GLfloat fbm2(
	const PerlinSeed* seed,
	const FBMSettings* settings,
	const Vec2f pos
) {
	GLfloat result = 0.0f;
	GLfloat frequency = settings->frequency;
	GLfloat intensity = settings->intensity;
	for (uint8_t octave = 0; octave < settings->octave_count; octave++) {
		result += perlin2(
			seed,
			vec2f_mul(pos, frequency)
		) * intensity;
		frequency *= settings->lacunarity;
		intensity *= settings->persistance;
	}
	return result;
}
