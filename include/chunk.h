#pragma once
#include "perlin.h"
#include "mathf.h"
#include "render.h"

typedef struct Position Position;
struct Position
{
	float x;
	float y;
	float z;
};

typedef struct BlockPosition BlockPosition;
struct BlockPosition
{
	int x;
	int y;
	int z;
};

typedef struct ChunkPosition ChunkPosition;
struct ChunkPosition
{
	int x;
	int y;
	int z;
};

typedef uint8_t Block;
enum 
{
	block_unknown,
	block_air,
	block_stone,
	block_count
};

typedef enum BlockFaceCulling BlockFaceCulling;
enum BlockFaceCulling 
{
	// Surface never has a texure.
	block_face_culling_transparent,
	// Surface has a texture only if the adjacent face is transparent.
	block_face_culling_semitransparent,
	// Surface has a texture only if the adjacent face is transparent or semitransparent.
	block_face_culling_solid,
	block_face_culling_count
};

BlockFaceCulling block_face_culling_get(Block block);
int should_cull_face(BlockFaceCulling from, BlockFaceCulling to);

typedef enum BlockFace BlockFace;
enum BlockFace 
{
	block_face_right,
	block_face_left,
	block_face_top,
	block_face_bottom,
	block_face_front,
	block_face_back,
	block_face_count
};

BlockPosition block_face_normal(BlockFace face);

#define ATLAS_SIDELEN 16

// Index inside of a texture atlas.
typedef struct AtlasIndex AtlasIndex;
struct AtlasIndex 
{
	size_t x;
	size_t y;
};

AtlasIndex atlas_index_get(Block block, BlockFace face);

#define CHUNK_SIDELEN 8

typedef enum ChunkGenerationStage ChunkGenerationStage;
enum ChunkGenerationStage 
{
	chunk_generation_stage_awaits_blocks,
	chunk_generation_stage_awaits_mesh,
	chunk_generation_stage_ready,
	chunk_generation_stage_count
};

// A cubic collection of blocks.
typedef struct Chunk Chunk;
struct Chunk 
{
	Block blocks[CHUNK_SIDELEN * CHUNK_SIDELEN * CHUNK_SIDELEN];
	ChunkGenerationStage generation_stage;
	GLsizei vertex_count;
	GLuint vao;
};

#define CHUNK_BLOCK_IDX(x, y, z) (x * CHUNK_SIDELEN * CHUNK_SIDELEN + y * CHUNK_SIDELEN + z)
#define CHUNK_BLOCK_IDX_V(v) CHUNK_BLOCK_IDX(v.x, v.y, v.z)

Chunk chunk_init(void);
void chunk_deinit(Chunk *chunk);

void chunk_generate_blocks(
	Chunk *chunk,
	const Perlin *perlin,
	Fbm heightmap_fbm,
	BlockPosition world_min);

// A collection of adjacent chunks excluding the center chunk.
typedef struct AdjacentChunks AdjacentChunks;
struct AdjacentChunks
{
	Chunk* items[block_face_count];
};

void chunk_generate_mesh(
	Chunk *chunk, 
	AdjacentChunks adjacent_chunks);
void chunk_unload(Chunk *chunk);

// Specifies the thansformation between the world's chunk coordinates and 
// the internal coordinates used by `Chunks`.
typedef struct ChunkArea ChunkArea;
struct ChunkArea
{
	ChunkPosition min;
	ChunkPosition offset;
	size_t sidelen;
};

// A moveable area of chunks ment to be loaded and updated on the fly.
typedef struct Chunks Chunks;
struct Chunks 
{
	ChunkArea area;
	Chunk *items;
};

#define CHUNKS_CHUNK_IDX(x, y, z, sidelen) (x * sidelen * sidelen + y * sidelen + z)
#define CHUNKS_CHUNK_IDX_V(v, sidelen) CHUNKS_CHUNK_IDX(v.x, v.y, v.z, sidelen)

int chunks_init(Chunks* chunks, ChunkPosition min, size_t sidelen);
void chunks_deinit(Chunks *chunks);

void chunks_generate_blocks(
	Chunks *chunks,
	const Perlin *perlin, 
	Fbm heightmap_fbm);
void chunks_generate_mesh(Chunks *chunks);
void chunks_unload(Chunks *chunks);

// void chunks_move(Chunks* chunks, Vec3i delta);
