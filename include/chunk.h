#pragma once
#include "perlin.h"
#include "block.h"
#include "render.h"

typedef enum ChunkGenerationStage ChunkGenerationStage;
enum ChunkGenerationStage 
{
	chunk_generation_stage_awaits_blocks,
	chunk_generation_stage_awaits_mesh,
	chunk_generation_stage_ready,
	chunk_generation_stage_count
};

#define CHUNK_SIDELEN 8

typedef struct Chunk Chunk;
struct Chunk
{
	Block blocks[CHUNK_SIDELEN * CHUNK_SIDELEN * CHUNK_SIDELEN];
	Mesh mesh;
	ChunkGenerationStage generation_stage;
};

#define CHUNK_BLOCK_IDX(x, y, z) (z * CHUNK_SIDELEN * CHUNK_SIDELEN + y * CHUNK_SIDELEN + x)
#define CHUNK_BLOCK_IDX_V(v) CHUNK_BLOCK_IDX(v.x, v.y, v.z)

typedef struct CPos CPos;
struct CPos {
	int x;
	int y;
	int z;
};

inline BPos cp2bp(CPos pos) {
	return (BPos) {
		.x = pos.x * CHUNK_SIDELEN,
		.y = pos.y * CHUNK_SIDELEN,
		.z = pos.z * CHUNK_SIDELEN,
	};
}

inline CPos bp2cp(BPos pos) {
	return (CPos) {
		.x = pos.x / CHUNK_SIDELEN,
		.y = pos.y / CHUNK_SIDELEN,
		.z = pos.z / CHUNK_SIDELEN,
	};
}

// A collection of adjacent chunks excluding the center chunk.
typedef struct AdjacentChunks AdjacentChunks;
struct AdjacentChunks
{
	Chunk* items[dir_count];
};

void chunk_init(
	Chunk* chunk
);
void chunk_deinit(
	Chunk* chunk
);
void chunk_generate_blocks(
	Chunk *chunk, 
	const Perlin *perlin, 
	Fbm heightmap, 
	BPos world_min
);
void chunk_generate_mesh(
	Chunk *chunk, 
	AdjacentChunks adjacent_chunks
);
void chunk_unload(
	Chunk *chunk
);

// Specifies the thansformation between the world's chunk coordinates and 
// the internal coordinates used by `Chunks`.
typedef struct ChunkArea ChunkArea;
struct ChunkArea
{
	CPos min;
	CPos offset;
	size_t sidelen;
};

// A moveable area of chunks ment to be loaded and updated on the fly.
typedef struct Chunks Chunks;
struct Chunks 
{
	ChunkArea area;
	Chunk *items;
};

#define CHUNKS_CHUNK_IDX(x, y, z, sidelen) (z * sidelen * sidelen + y * sidelen + x)
#define CHUNKS_CHUNK_IDX_V(v, sidelen) CHUNKS_CHUNK_IDX(v.x, v.y, v.z, sidelen)

void chunks_init(Chunks* chunks, CPos min, size_t sidelen);
void chunks_deinit(Chunks *chunks);
void chunks_generate_blocks(
	Chunks* chunks,
	const Perlin* perlin,
	Fbm heightmap_fbm
);
void chunks_generate_mesh(Chunks* chunks);
void chunks_unload(Chunks* chunks);
void chunks_draw(Chunks* chunks, Camera cam, Perspective p);

// void chunks_move(Chunks* chunks, Vec3i delta);
