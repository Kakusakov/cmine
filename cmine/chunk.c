#include "chunk.h"
#include "safety.h"
#include "mathf.h"
#include "glad.h"

typedef uint8_t block_surface_culling_id;

// Surface never has a texure.
#define BLOCK_SURFACE_CULLING_TRANSPARENT ((block_surface_culling_id)0)
// Surface has a texture only if the adjacent surface is transparent.
#define BLOCK_SURFACE_CULLING_SEMITRANSPARENT ((block_surface_culling_id)1)
// Surface has a texture only if the adjacent surface is transparent or semitransparent.
#define BLOCK_SURFACE_CULLING_SOLID ((block_surface_culling_id)2)

#define BLOCK_SURFACE_CULLING_FIRST_ID BLOCK_SURFACE_CULLING_TRANSPARENT
#define BLOCK_SURFACE_CULLING_LAST_ID BLOCK_SURFACE_CULLING_SOLID

static block_surface_culling_id block_get_block_surface_culling_strategy(block_id block) {
	switch (block) {
	case BLOCK_UNKNOWN:
		return BLOCK_SURFACE_CULLING_TRANSPARENT;
	case BLOCK_AIR:
		return BLOCK_SURFACE_CULLING_TRANSPARENT;
	case BLOCK_STONE:
		return BLOCK_SURFACE_CULLING_SOLID;
	}
	try(false);
}

