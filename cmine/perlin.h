#pragma once
#include "fixed_types.h"
#include "arena.h"

typedef struct PerlinSeed PerlinSeed;

PerlinSeed* perlin_init_from_stdrand(Arena* arena, unsigned rand_seed);

float32_t perlin3(const PerlinSeed* seed, float32_t x, float32_t y, float32_t z);
float32_t perlin2(const PerlinSeed* seed, float32_t x, float32_t y);
float32_t perlin1(const PerlinSeed* seed, float32_t x);

struct fbm_settings {
	uint8_t octave_count;
	float32_t frequency;
	float32_t intensity;
	float32_t lacunarity;
	float32_t persistance;
};
typedef struct fbm_settings fbm_settings;

float32_t fbm3(
	const PerlinSeed* seed,
	const fbm_settings* settings,
	float32_t x,
	float32_t y,
	float32_t z
);
float32_t fbm2(
	const PerlinSeed* seed,
	const fbm_settings* settings,
	float32_t x,
	float32_t y
);

float32_t fbm1(
	const PerlinSeed* seed,
	const fbm_settings* settings,
	float32_t x
);
