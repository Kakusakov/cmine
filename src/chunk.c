#include "chunk.h"
#include "mathi.h"
#include "safety.h"
#include "glad.h"

block_surface_culling_id block_get_block_surface_culling_strategy(block_id block) {
	switch (block) {
	case block_unknown:
		return block_surface_culling_transparent;
	case block_air:
		return block_surface_culling_transparent;
	case block_stone:
		return block_surface_culling_solid;
	default:
		try(false);
	}
}

bool block_surface_culling_should_cull(
	block_surface_culling_id from,
	block_surface_culling_id to) 
{
	return from <= to;
}

Vec3i block_surface_normal(
	block_surface_id surface_id)
{
	Vec3i normal = { 0, 0, 0 };
	switch (surface_id) {
	case block_surface_right:
		normal.x += 1;
		break;
	case block_surface_left:
		normal.x -= 1;
		break;
	case block_surface_top:
		normal.y += 1;
		break;
	case block_surface_bottom:
		normal.y -= 1;
		break;
	case block_surface_front:
		normal.z += 1;
		break;
	case block_surface_back:
		normal.z -= 1;
		break;
	default:
		try(false);
	}
	return normal;
}

AtlasIndex block_get_surface_texture_atlas_index(
	block_id block,
	block_surface_id surface)
{
	switch (block) {
	case block_unknown:
		return (AtlasIndex){0, 0};
	case block_air:
		return (AtlasIndex){0, 0};
	case block_stone:
		return (AtlasIndex){0, 1};
	default:
		try(false);
	}
}

block_face_vertex block_face_vertex_init(
	size_t x,
	size_t y,
	size_t z,
	block_surface_id surface_id,
	AtlasIndex atlas_index)
{
	try(x < CHUNK_SIDELEN);
	try(y < CHUNK_SIDELEN);
	try(z < CHUNK_SIDELEN);
	try(surface_id <= block_surface_count);
	try(atlas_index.x < ATLAS_X_LEN);
	try(atlas_index.y < ATLAS_Y_LEN);

	return
		((uint32_t)x) |
		(((uint32_t)y) << 3) |
		(((uint32_t)z) << 6) |
		(((uint32_t)(
			(surface_id == block_surface_right) |
			(surface_id == block_surface_top) |
			(surface_id == block_surface_front))) << 9) |
		(((uint32_t)(
			(surface_id == block_surface_right) |
			(surface_id == block_surface_left))) << 10) |
		(((uint32_t)(
			(surface_id == block_surface_top) |
			(surface_id == block_surface_bottom))) << 11) |
		(((uint32_t)(
			(surface_id == block_surface_front) |
			(surface_id == block_surface_back))) << 12) |
		(((uint32_t)atlas_index.x) << 13) |
		(((uint32_t)atlas_index.y) << 17);
}

Chunk chunk_init(void) {
	Chunk chunk = {
		.generation_stage = 0,
		.vertex_count = 0,
	};
	glGenVertexArrays(1, &chunk.vao);
	return chunk;
}

void chunk_deinit(Chunk* chunk) {
	glDeleteVertexArrays(1, &chunk->vao);
	chunk->vao = 0;
}

size_t to_chunk_block_index(Vec3i chunk_local)
{
	try(chunk_local.x >= 0);
	try(chunk_local.y >= 0);
	try(chunk_local.z >= 0);
	try(chunk_local.x < CHUNK_SIDELEN);
	try(chunk_local.y < CHUNK_SIDELEN);
	try(chunk_local.z < CHUNK_SIDELEN);

	return (size_t)(
		chunk_local.x * CHUNK_SIDELEN * CHUNK_SIDELEN +
		chunk_local.y * CHUNK_SIDELEN +
		chunk_local.z);
}

block_id* chunk_block_at(Chunk* chunk_local, Vec3i pos) {
	return &chunk_local->blocks[to_chunk_block_index(pos)];
}