static bool block_surface_culling_should_cull(
	block_surface_culling_id from,
	block_surface_culling_id to)
{
	return from <= to;
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

static void block_surface_step_towards(
	block_surface_id surface_id,
	int32_t* x,
	int32_t* y,
	int32_t* z)
{
	switch (surface_id) {
	case BLOCK_SURFACE_RIGHT:
		*x += 1;
		break;
	case BLOCK_SURFACE_LEFT:
		*x -= 1;
		break;
	case BLOCK_SURFACE_TOP:
		*y += 1;
		break;
	case BLOCK_SURFACE_BOTTOM:
		*y -= 1;
		break;
	case BLOCK_SURFACE_FRONT:
		*z += 1;
		break;
	case BLOCK_SURFACE_BACK:
		*z -= 1;
		break;
	default:
		try(false);
	}
}

typedef struct AtlasIndex AtlasIndex;
struct AtlasIndex {
	size_t x;
	size_t y;
};

static AtlasIndex block_get_surface_texture_atlas_index(
	block_id block,
	block_surface_id surface)
{
	// TODO: Assign the textures to blocks.
	return (AtlasIndex) { 0, 0 };
}

typedef uint32_t block_face_vertex;

static block_face_vertex block_face_vertex_init(
	size_t x,
	size_t y,
	size_t z,
	block_surface_id surface_id,
	AtlasIndex atlas_index)
{
	try(x < CHUNK_SIDELEN);
	try(y < CHUNK_SIDELEN);
	try(z < CHUNK_SIDELEN);
	try(surface_id >= BLOCK_SURFACE_FIRST_ID);
	try(surface_id <= BLOCK_SURFACE_LAST_ID);
	try(atlas_index.x < ATLAS_X_LEN);
	try(atlas_index.y < ATLAS_Y_LEN);

	return
		((uint32_t)x) |
		(((uint32_t)y) << 3) |
		(((uint32_t)z) << 6) |
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
		(((uint32_t)atlas_index.x) << 13) |
		(((uint32_t)atlas_index.y) << 17);
}

typedef uint8_t chunk_generateion_state_id;

#define CHUNK_GENERATION_STATE_NONE ((chunk_generateion_state_id)0)
#define CHUNK_GENERATION_STATE_BLOCKS ((chunk_generateion_state_id)1)
#define CHUNK_GENERATION_STATE_MESH ((chunk_generateion_state_id)2)

#define CHUNK_GENERATION_STATE_FIRST_ID CHUNK_GENERATION_STATE_NONE;
#define CHUNK_GENERATION_STATE_LAST_ID CHUNK_GENERATION_STATE_MESH;

struct Chunk {
	chunk_generateion_state_id generation_state;
	// [x, y, z]
	block_id blocks[CHUNK_SIDELEN * CHUNK_SIDELEN * CHUNK_SIDELEN];
	gl_handle vao;
};

static Chunk chunk_init(void) {
	Chunk chunk = {
		.generation_state = CHUNK_GENERATION_STATE_NONE,
	};
	glGenVertexArrays(1, &chunk.vao);
	return chunk;
}

static void chunk_deinit(Chunk* chunk) {
	glDeleteVertexArrays(1, &chunk->vao);
	chunk->vao = 0;
}

static size_t to_block_index(size_t x, size_t y, size_t z)
{
	try(x < CHUNK_SIDELEN);
	try(y < CHUNK_SIDELEN);
	try(z < CHUNK_SIDELEN);

	return 
		x * CHUNK_SIDELEN * CHUNK_SIDELEN +
		y * CHUNK_SIDELEN +
		z;
}

static void chunk_generate_blocks(
	const Perlin* perlin,
	const FBM* heightmap_fmb,
	Chunk* chunk,
	int32_t chunk_x,
	int32_t chunk_y,
	int32_t chunk_z)
{
	for (int32_t x = 0; x < CHUNK_SIDELEN; x++) {
		for (int32_t z = 0; z < CHUNK_SIDELEN; z++) {
			int32_t height = (int32_t)fbm2(
				perlin,
				heightmap_fmb,
				chunk_x * CHUNK_SIDELEN + x,
				chunk_z * CHUNK_SIDELEN + z
			);
			for (int32_t y = 0; y < CHUNK_SIDELEN; y++) {
				int32_t world_y = chunk_y * CHUNK_SIDELEN + y;
				chunk->blocks[to_block_index(x, y, z)] = world_y > height ? BLOCK_AIR : BLOCK_STONE;
			}
		}
	}
}

struct Chunks {
	size_t sidelen;
	Chunk values[];
};

Chunks* chunks_init(Arena* arena, size_t sidelen) {
	try(sidelen != 0);
	try(SIZE_MAX / sidelen / sidelen / sidelen >= 1);
	size_t chunk_count = sidelen * sidelen * sidelen;
	Chunks* chunks = arena_allocate(
		arena,
		sizeof(Chunks) + chunk_count * sizeof(Chunk),
		_Alignof(Chunks)
	);
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
}

static size_t to_chunk_index(size_t x, size_t y, size_t z, size_t sidelen) {
	return
		x * sidelen * sidelen +
		y * sidelen +
		z;
}

static inline Chunk* chunk_area_chunk_at(const ChunkArea* area, Chunks* chunks, int32_t x, int32_t y, int32_t z) {
	try(x >= area->min_x);
	try(y >= area->min_y);
	try(z >= area->min_z);
	size_t sidelen = chunks->sidelen;
	try(x < area->min_x + sidelen);
	try(y < area->min_y + sidelen);
	try(z < area->min_z + sidelen);

	return &chunks[to_chunk_index(
		(size_t)(x - area->min_x),
		(size_t)(x - area->min_y),
		(size_t)(x - area->min_z),
		sidelen)];
}

static void chunk_area_generate_chunk_mesh(
	const ChunkArea* area,
	Chunks* chunks,
	int32_t chunk_x,
	int32_t chunk_y,
	int32_t chunk_z)
{
	Chunk* chunk = chunk_area_chunk_at(area, chunks, chunk_x, chunk_y, chunk_z);
	block_face_vertex* vertices = NULL;
	size_t vertices_count = 0;
	size_t vertices_capacity = 0;

	for (size_t x = 0; x < CHUNK_SIDELEN; x++) {
		for (size_t y = 0; y < CHUNK_SIDELEN; y++) {
			for (size_t z = 0; z < CHUNK_SIDELEN; z++) {
				block_id block = chunk->blocks[to_block_index(x, y, z)];
				block_surface_culling_id from = 
					block_get_block_surface_culling_strategy(block);
				for (block_surface_id surface_id = BLOCK_SURFACE_FIRST_ID;
					surface_id < BLOCK_SURFACE_LAST_ID;
					surface_id++)
				{
					size_t adj_x = x;
					size_t adj_y = y;
					size_t adj_z = z;
					block_surface_step_towards(surface_id, &adj_x, &adj_y, &adj_z);

					if (adj_x < CHUNK_SIDELEN &&
						adj_y < CHUNK_SIDELEN &&
						adj_z < CHUNK_SIDELEN)
					{
						block_id adj_block = chunk->blocks[to_block_index(adj_x, adj_y, adj_z)];
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

	sfree(vertices);
}

void chunk_area_generate_blocks(
	const ChunkArea* area,
	Chunks* chunks,
	const Perlin* perlin, 
	const FBM* heightmap_fbm) 
{
	size_t sidelen = chunks->sidelen;
	for (int32_t x = area->min_x; x < area->min_x + sidelen; x++) {
		for (int32_t y = area->min_y; y < area->min_y + sidelen; y++) {
			for (int32_t z = area->min_z; z < area->min_z + sidelen; z++) {
				Chunk* chunk = chunk_area_chunk_at(area, chunks, x, y, z);
				if (chunk->generation_state != CHUNK_GENERATION_STATE_NONE) continue;
				chunk_generate_blocks(
					perlin, 
					heightmap_fbm, 
					chunk, 
					x, 
					y, 
					z);
				chunk->generation_state = CHUNK_GENERATION_STATE_BLOCKS;
			}
		}
	}
}

void chunk_area_generate_meshes(const ChunkArea* area, Chunks* chunks) {
	size_t sidelen = chunks->sidelen;
	for (int32_t x = area->min_x; x < area->min_x + sidelen; x++) {
		for (int32_t y = area->min_y; y < area->min_y + sidelen; y++) {
			for (int32_t z = area->min_z; z < area->min_z + sidelen; z++) {
				Chunk* chunk = chunk_area_chunk_at(area, chunks, x, y, z);
				if (chunk->generation_state != CHUNK_GENERATION_STATE_BLOCKS) continue;
				chunk_area_generate_chunk_mesh(area, chunks, x, y, z);
				chunk->generation_state = CHUNK_GENERATION_STATE_MESH;
			}
		}
	}
}
