#include "chunk.h"
#include "render.h"
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#define ASSERT(x) assert(x)

BlockFaceCulling block_face_culling_get(Block block) 
{
	BlockFaceCulling face_culling;
	switch (block) {
	case block_unknown: face_culling = block_face_culling_transparent; break;
	case block_air:     face_culling = block_face_culling_transparent; break;
	case block_stone:   face_culling = block_face_culling_solid;       break;
	default: ASSERT(0); break;
	}
	return face_culling;
}

int should_cull_face(BlockFaceCulling from, BlockFaceCulling to) 
{
	return from <= to;
}

BlockPosition block_face_normal(BlockFace face)
{
	BlockPosition normal = {0};
	switch (face) {
	case block_face_front:  normal.x += 1; break;
	case block_face_back:   normal.x -= 1; break;
	case block_face_right:  normal.y += 1; break;
	case block_face_left:   normal.y -= 1; break;
	case block_face_top:    normal.z += 1; break;
	case block_face_bottom: normal.z -= 1; break;
	default: ASSERT(0); break;
	}
	return normal;
}

AtlasIndex atlas_index_get(Block block, BlockFace face)
{
	AtlasIndex index;
	switch (block) {
	case block_unknown: index.x = 0; index.y = 0; break;
	case block_air:     index.x = 0; index.y = 0; break;
	case block_stone:   index.x = 1; index.y = 0; break;
	default: ASSERT(0); break;
	}
	return index;
}

Chunk chunk_init(void)
{
	GLuint vao;
	glGenVertexArrays(1, &vao);
	return (Chunk) {
		.blocks = {0},
		.generation_stage = 0,
		.vertex_count = 0,
		.vao = vao,
	};
}

void chunk_deinit(Chunk *chunk)
{
	chunk_unload(chunk);  // Should I do this?
	glDeleteVertexArrays(1, &chunk->vao);
	chunk->vao = 0;
}

void chunk_generate_blocks(
	Chunk *chunk,
	const Perlin *perlin,
	Fbm heightmap_fbm,
	BlockPosition world_min)
{
	ASSERT(chunk->generation_stage == chunk_generation_stage_awaits_blocks);
	for (int x = 0; x < CHUNK_SIDELEN; x++)
	{
		for (int y = 0; y < CHUNK_SIDELEN; y++)
		{
			float noise = fbm2(
				perlin,
				heightmap_fbm,
				(float)(world_min.x + x + 0.31415f),
				(float)(world_min.y + y + 0.31415f));
			ASSERT(noise > INT_MIN && noise < INT_MAX);  // Check for world boundaries.
			int height = (int)noise;
			for (int z = 0; z < CHUNK_SIDELEN; z++)
			{
				int world_z = world_min.z + z;
				chunk->blocks[CHUNK_BLOCK_IDX(x, y, z)] = (world_z > height) ? block_air : block_stone;
			}
		}
	}
	chunk->generation_stage++;
}

typedef struct ChunkVertex ChunkVertex;
PACK(struct ChunkVertex
{
	Vec3f pos;
	Vec2f uv;
});

typedef struct ChunkVertices ChunkVertices;
struct ChunkVertices
{
	GLsizei count;
	GLsizei capacity;
	ChunkVertex* items;
};

