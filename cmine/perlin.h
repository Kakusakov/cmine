#pragma once
#include "fixed_types.h"
#include "vec_math.h"

typedef struct PerlinSeed PerlinSeed;
struct PerlinSeed {
	uint8_t p[512];
};

void perlin_init_from_stdrand(
	PerlinSeed* seed, 
	unsigned rand_seed
);
GLfloat perlin3(
	const PerlinSeed* seed,
	const Vec3f pos
);
GLfloat perlin2(
	const PerlinSeed* seed, 
	const Vec2f pos
);

struct FBMSettings {
	uint8_t octave_count;
	float32_t frequency;
	float32_t intensity;
	float32_t lacunarity;
	float32_t persistance;
};
typedef struct FBMSettings FBMSettings;

float32_t fbm3(
	const PerlinSeed* seed,
	const FBMSettings* settings,
	const Vec3f pos
);
float32_t fbm2(
	const PerlinSeed* seed,
	const FBMSettings* settings,
	const Vec2f pos
);
