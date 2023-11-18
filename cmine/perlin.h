#pragma once
#include <stdint.h>

struct PerlinSeed {
	uint8_t p[512];
};
typedef struct PerlinSeed PerlinSeed;

void perlin_init_from_stdrand(
	PerlinSeed* seed, 
	unsigned rand_seed
);
float perlin3(
	const PerlinSeed* seed, 
	float x, 
	float y, 
	float z
);
float perlin2(
	const PerlinSeed* seed, 
	float x, 
	float y
);

struct FBMSettings {
	uint8_t octave_count;
	float frequency;
	float intensity;
	float lacunarity;
	float persistance;
};
typedef struct FBMSettings FBMSettings;

float fbm3(
	const PerlinSeed* seed,
	const FBMSettings* settings,
	float x,
	float y,
	float z
);
float fbm2(
	const PerlinSeed* seed,
	const FBMSettings* settings,
	float x,
	float y
);
