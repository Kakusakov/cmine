#pragma once
#include "perlin.h"
#include "vec_math.h"

#include "glad.h"

#include <stdint.h>
#include <stdbool.h>

#define ATLAS_X_LEN 16
#define ATLAS_Y_LEN 16

typedef uint8_t BlockId;

// I would love to have C23 constexpr, but my 
// version of msvs does not have it.

#define BLOCK_UNKNOWN ((BlockId)0)
#define BLOCK_AIR ((BlockId)1)
#define BLOCK_STONE ((BlockId)2)

#define BLOCK_FIRST_ID BLOCK_UNKNOWN
#define BLOCK_LAST_ID BLOCK_STONE

#define CHUNK_SIDELEN 8

struct Chunk {
	bool is_generated;
	// [x, y, z]
	BlockId blocks[CHUNK_SIDELEN * CHUNK_SIDELEN * CHUNK_SIDELEN];
	GLuint vao;
};
typedef struct Chunk Chunk;

struct ChunkArea {
	Vec3i32 anchor_pos;

	// [x, y, z]
	Chunk* chunks;
	size_t sidelen;

	Vec3s internal_offset;
};
typedef struct ChunkArea ChunkArea;

struct WorldGenerationSettings {
	PerlinSeed perlin_seed;
	FBMSettings heightmap_fbm;
};
typedef struct WorldGenerationSettings WorldGenerationSettings;