Chunks* chunks_init(Vec3i min, size_t sidelen) {
	try(sidelen != 0);
	try(SIZE_MAX / sidelen / sidelen / sidelen >= 1);
	size_t chunk_count = sidelen * sidelen * sidelen;
	Chunks* chunks = smalloc(sizeof(Chunks) + chunk_count * sizeof(Chunk));
	chunks->area = (ChunkArea){
		.min = min,
		.offset = { 0, 0, 0 },
	};
	for (size_t i = 0; i < chunk_count; i++) {
		chunks->values[i] = chunk_init();
	}
	return chunks;
}

void chunks_deinit(Chunks* chunks) {
	size_t sidelen = chunks->sidelen;
	size_t chunk_count = sidelen * sidelen * sidelen;
	for (size_t i = 0; i < chunk_count; i++) {
		chunk_deinit(&chunks->values[i]);
	}
	sfree(chunks);
}

void chunk_generate_blocks(Chunk* chunk, const Perlin* perlin, const FBM* heightmap_noise, Vec3i world_min) {
	if (chunk->generation_stage != chunk_generation_stage_awaits_blocks) return;
	for (int32_t x = 0; x < CHUNK_SIDELEN; x++) {
		for (int32_t z = 0; z < CHUNK_SIDELEN; z++) {
			int32_t height = (int32_t)fbm2(
				perlin,
				heightmap_noise,
				world_min.x + x,
				world_min.z + z
			);
			for (int32_t y = 0; y < CHUNK_SIDELEN; y++) {
				int32_t world_y = world_min.y + y;
				chunk->blocks[to_chunk_block_index((Vec3i) { x, y, z })] = world_y > height ? block_air : block_stone;
			}
		}
	}
	chunk->generation_stage++;
}

