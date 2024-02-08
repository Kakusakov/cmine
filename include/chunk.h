#pragma once
#include "perlin.h"
#include "mathi.h"

// I would love to have C23 constexpr for 
// better enums, but C23 doesn't have 
// good support yet.

typedef uint8_t block_id;
enum {
	block_unknown,
	block_air,
	block_stone,
	block_count
};

typedef uint8_t block_surface_culling_id;
enum {
	// Surface never has a texure.
	block_surface_culling_transparent,
	// Surface has a texture only if the adjacent surface is transparent.
	block_surface_culling_semitransparent,
	// Surface has a texture only if the adjacent surface is transparent or semitransparent.
	block_surface_culling_solid,
	block_surface_culling_count
};

block_surface_culling_id block_get_block_surface_culling_strategy(block_id block);
bool block_surface_culling_should_cull(
	block_surface_culling_id from,
	block_surface_culling_id to);

typedef uint8_t block_surface_id;
enum {
	block_surface_right,
	block_surface_left,
	block_surface_top,
	block_surface_bottom,
	block_surface_front,
	block_surface_back,
	block_surface_count
};

Vec3i block_surface_normal(block_surface_id surface_id);

#define ATLAS_X_LEN 16
#define ATLAS_Y_LEN 16

// Index inside of a texture atlas.
typedef struct AtlasIndex AtlasIndex;
struct AtlasIndex {
	size_t x;
	size_t y;
};

AtlasIndex block_get_surface_texture_atlas_index(
	block_id block,
	block_surface_id surface);

typedef uint32_t block_face_vertex;

block_face_vertex block_face_vertex_init(
	size_t x,
	size_t y,
	size_t z,
	block_surface_id surface_id,
	AtlasIndex atlas_index);


#define CHUNK_SIDELEN 8

typedef uint8_t chunk_generateion_stage_id;
enum {
	chunk_generation_stage_awaits_blocks,
	chunk_generation_stage_awaits_mesh,
	chunk_generation_stage_ready,
	chunk_generation_stage_count,
};

// A cubic collection of blocks.
typedef struct Chunk Chunk;
struct Chunk {
	chunk_generateion_stage_id generation_stage;
	block_id blocks[CHUNK_SIDELEN * CHUNK_SIDELEN * CHUNK_SIDELEN];
	size_t vertex_count;
	gl_handle vao;
};

// A collection of adjacent chunks excluding the center chunk.
typedef struct AdjacentChunks AdjacentChunks;
struct AdjacentChunks {
	Chunk items[block_surface_count];
};

Chunk chunk_init(void); 
void chunk_deinit(Chunk* chunk);

size_t to_chunk_block_index(Vec3i chunk_local);
block_id* chunk_block_at(Chunk* chunk, Vec3i chunk_local);

void chunk_generate_blocks(Chunk* chunk, const Perlin* perlin, const FBM* heightmap_noise, Vec3i world_min);
void chunk_generate_mesh(Chunk* chunk, AdjacentChunks adjacent_chunks);
void chunk_unload(Chunk* chunk);

// Specifies the thansformation between the world's chunk coordinates and 
// the internal coordinates used by `Chunks`.
typedef struct ChunkArea ChunkArea;
struct ChunkArea {
	Vec3i min;
	Vec3i offset;
};

// A moveable area of chunks ment to be loaded and updated on the fly.
typedef struct Chunks Chunks;
struct Chunks {
	ChunkArea area;
	size_t sidelen;
	Chunk values[];
};

Chunks* chunks_init(Vec3i min, size_t sidelen);
void chunks_deinit(Chunks* chunks);

Vec3i chunk_world_to_area_local(Vec3i chunk_world, size_t sidelen, ChunkArea area);
Vec3i area_local_to_chunk_world(Vec3i area_local, size_t sidelen, ChunkArea area);

size_t area_local_to_chunks_index(Vec3i area_local, size_t sidelen);
Chunk* chunks_chunk_at_local(Chunks* chunks, Vec3i area_local);
size_t chunk_world_to_chunks_index(Vec3i chunk_world, size_t sidelen, ChunkArea area);
Chunk* chunks_chunk_at_world(Chunks* chunks, Vec3i chunk_world);

void chunks_generate_blocks(
	Chunks* chunks,
	const Perlin* perlin,
	const FBM* heightmap_fbm);
void chunks_generate_mesh(Chunks* chunks);
void chunks_generate_mesh(Chunks* chunks);

// void chunks_move(Chunks* chunks, Vec3i delta);
