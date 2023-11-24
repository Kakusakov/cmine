#pragma once
#include <stdint.h>
#include "glad.h"
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
	GLfloat frequency;
	GLfloat intensity;
	GLfloat lacunarity;
	GLfloat persistance;
};
typedef struct FBMSettings FBMSettings;

GLfloat fbm3(
	const PerlinSeed* seed,
	const FBMSettings* settings,
	const Vec3f pos
);
GLfloat fbm2(
	const PerlinSeed* seed,
	const FBMSettings* settings,
	const Vec2f pos
);