void chunk_generate_mesh(Chunk* chunk, AdjacentChunks adjacent_chunks) {
	if (chunk->generation_stage != chunk_generation_stage_awaits_mesh) return;

	block_face_vertex* vertices = NULL;
	size_t vertices_count = 0;
	size_t vertices_capacity = 0;

	for (size_t x = 0; x < CHUNK_SIDELEN; x++) {
		for (size_t y = 0; y < CHUNK_SIDELEN; y++) {
			for (size_t z = 0; z < CHUNK_SIDELEN; z++) {
				Vec3i pos = { x, y, z };
				block_id block = chunk->blocks[to_chunk_block_index(pos)];
				block_surface_culling_id from =
					block_get_block_surface_culling_strategy(block);
				for (block_surface_id surface_id = 0;
					surface_id < block_surface_count;
					surface_id++)
				{

					Vec3i adj = vec3i_add_v(pos, block_surface_normal(surface_id));
					if (adj.x < CHUNK_SIDELEN &&
						adj.y < CHUNK_SIDELEN &&
						adj.z < CHUNK_SIDELEN)
					{
						block_id adj_block = chunk->blocks[to_chunk_block_index(adj)];
						block_surface_culling_id to =
							block_get_block_surface_culling_strategy(adj_block);
						if (block_surface_culling_should_cull(from, to)) continue;
					}
					if (vertices_count == vertices_capacity) {
						vertices_capacity = vertices_capacity ? vertices_capacity * 2 : 1;
						vertices = srealloc(vertices, vertices_capacity * sizeof(block_face_vertex));
					}
					AtlasIndex atlas_index = block_get_surface_texture_atlas_index(block, surface_id);
					vertices[vertices_count] = block_face_vertex_init(x, y, z, surface_id, atlas_index);
				}
			}
		}
	}

	gl_handle vbo;
	glGenBuffers(1, &vbo);
	glBindVertexArray(chunk->vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices_count * sizeof(block_face_vertex), &vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 1, GL_UNSIGNED_INT, GL_FALSE, sizeof(block_face_vertex), (void*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);
	glDeleteBuffers(1, &vbo);
	chunk->vertex_count = vertices_count;

	sfree(vertices);
	chunk->generation_stage++;
}

void chunk_unload(Chunk* chunk) {
	switch (chunk->generation_stage) {
	case chunk_generation_stage_ready:
		glBindVertexArray(chunk->vao);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		chunk->vertex_count = 0;
	case chunk_generation_stage_awaits_mesh:
	case chunk_generation_stage_awaits_blocks:
		break;
	default:
		try(false);
	}
	chunk->generation_stage = 0;
}

Vec3i chunk_world_to_area_local(Vec3i chunk_world, size_t sidelen, ChunkArea area) {
	try(chunk_world.x >= area.min.x);
	try(chunk_world.y >= area.min.y);
	try(chunk_world.z >= area.min.z);
	try(chunk_world.x < area.min.x + sidelen);
	try(chunk_world.y < area.min.y + sidelen);
	try(chunk_world.z < area.min.z + sidelen);

	return vec3i_mod(vec3i_add_v(vec3i_sub_v(chunk_world, area.min), area.offset), sidelen);
}

Vec3i area_local_to_chunk_world(Vec3i area_local, size_t sidelen, ChunkArea area) {
	try(area_local.x >= 0);
	try(area_local.y >= 0);
	try(area_local.z >= 0);
	try(area_local.x < sidelen);
	try(area_local.y < sidelen);
	try(area_local.z < sidelen);

	return vec3i_add_v(area.min, vec3i_mod(vec3i_sub_v(area_local, area.offset), sidelen));
}

size_t area_local_to_chunks_index(Vec3i area_local, size_t sidelen) {
	try(area_local.x >= 0);
	try(area_local.y >= 0);
	try(area_local.z >= 0);
	try(area_local.x < sidelen);
	try(area_local.y < sidelen);
	try(area_local.z < sidelen);

	return
		area_local.x * sidelen * sidelen +
		area_local.y * sidelen +
		area_local.z;
}

Chunk* chunks_chunk_at_local(Chunks* chunks, Vec3i area_local) {
	return &chunks->values[area_local_to_chunks_index(area_local, chunks->sidelen)];
}

size_t chunk_world_to_chunks_index(Vec3i chunk_world, size_t sidelen, ChunkArea area) {
	return area_local_to_chunks_index(chunk_world_to_area_local(chunk_world, sidelen, area), sidelen);
}

Chunk* chunks_chunk_at_world(Chunks* chunks, Vec3i chunk_world) {
	return &chunks->values[chunk_world_to_chunks_index(chunk_world, chunks->sidelen, chunks->area)];
}

void chunks_generate_blocks(
	Chunks* chunks,
	const Perlin* perlin, 
	const FBM* heightmap_fbm) 
{
	size_t sidelen = chunks->sidelen;
	for (int32_t x = 0; x < sidelen; x++) {
		for (int32_t y = 0; y < sidelen; y++) {
			for (int32_t z = 0; z < sidelen; z++) {
				Vec3i area_local = { x, y, z };
				Chunk* chunk = chunks_chunk_at_local(chunks, area_local);
				Vec3i world_min = vec3i_mul(
					area_local_to_chunk_world(
						area_local, 
						chunks->sidelen, 
						chunks->area), 
					CHUNK_SIDELEN);
				chunk_generate_blocks(chunk, perlin, heightmap_fbm, world_min);
			}
		}
	}
}

void chunks_generate_mesh(Chunks* chunks) {
	size_t sidelen = chunks->sidelen;
	for (int32_t x = 0; x < sidelen; x++) {
		for (int32_t y = 0; y < sidelen; y++) {
			for (int32_t z = 0; z < sidelen; z++) {
				Vec3i area_local = { x, y, z };
				Chunk* chunk = chunks_chunk_at_local(chunks, area_local);
				chunk_generate_mesh(chunk, (AdjacentChunks){0});
			}
		}
	}
}

void chunks_unload(Chunks* chunks) {
	size_t sidelen = chunks->sidelen;
	for (int32_t x = 0; x < sidelen; x++) {
		for (int32_t y = 0; y < sidelen; y++) {
			for (int32_t z = 0; z < sidelen; z++) {
				Vec3i area_local = { x, y, z };
				Chunk* chunk = chunks_chunk_at_local(chunks, area_local);
				chunk_unload(chunk);
			}
		}
	}
}
