#include "chunk.h"
#include "safety.h"
#include "vec.h"

struct Chunk {
	bool is_generated;
	// [x, y, z]
	block_id blocks[CHUNK_SIDELEN * CHUNK_SIDELEN * CHUNK_SIDELEN];
	gl_handle vao;
};

struct ChunkArea {
	vec3i pos;
	vec3size offset;
	size_t sidelen;

	// [x, y, z]
	Chunk* chunks;
};

typedef uint8_t block_surface_culling_id;

// Surface never has a texure.
#define BLOCK_SURFACE_CULLING_ALWAYS ((block_surface_culling_id)0)
// Surface has a texture only if the adjacent surface doesn't.
#define BLOCK_SURFACE_CULLING_WEAK ((block_surface_culling_id)1)
// Surface always has a texture.
#define BLOCK_SURFACE_CULLING_STRONG ((block_surface_culling_id)2)

#define BLOCK_SURFACE_CULLING_FIRST_ID BLOCK_SURFACE_CULLING_ALWAYS
#define BLOCK_SURFACE_CULLING_LAST_ID BLOCK_SURFACE_CULLING_STRONG

block_surface_culling_id block_get_block_surface_culling_strategy(block_id block)
{
	switch (block) {
	case BLOCK_UNKNOWN:
		return BLOCK_SURFACE_CULLING_ALWAYS;
	case BLOCK_AIR:
		return BLOCK_SURFACE_CULLING_ALWAYS;
	case BLOCK_STONE:
		return BLOCK_SURFACE_CULLING_STRONG;
	}
	try(false);
}

bool block_surface_culling_should_cull(
	block_surface_culling_id from,
	block_surface_culling_id to)
{
	return from <= to;
}

static size_t to_block_index(vec3size pos)
{
	try(pos.x < CHUNK_SIDELEN);
	try(pos.y < CHUNK_SIDELEN);
	try(pos.z < CHUNK_SIDELEN);

	return 
		pos.x * CHUNK_SIDELEN * CHUNK_SIDELEN +
		pos.y * CHUNK_SIDELEN +
		pos.z;
}

block_id* chunk_block_at(
	Chunk* const chunk, 
	vec3size pos)
{
	return &chunk->blocks[to_block_index(pos)];
}

static size_t to_chunk_index(
	vec3size area_pos,
	size_t sidelen) 
{
	return 
		area_pos.x * sidelen * sidelen +
		area_pos.y * sidelen +
		area_pos.z;
}

Chunk* chunk_area_get_chunk_at(
	ChunkArea* const area,
	vec3i chunk_pos)
{
	try(chunk_pos.x > area->pos.x && chunk_pos.x < area->pos.x + area->sidelen);
	try(chunk_pos.y > area->pos.y && chunk_pos.y < area->pos.y + area->sidelen);
	try(chunk_pos.z > area->pos.z && chunk_pos.z < area->pos.z + area->sidelen);

	vec3size pos = {
		((size_t)(chunk_pos.x - area->pos.x)) % area->sidelen,
		((size_t)(chunk_pos.y - area->pos.y)) % area->sidelen,
		((size_t)(chunk_pos.z - area->pos.z)) % area->sidelen,
	};
	return &area->chunks[to_chunk_index(pos, area->sidelen)];
}

void chunk_generate_blocks(
	const PerlinSeed* seed,
	const fbm_settings* heightmap_fmb,
	Chunk* chunk,
	vec3i chunk_pos)
{
	for (vec3size pos = { 0, 0, 0 }; pos.x < CHUNK_SIDELEN; pos.x++) {
		for (pos.z = 0; pos.z < CHUNK_SIDELEN; pos.z++) {
			int32_t height = (int32_t)fbm2(
				seed,
				heightmap_fmb,
				chunk_pos.x * CHUNK_SIDELEN + (int32_t)pos.x,
				chunk_pos.x * CHUNK_SIDELEN + (int32_t)pos.z
			);
			for (pos.y = 0; pos.y < CHUNK_SIDELEN; pos.y++) {
				int32_t world_y = chunk_pos.y * CHUNK_SIDELEN + (int32_t)pos.y;
				block_id* block = chunk_block_at(chunk, pos);
				*block = world_y > height ? BLOCK_AIR : BLOCK_STONE;
			}
	// Let this be a reminder.
	//for (Vec3s local_pos = vec3s_zero(); local_pos.vs[0] < CHUNK_SIDELEN; local_pos.vs[0]++) {
	//	for (local_pos.vs[2] = 0; local_pos.vs[2] < CHUNK_SIDELEN; local_pos.vs[2]++) {
			/*const int32_t height = (int32_t)fbm2(
				&settings->perlin_seed,
				&settings->heightmap_fbm,
				(float)local_pos.vs[0],
				(float)local_pos.vs[2]);
			for (local_pos.vs[1] = 0; local_pos.vs[1] < CHUNK_SIDELEN; local_pos.vs[1]++) {
				const int32_t y = chunk_pos.vs[1] * CHUNK_SIDELEN + (int32_t)local_pos.vs[1];
				BlockId* const block = chunk_block_at(chunk, local_pos);
				*block = y > height ? BLOCK_AIR : BLOCK_STONE;
			}*/
		}
	}
}

typedef uint8_t block_surface_id;

