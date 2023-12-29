#pragma once
#include "fixed_types.h"
#include "perlin.h"

// I would love to have C23 constexpr for 
// compiletime constants, but C23 doesn't
// have good support yet.

#define CHUNK_SIDELEN 8

#define ATLAS_X_LEN 16
#define ATLAS_Y_LEN 16

typedef uint8_t block_id;

#define BLOCK_UNKNOWN ((block_id)0)
#define BLOCK_AIR ((block_id)1)
#define BLOCK_STONE ((block_id)2)

#define BLOCK_FIRST_ID BLOCK_UNKNOWN
#define BLOCK_LAST_ID BLOCK_STONE

typedef struct Chunk Chunk;
typedef struct Chunks Chunks;

typedef struct ChunkArea ChunkArea;
struct ChunkArea {
	int32_t min_x;
	int32_t min_y;
	int32_t min_z;

	size_t offset_x;
	size_t offset_y;
	size_t offset_z;
};

Chunks* chunks_init(Arena* arena, size_t sidelen);
void chunks_deinit(Chunks* chunks);

void chunk_area_generate_blocks(
	const ChunkArea* area,
	Chunks* chunks,
	const Perlin* perlin,
	const FBM* heightmap_fbm);
void chunk_area_generate_meshes(
	const ChunkArea* area, 
	Chunks* chunks);
