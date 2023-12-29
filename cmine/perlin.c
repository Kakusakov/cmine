#include "perlin.h"
#include <stdlib.h>

struct Perlin {
	uint8_t p[512];
};

static inline float32_t fade(float32_t t) {
	return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
}
static inline float32_t lerp(float32_t t, float32_t a, float32_t b) {
	return a + t * (b - a);
}
static inline float32_t grad3(uint8_t hash, float32_t x, float32_t y, float32_t z) {
	uint8_t h = hash & 15;
	float32_t u = h < 8 ? x : y;
	float32_t v = h < 4 ? y : (h == 12 || h == 14 ? x : z);
	return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}
static inline float32_t grad2(uint8_t hash, float32_t x, float32_t y) {
	return ((hash & 1) == 0 ? x : -x) + ((hash & 2) == 0 ? y : -y);
}
static inline float32_t grad1(uint8_t hash, float32_t x) {
	return ((hash & 1) == 0 ? x : -x);
}

Perlin* perlin_from_stdrand(Arena* arena, unsigned rand_seed) {
	Perlin* seed = arena_alloc(arena, 1, Perlin);
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
	return seed;
}
float32_t perlin3(const Perlin* perlin, float32_t x, float32_t y, float32_t z) {
	size_t xi = ((int32_t)x) & 255;
	size_t yi = ((int32_t)y) & 255;
	size_t zi = ((int32_t)z) & 255;
	x = x - ((int32_t)x);
	y = y - ((int32_t)y);
	z = z - ((int32_t)z);
	float32_t u = fade(x);
	float32_t v = fade(y);
	float32_t w = fade(z);
	size_t a = perlin->p[xi] + yi;
	size_t b = perlin->p[xi + 1] + yi;
	size_t aa = perlin->p[a] + zi;
	size_t ab = perlin->p[a + 1] + zi;
	size_t ba = perlin->p[b] + zi;
	size_t bb = perlin->p[b + 1] + zi;

	return (lerp(w,
		lerp(v,
			lerp(u, 
				grad3(perlin->p[aa], x, y, z),
				grad3(perlin->p[ba], x - 1.0f, y, z)
			),
			lerp(u, 
				grad3(perlin->p[ab], x, y - 1, z),
				grad3(perlin->p[bb], x - 1.0f, y - 1.0f, z)
			)
		),
		lerp(v,
			lerp(u, 
				grad3(perlin->p[aa + 1], x, y, z - 1.0f),
				grad3(perlin->p[ba + 1], x - 1.0f, y, z - 1.0f)
			),
			lerp(u, 
				grad3(perlin->p[ab + 1], x, y - 1.0f, z - 1.0f),
				grad3(perlin->p[bb + 1], x - 1.0f, y - 1.0f, z - 1.0f)
			)
		)
	) + 1.0f) / 2.0f;
}
float32_t perlin2(const Perlin* perlin, float32_t x, float32_t y) {
	size_t xi = ((int32_t)x) & 255;
	size_t yi = ((int32_t)y) & 255;
	x = x - (int32_t)x;
	y = y - (int32_t)y;
	float32_t u = fade(x);
	float32_t v = fade(y);
	size_t a = perlin->p[xi] + yi;
	size_t b = perlin->p[xi + 1] + yi;

	return (lerp(v,
		lerp(u, 
			grad2(perlin->p[a], x, y),
			grad2(perlin->p[b], x - 1.0f, y)
		),
		lerp(u, 
			grad2(perlin->p[a + 1], x, y - 1.0f),
			grad2(perlin->p[b + 1], x - 1.0f, y - 1.0f)
		)
	) + 1.0f) / 2.0f;
}
float32_t perlin1(const Perlin* perlin, float32_t x) {
	size_t xi = ((int32_t)x) & 255;
	x = x - ((float32_t)(int32_t)x);
	float32_t u = fade(x);
	
	return (lerp(u, 
		grad1(perlin->p[xi], x), 
		grad1(perlin->p[xi + 1], x - 1.0f)
	) + 1.0f) / 2.0f;
}

float32_t fbm3(
	const Perlin* perlin,
	const FBM* fbm,
	float32_t x,
	float32_t y,
	float32_t z
) {
	float32_t result = 0.0f;
	float32_t frequency = fbm->frequency;
	float32_t intensity = fbm->intensity;
	for (uint8_t octave = 0; octave < fbm->octave_count; octave++) {
		result += perlin3(
			perlin, 
			x * frequency,
			y * frequency,
			z * frequency
		) * intensity;
		frequency *= fbm->lacunarity;
		intensity *= fbm->persistance;
	}
	return result;
}
float32_t fbm2(
	const Perlin* perlin,
	const FBM* fbm,
	float32_t x,
	float32_t y
) {
	float32_t result = 0.0f;
	float32_t frequency = fbm->frequency;
	float32_t intensity = fbm->intensity;
	for (uint8_t octave = 0; octave < fbm->octave_count; octave++) {
		result += perlin2(
			perlin,
			x * frequency,
			y * frequency
		) * intensity;
		frequency *= fbm->lacunarity;
		intensity *= fbm->persistance;
	}
	return result;
}
float32_t fbm1(
	const Perlin* perlin,
	const FBM* fbm,
	float32_t x
) {
	float32_t result = 0.0f;
	float32_t frequency = fbm->frequency;
	float32_t intensity = fbm->intensity;
	for (uint8_t octave = 0; octave < fbm->octave_count; octave++) {
		result += perlin1(
			perlin,
			x * frequency
		) * intensity;
		frequency *= fbm->lacunarity;
		intensity *= fbm->persistance;
	}
	return result;
}
