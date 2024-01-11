#pragma once
#include "perlin.h"
#include "mathi.h"

// I would love to have C23 constexpr for 
// better enums, but C23 doesn't have 
// good support yet.

typedef uint8_t block_id;

#define BLOCK_UNKNOWN ((block_id)0)
#define BLOCK_AIR ((block_id)1)
#define BLOCK_STONE ((block_id)2)

#define BLOCK_FIRST_ID BLOCK_UNKNOWN
#define BLOCK_LAST_ID BLOCK_STONE

typedef uint8_t block_surface_culling_id;

// Surface never has a texure.
#define BLOCK_SURFACE_CULLING_TRANSPARENT ((block_surface_culling_id)0)
// Surface has a texture only if the adjacent surface is transparent.
#define BLOCK_SURFACE_CULLING_SEMITRANSPARENT ((block_surface_culling_id)1)
// Surface has a texture only if the adjacent surface is transparent or semitransparent.
#define BLOCK_SURFACE_CULLING_SOLID ((block_surface_culling_id)2)

#define BLOCK_SURFACE_CULLING_FIRST_ID BLOCK_SURFACE_CULLING_TRANSPARENT
#define BLOCK_SURFACE_CULLING_LAST_ID BLOCK_SURFACE_CULLING_SOLID

block_surface_culling_id block_get_block_surface_culling_strategy(block_id block);
bool block_surface_culling_should_cull(
	block_surface_culling_id from,
	block_surface_culling_id to);

typedef uint8_t block_surface_id;

#define BLOCK_SURFACE_RIGHT ((block_surface_id)0)
#define BLOCK_SURFACE_LEFT ((block_surface_id)1)
#define BLOCK_SURFACE_TOP ((block_surface_id)2)
#define BLOCK_SURFACE_BOTTOM ((block_surface_id)3)
#define BLOCK_SURFACE_FRONT ((block_surface_id)4)
#define BLOCK_SURFACE_BACK ((block_surface_id)5)

#define BLOCK_SURFACE_FIRST_ID BLOCK_SURFACE_RIGHT
#define BLOCK_SURFACE_LAST_ID BLOCK_SURFACE_BACK

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

#define CHUNK_GENERATION_STAGE_AWAITS_BLOCKS ((chunk_generateion_stage_id)0)
#define CHUNK_GENERATION_STAGE_AWAITS_MESH ((chunk_generateion_stage_id)1)
#define CHUNK_GENERATION_STAGE_READY ((chunk_generateion_stage_id)2)

#define CHUNK_GENERATION_STAGE_FIRST_ID CHUNK_GENERATION_STAGE_AWAITS_BLOCKS;
#define CHUNK_GENERATION_STAGE_LAST_ID CHUNK_GENERATION_STAGE_READY;

// A cubic collection of blocks.
typedef struct Chunk Chunk;
struct Chunk {
	chunk_generateion_stage_id generation_stage;
	block_id blocks[CHUNK_SIDELEN * CHUNK_SIDELEN * CHUNK_SIDELEN];
	gl_handle vao;
};

Chunk chunk_init(void); 
void chunk_deinit(Chunk* chunk);

size_t to_chunk_block_index(Vec3i chunk_local);
block_id* chunk_block_at(Chunk* chunk, Vec3i chunk_local);

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

Chunks* chunks_init(size_t sidelen);
void chunks_deinit(Chunks* chunks);

Vec3i chunk_world_to_area_local(Vec3i chunk_world, size_t sidelen, ChunkArea area);
Vec3i area_local_to_chunk_world(Vec3i area_local, size_t sidelen, ChunkArea area);

size_t area_local_to_chunks_index(Vec3i area_local, size_t sidelen);
Chunk* chunks_chunk_at_local(Chunks* chunks, Vec3i area_local);
size_t chunk_world_to_chunks_index(Vec3i chunk_world, size_t sidelen, ChunkArea area);
Chunk* chunks_chunk_at_world(Chunks* chunks, Vec3i chunk_world);

bool chunks_generate_blocks_for_chunk(
	Chunks* chunks,
	const Perlin* perlin,
	const FBM* heightmap_fmb,
	Vec3i area_local
);
bool chunks_generate_mesh_for_chunk(
	Chunks* chunks,
	Vec3i area_local);

void chunks_generate_blocks(
	Chunks* chunks,
	const Perlin* perlin,
	const FBM* heightmap_fbm);
void chunks_generate_mesh(Chunks* chunks);
