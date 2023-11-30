#pragma once
#include "fixed_types.h"
#include "vec.h"
#include "perlin.h"

#include "glad.h"

#define ATLAS_X_LEN 16
#define ATLAS_Y_LEN 16

typedef uint8_t block_id;

// I would love to have C23 constexpr, but my 
// version of msvs does not have it.

#define BLOCK_UNKNOWN ((block_id)0)
#define BLOCK_AIR ((block_id)1)
#define BLOCK_STONE ((block_id)2)

#define BLOCK_FIRST_ID BLOCK_UNKNOWN
#define BLOCK_LAST_ID BLOCK_STONE

#define CHUNK_SIDELEN 8

typedef struct Chunk Chunk;
typedef struct ChunkArea ChunkArea;

//struct world_generation_settings {
//	PerlinSeed* perlin_seed;
//	FBMSettings heightmap_fbm;
//};
//typedef struct world_generation_settings world_generation_settings;
