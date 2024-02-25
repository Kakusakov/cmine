#include "perlin.h"
#include <stdlib.h>
#include <stddef.h>
#include <float.h>
#include <assert.h>
#define ASSERT(x) assert(x)

static inline float fade(float t)
{
	return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
}
static inline float lerp(float t, float a, float b)
{
	return a + t * (b - a);
}
static inline float grad3(uint8_t hash, float x, float y, float z)
{
	uint8_t h = hash & 15;
	float u = h < 8 ? x : y;
	float v = h < 4 ? y : (h == 12 || h == 14 ? x : z);
	return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}
static inline float grad2(uint8_t hash, float x, float y)
{
	return ((hash & 1) == 0 ? x : -x) + ((hash & 2) == 0 ? y : -y);
}
static inline float grad1(uint8_t hash, float x)
{
	return ((hash & 1) == 0 ? x : -x);
}

static uint32_t hash_u32(uint32_t u)
{
	u = (u ^ 61) ^ (u >> 16);
	u = u + (u << 3);
	u = u ^ (u >> 4);
	u = u * 0x27d4eb2d;
	u = u ^ (u >> 15);
	return u;
}

void perlin_init(Perlin* perlin, uint32_t seed)
{
	ASSERT(perlin != NULL);

	for (size_t i = 0; i < PERLIN_ARRAY_SIZE; i++) {
		perlin->p[i] = (uint8_t)i;
	}

	for (size_t i = 0; i < PERLIN_ARRAY_SIZE; i++) {
		seed = hash_u32(seed);
		size_t j = i + seed / (UINT32_MAX / (PERLIN_ARRAY_SIZE - i) + 1);  // ???
		uint8_t temp = perlin->p[j];
		perlin->p[j] = perlin->p[i];
		perlin->p[i] = temp;
	}
	for (size_t i = 0; i < PERLIN_ARRAY_SIZE; i++) {
		perlin->p[i + PERLIN_ARRAY_SIZE] = perlin->p[i];
	}
	return perlin;
}

float perlin3(const Perlin *perlin, float x, float y, float z)
{
	ASSERT(perlin != NULL);
	size_t xi = ((size_t)x) & (PERLIN_ARRAY_SIZE - 1);
	size_t yi = ((size_t)y) & (PERLIN_ARRAY_SIZE - 1);
	size_t zi = ((size_t)z) & (PERLIN_ARRAY_SIZE - 1);
	x = x - ((size_t)x);
	y = y - ((size_t)y);
	z = z - ((size_t)z);
	float u = fade(x);
	float v = fade(y);
	float w = fade(z);
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
				grad3(perlin->p[ba], x - 1.0f, y, z)),
			lerp(u, 
				grad3(perlin->p[ab], x, y - 1, z),
				grad3(perlin->p[bb], x - 1.0f, y - 1.0f, z))),
		lerp(v,
			lerp(u, 
				grad3(perlin->p[aa + 1], x, y, z - 1.0f),
				grad3(perlin->p[ba + 1], x - 1.0f, y, z - 1.0f)),
			lerp(u, 
				grad3(perlin->p[ab + 1], x, y - 1.0f, z - 1.0f),
				grad3(perlin->p[bb + 1], x - 1.0f, y - 1.0f, z - 1.0f)))
	) + 1.0f) / 2.0f;
}
float perlin2(const Perlin *perlin, float x, float y)
{
	ASSERT(perlin != NULL);
	size_t xi = ((size_t)x) & (PERLIN_ARRAY_SIZE - 1);
	size_t yi = ((size_t)y) & (PERLIN_ARRAY_SIZE - 1);
	x = x - (size_t)x;
	y = y - (size_t)y;
	float u = fade(x);
	float v = fade(y);
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
float perlin1(const Perlin *perlin, float x)
{
	ASSERT(perlin != NULL);
	size_t xi = ((size_t)x) & (PERLIN_ARRAY_SIZE - 1);
	x = x - ((size_t)x);
	float u = fade(x);
	
	return (lerp(u, 
		grad1(perlin->p[xi], x), 
		grad1(perlin->p[xi + 1], x - 1.0f)
	) + 1.0f) / 2.0f;
}

float fbm3(
	const Perlin *perlin,
	Fbm fbm,
	float x, float y, float z)
{
	ASSERT(perlin != NULL);
	float result = 0.0f;
	float frequency = fbm.frequency;
	float intensity = fbm.intensity;
	for (int octave = 0; octave < fbm.octave_count; octave++) {
		result += perlin3(
			perlin, 
			x * frequency,
			y * frequency,
			z * frequency
		) * intensity;
		frequency *= fbm.lacunarity;
		intensity *= fbm.persistance;
	}
	return result;
}
float fbm2(
	const Perlin *perlin,
	Fbm fbm,
	float x, float y)
{
	ASSERT(perlin != NULL);
	float result = 0.0f;
	float frequency = fbm.frequency;
	float intensity = fbm.intensity;
	for (int octave = 0; octave < fbm.octave_count; octave++) {
		result += perlin2(
			perlin,
			x * frequency,
			y * frequency
		) * intensity;
		frequency *= fbm.lacunarity;
		intensity *= fbm.persistance;
	}
	return result;
}
float fbm1(
	const Perlin *perlin,
	Fbm fbm,
	float x)
{
	ASSERT(perlin != NULL);
	float result = 0.0f;
	float frequency = fbm.frequency;
	float intensity = fbm.intensity;
	for (int octave = 0; octave < fbm.octave_count; octave++) {
		result += perlin1(
			perlin,
			x * frequency
		) * intensity;
		frequency *= fbm.lacunarity;
		intensity *= fbm.persistance;
	}
	return result;
}