static int chunk_vertices_add(
	ChunkVertices *vertices,
	Vec3f pos,
	Block block,
	BlockFace face)
{
	ASSERT(pos.x >= 0);
	ASSERT(pos.y >= 0);
	ASSERT(pos.z >= 0);
	if (vertices->count + 6 > vertices->capacity)
	{
		vertices->capacity = (vertices->capacity == 0) ? 6 : vertices->capacity * 2;
		void* tmp = realloc(vertices->items, sizeof(ChunkVertex) * vertices->capacity);
		if (!tmp) return 0;
		vertices->items = tmp;
	}

	pos = vec3f_opengl_style(pos);
	Vec3f v1;
	Vec3f v2;
	Vec3f v3;
	Vec3f v4;

	switch (face) {
	case block_face_back:
		v1 = (Vec3f){0, 1, 1};
		v2 = (Vec3f){0, 0, 1};
		v3 = (Vec3f){1, 0, 1};
		v4 = (Vec3f){1, 1, 1};
		break;
	case block_face_front:
		v1 = (Vec3f){1, 1, 0};
		v2 = (Vec3f){1, 0, 0};
		v3 = (Vec3f){0, 0, 0};
		v4 = (Vec3f){0, 1, 0};
		break;
	case block_face_right:
		v1 = (Vec3f){1, 1, 1};
		v2 = (Vec3f){1, 0, 1};
		v3 = (Vec3f){1, 0, 0};
		v4 = (Vec3f){1, 1, 0};
		break;
	case block_face_left:
		v1 = (Vec3f){0, 1, 0};
		v2 = (Vec3f){0, 0, 0};
		v3 = (Vec3f){0, 0, 1};
		v4 = (Vec3f){0, 1, 1};
		break;
	case block_face_top:
		v1 = (Vec3f){0, 1, 0};
		v2 = (Vec3f){0, 1, 1};
		v3 = (Vec3f){1, 1, 1};
		v4 = (Vec3f){1, 1, 0};
		break;
	case block_face_bottom:
		v1 = (Vec3f){0, 0, 1};
		v2 = (Vec3f){0, 0, 0};
		v3 = (Vec3f){1, 0, 0};
		v4 = (Vec3f){1, 0, 1};
		break;
	default:
		ASSERT(0); 
		break;
	}

	v1.x += pos.x; v1.y += pos.y; v1.z += pos.z;
	v2.x += pos.x; v2.y += pos.y; v2.z += pos.z;
	v3.x += pos.x; v3.y += pos.y; v3.z += pos.z;
	v4.x += pos.x; v4.y += pos.y; v4.z += pos.z;

	AtlasIndex atlas_index = atlas_index_get(block, face);
	Vec2f uv_base = {
		.x = 0, //((float)atlas_index.x) / ATLAS_SIDELEN,
		.y = 0, //((float)atlas_index.y) / ATLAS_SIDELEN,
	};
	float uv_step_x = 1; //1.0f / ATLAS_SIDELEN;
	float uv_step_y = 1; //1.0f / ATLAS_SIDELEN;

	Vec2f uv1 = {uv_step_x, uv_step_y};
	Vec2f uv2 = {uv_step_x, 0};
	Vec2f uv3 = {0, 0};
	Vec2f uv4 = {0, uv_step_y};

	uv1.x += uv_base.x; uv1.y += uv_base.y;
	uv2.x += uv_base.x; uv2.y += uv_base.y;
	uv3.x += uv_base.x; uv3.y += uv_base.y;
	uv4.x += uv_base.x; uv4.y += uv_base.y;

	vertices->items[vertices->count++] = (ChunkVertex){v1, uv1};
	vertices->items[vertices->count++] = (ChunkVertex){v2, uv2};
	vertices->items[vertices->count++] = (ChunkVertex){v4, uv4};
	vertices->items[vertices->count++] = (ChunkVertex){v2, uv2};
	vertices->items[vertices->count++] = (ChunkVertex){v3, uv3};
	vertices->items[vertices->count++] = (ChunkVertex){v4, uv4};
	return 1;
}

void chunk_generate_mesh(Chunk *chunk, AdjacentChunks adjacent_chunks)
{
	ASSERT(chunk->generation_stage == chunk_generation_stage_awaits_mesh);
	ChunkVertices vertices = {0};

	for (int x = 0; x < CHUNK_SIDELEN; x++)
	{
		for (int y = 0; y < CHUNK_SIDELEN; y++)
		{
			for (int z = 0; z < CHUNK_SIDELEN; z++)
			{
				Block block = chunk->blocks[CHUNK_BLOCK_IDX(x, y, z)];
				BlockFaceCulling from = block_face_culling_get(block);
				for (BlockFace face = 0; face < block_face_count; face++)
				{
					BlockPosition norm = block_face_normal(face);
					BlockPosition adj = {
						x + norm.x,
						y + norm.y,
						z + norm.z,
					};
					if (adj.x >= 0 && adj.x < CHUNK_SIDELEN &&
						adj.y >= 0 && adj.y < CHUNK_SIDELEN &&
						adj.z >= 0 && adj.z < CHUNK_SIDELEN)
					{
						Block adj_block = chunk->blocks[CHUNK_BLOCK_IDX_V(adj)];
						BlockFaceCulling to = block_face_culling_get(adj_block);
						if (should_cull_face(from, to)) continue;
					}
					else if (from == block_face_culling_transparent) continue;
					Vec3f pos = {x, y, z};
					if (!chunk_vertices_add(&vertices, pos, block, face))
					{
						// TODO: Handle error...
						ASSERT(0);
					}
				}
			}
		}
	}

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindVertexArray(chunk->vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.count * sizeof(ChunkVertex), vertices.items, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ChunkVertex), (void*)0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(ChunkVertex), (void*)sizeof(Vec3f));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);
	glDeleteBuffers(1, &vbo);
	chunk->vertex_count = vertices.count;

	free(vertices.items);
	chunk->generation_stage++;
}

void chunk_unload(Chunk *chunk)
{
	switch (chunk->generation_stage)
	{
	case chunk_generation_stage_ready:
		glBindVertexArray(chunk->vao);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		chunk->vertex_count = 0;
	case chunk_generation_stage_awaits_mesh:
	case chunk_generation_stage_awaits_blocks:
		break;
	default:
		ASSERT(0);
		break;
	}
	chunk->generation_stage = 0;
}

