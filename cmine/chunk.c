#include "chunk.h"
#include "safety.h"
#include "vec_math.h"

#include <stdlib.h>


typedef uint_fast8_t BlockSurfaceCullingId;

// Surface never has a texure.
#define BLOCK_SURFACE_CULLING_ALWAYS ((BlockSurfaceCullingId)0)
// Surface has a texture only if the adjacent surface doesn't.
#define BLOCK_SURFACE_CULLING_WEAK ((BlockSurfaceCullingId)1)
// Surface always has a texture.
#define BLOCK_SURFACE_CULLING_STRONG ((BlockSurfaceCullingId)2)

#define BLOCK_SURFACE_CULLING_FIRST_ID BLOCK_SURFACE_CULLING_ALWAYS
#define BLOCK_SURFACE_CULLING_LAST_ID BLOCK_SURFACE_CULLING_STRONG

BlockSurfaceCullingId block_get_block_surface_culling_strategy(const BlockId self)
{
	switch (self) {
	case BLOCK_UNKNOWN:
		return BLOCK_SURFACE_CULLING_ALWAYS;
	case BLOCK_AIR:
		return BLOCK_SURFACE_CULLING_ALWAYS;
	case BLOCK_STONE:
		return BLOCK_SURFACE_CULLING_STRONG;
	}
	TRY(false);
}

bool block_surface_culling_should_cull(
	const BlockSurfaceCullingId from,
	const BlockSurfaceCullingId to)
{
	return from <= to;
}

static size_t to_block_index(const Vec3s local_pos) {
	return 
		local_pos.vs[0] * CHUNK_SIDELEN * CHUNK_SIDELEN +
		local_pos.vs[1] * CHUNK_SIDELEN +
		local_pos.vs[2];
}

BlockId* chunk_block_at(
	Chunk* const chunk, 
	const Vec3s local_pos)
{
	TRY(local_pos.vs[0] < CHUNK_SIDELEN);
	TRY(local_pos.vs[1] < CHUNK_SIDELEN);
	TRY(local_pos.vs[2] < CHUNK_SIDELEN);

	const size_t index = to_block_index(local_pos);
	return &chunk->blocks[index];
}

static size_t to_chunk_index(
	const Vec3s area_internal_pos,
	const size_t sidelen) 
{
	return 
		area_internal_pos.vs[0] * sidelen * sidelen +
		area_internal_pos.vs[1] * sidelen +
		area_internal_pos.vs[2];
}

Chunk* chunk_area_get_chunk_at(
	ChunkArea* const area,
	const Vec3i32 chunk_pos)
{
	TRY(vec3i32_exact_veclt(chunk_pos, area->anchor_pos));
	TRY(!vec3i32_exact_veclt(
		chunk_pos,
		vec3i32_add(area->anchor_pos, (int32_t)area->sidelen)));

	const size_t index = to_chunk_index(
		vec3s_rem(
			vec3s_from_vec3i32(vec3i32_vecsub(chunk_pos, area->anchor_pos)),
			area->sidelen),
		area->sidelen);

	return &area->chunks[index];
}

