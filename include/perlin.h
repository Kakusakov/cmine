#pragma once
#include "fixed_types.h"
#include "arena.h"

// A seed for Perlin nose generatoin.
typedef struct Perlin Perlin;

Perlin* perlin_from_stdrand(Arena* arena, unsigned rand_seed);

float32_t perlin3(const Perlin* perlin, float32_t x, float32_t y, float32_t z);
float32_t perlin2(const Perlin* perlin, float32_t x, float32_t y);
float32_t perlin1(const Perlin* perlin, float32_t x);

// Settings for noise generation with the fractional Brownian motion algorithm.
typedef struct FBM FBM;
struct FBM {
	uint8_t octave_count;
	float32_t frequency;
	float32_t intensity;
	float32_t lacunarity;
	float32_t persistance;
};

float32_t fbm3(
	const Perlin* seed,
	const FBM* fbm,
	float32_t x,
	float32_t y,
	float32_t z
);
float32_t fbm2(
	const Perlin* seed,
	const FBM* fbm,
	float32_t x,
	float32_t y
);

float32_t fbm1(
	const Perlin* seed,
	const FBM* fbm,
	float32_t x
);