void draw_chunk(const Chunk* chunk, Mat transform, GLuint texture)
{
	GLuint prog = render_chunk_shader_program();
	glUseProgram(prog);

	const char* transform_name = "transform";
	GLuint location = glGetUniformLocation(prog, transform_name);
	if (location == -1)
	{
		fprintf(
			stderr,
			"\nCaught runtime error:\n"
			"\nRenderer is unbale to find unform `%s` in chunk shader program",
			transform_name);
		return;
	}
	glUniformMatrix4fv(location, 1, GL_FALSE, transform.v);

	glBindTexture(GL_TEXTURE_2D, texture);
	glBindVertexArray(chunk->vao);
	glDrawArrays(GL_TRIANGLES, 0, chunk->vertex_count);
	glBindVertexArray(0);
}

/*int chunks_init(Chunks* chunks, ChunkPosition min, size_t sidelen)
{
	ASSERT(sidelen != 0);
	ASSERT(SIZE_MAX / sidelen / sidelen / sidelen >= 1);
	size_t chunk_count = sidelen * sidelen * sidelen;
	chunks->area = (ChunkArea){
		.min = min,
		.offset = {0},
		.sidelen = sidelen,
	};
	chunks->items = malloc(chunk_count * sizeof(Chunk));
	if (chunks->items == NULL) return 0;

	for (size_t i = 0; i < chunk_count; i++)
	{
		chunks->items[i] = chunk_init();
	}
	return 1;
}

void chunks_deinit(Chunks* chunks)
{
	size_t sidelen = chunks->area.sidelen;
	ASSERT(SIZE_MAX / sidelen / sidelen / sidelen >= 1);
	size_t chunk_count = sidelen * sidelen * sidelen;
	for (size_t i = 0; i < chunk_count; i++) {
		chunk_deinit(&chunks->items[i]);
	}
	free(chunks->items);
	chunks->items = NULL;
	chunks->area.sidelen = 0;
}

static int is_world_within_area(ChunkPosition world, ChunkArea area)
{
	return
		world.x >= area.min.x &&
		world.y >= area.min.y &&
		world.z >= area.min.z &&
		world.x < area.min.x + area.sidelen &&
		world.y < area.min.y + area.sidelen &&
		world.z < area.min.z + area.sidelen;
}

static ChunkPosition world_to_area_local(ChunkPosition world, ChunkArea area)
{
	ASSERT(is_world_within_area(world, area));
	return (ChunkPosition) {
		mod(world.x - area.min.x + area.offset.x, (int)area.sidelen),
		mod(world.y - area.min.y + area.offset.y, (int)area.sidelen),
		mod(world.z - area.min.z + area.offset.z, (int)area.sidelen),
	};
}

static ChunkPosition area_local_to_world(ChunkPosition area_local, ChunkArea area)
{
	ASSERT(area_local.x >= 0);
	ASSERT(area_local.y >= 0);
	ASSERT(area_local.z >= 0);
	ASSERT(area_local.x < area.sidelen);
	ASSERT(area_local.y < area.sidelen);
	ASSERT(area_local.z < area.sidelen);

	return (ChunkPosition) {
		area.min.x + mod(area_local.x - area.offset.x, (int)area.sidelen),
		area.min.y + mod(area_local.y - area.offset.y, (int)area.sidelen),
		area.min.z + mod(area_local.z - area.offset.z, (int)area.sidelen),
	};
}

void chunks_generate_blocks(
	Chunks *chunks,
	const Perlin *perlin, 
	Fbm heightmap_fbm)
{
	size_t sidelen = chunks->area.sidelen;
	for (size_t x = 0; x < sidelen; x++)
	{
		for (size_t y = 0; y < sidelen; y++)
		{
			for (size_t z = 0; z < sidelen; z++)
			{
				Chunk *chunk = &chunks->items[CHUNKS_CHUNK_IDX(x, y, z, sidelen)];
				ChunkPosition area_local = {x, y, z};
				ChunkPosition chunk_world = area_local_to_world(area_local, chunks->area);
				BlockPosition world_min = {
					chunk_world.x * CHUNK_SIDELEN,
					chunk_world.y * CHUNK_SIDELEN,
					chunk_world.z * CHUNK_SIDELEN,
				};
				chunk_generate_blocks(chunk, perlin, heightmap_fbm, world_min);
			}
		}
	}
}

void chunks_generate_mesh(Chunks *chunks)
{
	size_t sidelen = chunks->area.sidelen;
	for (size_t x = 0; x < sidelen; x++)
	{
		for (size_t y = 0; y < sidelen; y++)
		{
			for (size_t z = 0; z < sidelen; z++)
			{
				Chunk *chunk = &chunks->items[CHUNKS_CHUNK_IDX(x, y, z, sidelen)];
				chunk_generate_mesh(chunk, (AdjacentChunks){0});
			}
		}
	}
}

void chunks_unload(Chunks *chunks)
{
	size_t sidelen = chunks->area.sidelen;
	for (size_t x = 0; x < sidelen; x++)
	{
		for (size_t y = 0; y < sidelen; y++)
		{
			for (size_t z = 0; z < sidelen; z++)
			{
				Chunk *chunk = &chunks->items[CHUNKS_CHUNK_IDX(x, y, z, sidelen)];
				chunk_unload(chunk);
			}
		}
	}
}*/
