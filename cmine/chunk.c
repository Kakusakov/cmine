#include "chunk.h"
#include "safety.h"

#include <stdlib.h>

bool block_should_cull_adjacent(const BlockId block) {
	switch (block) {
	case BLOCK_UNKNOWN:
		return false;
	case BLOCK_AIR:
		return false;
	case BLOCK_STONE:
		return true;
	}
	TRY(false);
}

#define ATLAS_X_LEN 16
#define ATLAS_Y_LEN 16

static size_t to_block_index(
	const size_t x,
	const size_t y,
	const size_t z)
{
	return x * CHUNK_SIDELEN * CHUNK_SIDELEN + y * CHUNK_SIDELEN + z;
}

BlockId* chunk_get_block_at(
	Chunk* const chunk,
	const size_t dx,
	const size_t dy,
	const size_t dz) 
{
	TRY(dx < CHUNK_SIDELEN);
	TRY(dy < CHUNK_SIDELEN);
	TRY(dz < CHUNK_SIDELEN);

	const size_t index = to_block_index(dx, dy, dz);

	return &chunk->blocks[index];
}

static size_t to_chunk_index(
	const size_t dx, 
	const size_t dy, 
	const size_t dz, 
	const size_t sidelen) 
{
	return dx * sidelen * sidelen + dy * sidelen + dz;
}

Chunk* chunk_area_get_chunk_at(
	ChunkArea* const area,
	const int32_t chunk_x,
	const int32_t chunk_y,
	const int32_t chunk_z) 
{
	TRY(chunk_x < area->x_anchor);
	TRY(chunk_y < area->y_anchor);
	TRY(chunk_z < area->z_anchor);
	TRY(chunk_x >= area->x_anchor + area->sidelen);
	TRY(chunk_y >= area->y_anchor + area->sidelen);
	TRY(chunk_z >= area->z_anchor + area->sidelen);

	const size_t index = to_chunk_index(
		((size_t)(chunk_x - area->x_anchor) + area->x_offset) % area->sidelen,
		((size_t)(chunk_x - area->x_anchor) + area->x_offset) % area->sidelen,
		((size_t)(chunk_x - area->x_anchor) + area->x_offset) % area->sidelen,
		area->sidelen);

	return &area->chunks[index];
}

void chunk_generate_blocks(
	const WorldGenerationSettings* const settings,
	Chunk* const chunk,
	const int32_t chunk_x,
	const int32_t chunk_y,
	const int32_t chunk_z)
{
	for (int32_t dx = 0; dx < CHUNK_SIDELEN; dx++) {
		const int32_t x = chunk_x + dx;
		for (int32_t dz = 0; dz < CHUNK_SIDELEN; dz++) {
			const int32_t z = chunk_z + dz;
			const int32_t height = (int32_t)fbm2(
				&settings->perlin_seed,
				&settings->heightmap_fbm,
				(float)x,
				(float)z);
			for (int32_t dy = 0; dy < CHUNK_SIDELEN; dy++) {
				const int32_t y = chunk_y + dy;
				BlockId* const block = chunk_get_block_at(chunk, dx, dy, dz);
				*block = y > height ? BLOCK_AIR : BLOCK_STONE;
			}
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

#define BLOCK_SURFACE_FIRST_ID ((BlockSurfaceId)0)
#define BLOCK_SURFACE_LAST_ID ((BlockSurfaceId)5)

typedef uint32_t BlockFaceVertex;

BlockFaceVertex create_block_face_vertex(
	const size_t dx,
	const size_t dy,
	const size_t dz,
	const BlockSurfaceId surface_id,
	const size_t uv_x,
	const size_t uv_y)
{
	TRY(dx < CHUNK_SIDELEN);
	TRY(dy < CHUNK_SIDELEN);
	TRY(dz < CHUNK_SIDELEN);
	TRY(surface_id >= BLOCK_SURFACE_FIRST_ID &&
		surface_id <= BLOCK_SURFACE_LAST_ID);
	TRY(uv_x < ATLAS_X_LEN);
	TRY(uv_y < ATLAS_Y_LEN);

	return
		((uint32_t)dx) |
		(((uint32_t)dy) << 3) |
		(((uint32_t)dz) << 6) |
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
		(((uint32_t)uv_x) << 13) |
		(((uint32_t)uv_y) << 17);
}

void chunk_area_generate_chunk_mesh(
	ChunkArea* const area,
	const int32_t chunk_x,
	const int32_t chunk_y,
	const int32_t chunk_z)
{
	Chunk* const chunk = chunk_area_get_chunk_at(area, chunk_x, chunk_y, chunk_z);
	BlockFaceVertex* vertices = NULL;
	for (size_t dx = 0; dx < CHUNK_SIDELEN; dx++) {
		for (size_t dz = 0; dz < CHUNK_SIDELEN; dz++) {
			for (size_t dy = 0; dy < CHUNK_SIDELEN; dy++) {
				const BlockId block = *chunk_get_block_at(chunk, dx, dy, dz);

			}
		}
	}
}