#define BLOCK_SURFACE_RIGHT ((block_surface_id)0)
#define BLOCK_SURFACE_LEFT ((block_surface_id)1)
#define BLOCK_SURFACE_TOP ((block_surface_id)2)
#define BLOCK_SURFACE_BOTTOM ((block_surface_id)3)
#define BLOCK_SURFACE_FRONT ((block_surface_id)4)
#define BLOCK_SURFACE_BACK ((block_surface_id)5)

#define BLOCK_SURFACE_FIRST_ID BLOCK_SURFACE_RIGHT
#define BLOCK_SURFACE_LAST_ID BLOCK_SURFACE_BACK

vec3i block_surface_get_normal(const block_surface_id surface) {
	vec3i ret = { 0, 0, 0 };
	switch (surface) {
	case BLOCK_SURFACE_RIGHT:
		ret.x = 1;
		return ret;
	case BLOCK_SURFACE_LEFT:
		ret.x = -1;
		return ret;
	case BLOCK_SURFACE_TOP:
		ret.y = 1;
		return ret;
	case BLOCK_SURFACE_BOTTOM:
		ret.y = -1;
		return ret;
	case BLOCK_SURFACE_FRONT:
		ret.z = 1;
		return ret;
	case BLOCK_SURFACE_BACK:
		ret.z = -1;
		return ret;
	}
	try(false);
}

vec2size block_get_surface_texture_atlas_pos(
	block_id block, 
	block_surface_id surface) 
{
	// TODO: Assign the textures to blocks.
	return (vec2size){ 0, 0 };
}

typedef uint32_t block_face_vertex;

block_face_vertex create_block_face_vertex(
	vec3size pos,
	block_surface_id surface_id,
	vec2size atlas_pos)
{
	try(pos.x < CHUNK_SIDELEN);
	try(pos.y < CHUNK_SIDELEN);
	try(pos.z < CHUNK_SIDELEN);
	try(surface_id >= BLOCK_SURFACE_FIRST_ID &&
		surface_id <= BLOCK_SURFACE_LAST_ID);
	try(atlas_pos.x < ATLAS_X_LEN);
	try(atlas_pos.y < ATLAS_Y_LEN);

	return
		((uint32_t)pos.x) |
		(((uint32_t)pos.y) << 3) |
		(((uint32_t)pos.z) << 6) |
		(((uint32_t)(
			(surface_id == BLOCK_SURFACE_RIGHT) |
			(surface_id == BLOCK_SURFACE_TOP) |
			(surface_id == BLOCK_SURFACE_FRONT))) << 9) |
		(((uint32_t)(
			(surface_id == BLOCK_SURFACE_RIGHT) |
			(surface_id == BLOCK_SURFACE_LEFT))) << 10) |
		(((uint32_t)(
			(surface_id == BLOCK_SURFACE_TOP) |
			(surface_id == BLOCK_SURFACE_BOTTOM))) << 11) |
		(((uint32_t)(
			(surface_id == BLOCK_SURFACE_FRONT) |
			(surface_id == BLOCK_SURFACE_BACK))) << 12) |
		(((uint32_t)atlas_pos.x) << 13) |
		(((uint32_t)atlas_pos.y) << 17);
}

void chunk_area_generate_chunk_mesh(
	ChunkArea* const area,
	vec3i chunk_pos)
{
	Chunk* chunk = chunk_area_get_chunk_at(area, chunk_pos);
	block_face_vertex* vertices = NULL;
	size_t vertices_count = 0;
	size_t vertices_capacity = 0;
	for (vec3size pos = { 0, 0, 0 }; pos.x < CHUNK_SIDELEN; pos.x++) {
		for (pos.y = 0; pos.y < CHUNK_SIDELEN; pos.y++) {
			for (pos.z = 0; pos.z < CHUNK_SIDELEN; pos.z++) {
				block_id block = *chunk_block_at(chunk, pos);
				block_surface_culling_id from = 
					block_get_block_surface_culling_strategy(block);
				for (block_surface_id surface = BLOCK_SURFACE_FIRST_ID;
					surface < BLOCK_SURFACE_LAST_ID;
					surface++)
				{
					vec3i normal = block_surface_get_normal(surface);
					vec3size adj = {
						(size_t)(pos.x + normal.x),
						(size_t)(pos.y + normal.y),
						(size_t)(pos.z + normal.z),
					};
					if (adj.x < CHUNK_SIDELEN &&
						adj.y < CHUNK_SIDELEN &&
						adj.z < CHUNK_SIDELEN)
					{
						block_id adjacent_block = *chunk_block_at(chunk, adj);
						block_surface_culling_id to =
							block_get_block_surface_culling_strategy(adjacent_block);
						if (block_surface_culling_should_cull(from, to)) continue;
					}
					if (!(vertices_count < vertices_capacity)) {
						vertices_capacity = vertices_capacity ? vertices_capacity * 2 : 1;
						vertices = srealloc(vertices, vertices_capacity * sizeof(block_face_vertex));
					}
					vec2size atlas_pos = block_get_surface_texture_atlas_pos(block, surface);
					vertices[vertices_count] = create_block_face_vertex(pos, surface, atlas_pos);
				}
			}
		}
	}
	// TODO: Load the vertices into VBO, update the chunk's VAO.
	sfree(vertices);
}
