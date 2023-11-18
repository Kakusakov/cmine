#pragma once
#include "perlin.h"

#include "glad.h"

#include <stdint.h>
#include <stdbool.h>

struct Uvs {
	float x_min;
	float y_min;
	float x_max;
	float y_max;
};
typedef struct Uvs Uvs;

struct Block {
	Uvs px, py, pz,
		nx, ny, nz;
};
typedef struct Block Block;

typedef uint8_t BlockId;

// I would love to have C23 constexpr, but my 
// version of msvs does not have it.
#define BLOCK_UNKNOWN ((BlockId)0)
#define BLOCK_AIR ((BlockId)1)
#define BLOCK_STONE ((BlockId)2)

#define BLOCK_FIRST_ID ((BlockId)0)
#define BLOCK_LAST_ID ((BlockId)2)

#define CHUNK_SIDELEN 8

struct Chunk {
	bool is_generated;
	// [x, y, z]
	BlockId blocks[CHUNK_SIDELEN * CHUNK_SIDELEN * CHUNK_SIDELEN];
	GLuint vao;
};
typedef struct Chunk Chunk;

struct ChunkArea {
	int32_t x_anchor;
	int32_t y_anchor;
	int32_t z_anchor;

	// [x, y, z]
	Chunk* chunks;
	size_t sidelen;

	size_t x_offset;
	size_t y_offset;
	size_t z_offset;
};
typedef struct ChunkArea ChunkArea;

struct WorldGenerationSettings {
	PerlinSeed perlin_seed;
	FBMSettings heightmap_fbm;
};
typedef struct WorldGenerationSettings WorldGenerationSettings;

BlockId* chunk_get_block_at(
	Chunk* const chunk,
	const size_t x,
	const size_t y,
	const size_t z);

Chunk* chunk_area_get_chunk_at(
	ChunkArea* const area,
	const int32_t chunk_x,
	const int32_t chunk_y,
	const int32_t chunk_z);

void chunk_generate_blocks(
	const WorldGenerationSettings* const settings,
	Chunk* const chunk,
	const int32_t chunk_x,
	const int32_t chunk_y,
	const int32_t chunk_z);