void chunk_generate_blocks(
	const WorldGenerationSettings* const settings,
	Chunk* const chunk,
	const Vec3i32 chunk_pos)
{
	for (Vec3s local_pos = vec3s_zero(); local_pos.vs[0] < CHUNK_SIDELEN; local_pos.vs[0]++) {
		for (local_pos.vs[2] = 0; local_pos.vs[2] < CHUNK_SIDELEN; local_pos.vs[2]++) {
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

typedef uint_fast8_t BlockSurfaceId;

#define BLOCK_SURFACE_RIGHT ((BlockSurfaceId)0)
#define BLOCK_SURFACE_LEFT ((BlockSurfaceId)1)
#define BLOCK_SURFACE_TOP ((BlockSurfaceId)2)
#define BLOCK_SURFACE_BOTTOM ((BlockSurfaceId)3)
#define BLOCK_SURFACE_FRONT ((BlockSurfaceId)4)
#define BLOCK_SURFACE_BACK ((BlockSurfaceId)5)

#define BLOCK_SURFACE_FIRST_ID BLOCK_SURFACE_RIGHT
#define BLOCK_SURFACE_LAST_ID BLOCK_SURFACE_BACK

Vec3i32 block_surface_get_normal(const BlockSurfaceId surface) {
	Vec3i32 ret = vec3i32_zero();
	switch (surface) {
	case BLOCK_SURFACE_RIGHT:
		*vec3i32_at(&ret, 0) = 1;
		return ret;
	case BLOCK_SURFACE_LEFT:
		*vec3i32_at(&ret, 0) = -1;
		return ret;
	case BLOCK_SURFACE_TOP:
		*vec3i32_at(&ret, 1) = 1;
		return ret;
	case BLOCK_SURFACE_BOTTOM:
		*vec3i32_at(&ret, 1) = -1;
		return ret;
	case BLOCK_SURFACE_FRONT:
		*vec3i32_at(&ret, 2) = 1;
		return ret;
	case BLOCK_SURFACE_BACK:
		*vec3i32_at(&ret, 2) = -1;
		return ret;
	}
	TRY(false);
}

Vec2s block_get_surface_texture_atlas_pos(
	const BlockId block, 
	const BlockSurfaceId surface) 
{
	// TODO: Assign the textures to blocks.
	return vec2s_zero();
}

typedef uint32_t BlockFaceVertex;

BlockFaceVertex create_block_face_vertex(
	const Vec3s local_pos,
	const BlockSurfaceId surface_id,
	const Vec2s atlas_pos)
{
	TRY(vec3s_exact_veclt(local_pos, vec3s_splat(CHUNK_SIDELEN)));
	TRY(surface_id >= BLOCK_SURFACE_FIRST_ID &&
		surface_id <= BLOCK_SURFACE_LAST_ID);
	TRY(atlas_pos.vs[0] < ATLAS_X_LEN);
	TRY(atlas_pos.vs[1] < ATLAS_Y_LEN);

	return
		((uint32_t)local_pos.vs[0]) |
		(((uint32_t)local_pos.vs[1]) << 3) |
		(((uint32_t)local_pos.vs[2]) << 6) |
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
		(((uint32_t)atlas_pos.vs[0]) << 13) |
		(((uint32_t)atlas_pos.vs[1]) << 17);
}

void chunk_area_generate_chunk_mesh(
	ChunkArea* const area,
	const Vec3i32 chunk_pos)
{
	Chunk* const chunk = chunk_area_get_chunk_at(area, chunk_pos);
	BlockFaceVertex* vertices = NULL;
	size_t vertices_count = 0;
	size_t vertices_capacity = 0;
	for (Vec3s local_pos = vec3s_zero(); local_pos.vs[0] < CHUNK_SIDELEN; local_pos.vs[0]++) {
		for (local_pos.vs[1] = 0; local_pos.vs[1] < CHUNK_SIDELEN; local_pos.vs[1]++) {
			for (local_pos.vs[2] = 0; local_pos.vs[2] < CHUNK_SIDELEN; local_pos.vs[2]++) {
				const BlockId block = *chunk_block_at(chunk, local_pos);
				const BlockSurfaceCullingId from = 
					block_get_block_surface_culling_strategy(block);
				for (BlockSurfaceId surface = BLOCK_SURFACE_FIRST_ID;
					surface < BLOCK_SURFACE_LAST_ID;
					surface++)
				{
					const Vec3s adjacent_pos = vec3s_vecadd(
						local_pos,
						vec3s_from_vec3i32(block_surface_get_normal(surface)));
					if (vec3s_exact_veclt(adjacent_pos, vec3s_splat(CHUNK_SIDELEN)))
					{
						const BlockId adjacent_block = *chunk_block_at(chunk, adjacent_pos);
						const BlockSurfaceCullingId to = 
							block_get_block_surface_culling_strategy(adjacent_block);
						if (block_surface_culling_should_cull(from, to)) continue;
					}
					if (!(vertices_count < vertices_capacity)) {
						vertices_capacity = vertices_capacity ? vertices_capacity * 2 : 1;
						vertices = srealloc(vertices, vertices_capacity * sizeof(BlockFaceVertex));
					}
					vertices[vertices_count] = create_block_face_vertex(
						local_pos, 
						surface, 
						block_get_surface_texture_atlas_pos(block, surface));
				}
			}
		}
	}
	// TODO: Load the vertices into VBO, update the chunk's VAO.
	free(vertices);
}
