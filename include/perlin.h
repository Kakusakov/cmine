#pragma once
#include <stdint.h>

// A seed for Perlin nose generatoin.
typedef struct Perlin Perlin;

Perlin *perlin_init(uint32_t seed);

float perlin3(const Perlin *perlin, float x, float y, float z);
float perlin2(const Perlin *perlin, float x, float y);
float perlin1(const Perlin *perlin, float x);

// Settings for noise generation with the fractional Brownian motion algorithm.
typedef struct Fbm Fbm;
struct Fbm
{
	int octave_count;
	float frequency;
	float intensity;
	float lacunarity;
	float persistance;
};

float fbm3(
	const Perlin *seed,
	Fbm fbm,
	float x,
	float y,
	float z);
float fbm2(
	const Perlin *seed,
	Fbm fbm,
	float x,
	float y);
float fbm1(
	const Perlin *seed,
	Fbm fbm,
	float x);
