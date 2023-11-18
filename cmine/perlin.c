#include "perlin.h"
#include <stdlib.h>

static inline float fade(float t) {
	return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
}

static inline float lerp(float t, float a, float b) {
	return a + t * (b - a);
}

static inline float grad3(uint8_t hash, float x, float y, float z) {
	uint8_t h = hash & 15;
	float u = h < 8 ? x : y;
	float v = h < 4 ? y : (h == 12 || h == 14 ? x : z);
	return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

static inline float grad2(uint8_t hash, float x, float y) {
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

float perlin3(const PerlinSeed* seed, float x, float y, float z) {
	int xi = ((int)x) & 255;
	int yi = ((int)y) & 255;
	int zi = ((int)z) & 255;
	x -= (int)x;
	y -= (int)y;
	z -= (int)z;
	float u = fade(x);
	float v = fade(y);
	float w = fade(z);
	int a = seed->p[xi] + yi;
	int b = seed->p[xi + 1] + yi;
	int aa = seed->p[a] + zi;
	int ab = seed->p[a + 1] + zi;
	int ba = seed->p[b] + zi;
	int bb = seed->p[b + 1] + zi;

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

float perlin2(const PerlinSeed* seed, float x, float y)
{
	int xi = (int)x & 255;
	int yi = (int)y & 255;
	x -= (int)x;
	y -= (int)y;
	float u = fade(x);
	float v = fade(y);
	int a = seed->p[xi] + yi;
	int b = seed->p[xi + 1] + yi;

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

float fbm3(
	const PerlinSeed* seed,
	const FBMSettings* settings,
	float x, 
	float y, 
	float z
) {
	float result = 0.0f;
	float frequency = settings->frequency;
	float intensity = settings->intensity;
	for (uint8_t octave = 0; octave < settings->octave_count; octave++) {
		result += perlin3(
			seed, 
			x * frequency, 
			y * frequency, 
			z * frequency
		) * intensity;
		frequency *= settings->lacunarity;
		intensity *= settings->persistance;
	}
	return result;
}

float fbm2(
	const PerlinSeed* seed,
	const FBMSettings* settings,
	float x,
	float y
) {
	float result = 0.0f;
	float frequency = settings->frequency;
	float intensity = settings->intensity;
	for (uint8_t octave = 0; octave < settings->octave_count; octave++) {
		result += perlin2(
			seed,
			x * frequency,
			y * frequency
		) * intensity;
		frequency *= settings->lacunarity;
		intensity *= settings->persistance;
	}
	return result;
}
