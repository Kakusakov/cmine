#include "chunk.h"
#include <limits.h>
#include <assert.h>
#include <stdlib.h>
#define ASSERT(x) assert(x)

void chunk_init(Chunk* chunk)
{
	*chunk = (Chunk) {0};
}

void chunk_deinit(Chunk *chunk)
{
	chunk_unload(chunk);
}

void chunk_generate_blocks(
	Chunk *chunk,
	const Perlin *perlin,
	Fbm heightmap,
	BPos world_min)
{
	ASSERT(chunk->generation_stage == chunk_generation_stage_awaits_blocks);
	for (int x = 0; x < CHUNK_SIDELEN; x++)
	{
		for (int y = 0; y < CHUNK_SIDELEN; y++)
		{
			float noise = fbm2(
				perlin,
				heightmap,
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
static void add_chunk_face(
	MeshBuilder *mb,
	BPos pos,
	Block block,
	Dir face)
{
	ASSERT(pos.x >= 0);
	ASSERT(pos.y >= 0);
	ASSERT(pos.z >= 0);

	Vec3OpenGL v1;
	Vec3OpenGL v2;
	Vec3OpenGL v3;
	Vec3OpenGL v4;

	switch (face) {
	case dir_nx:
		v1 = (Vec3OpenGL){0, 1, 1};
		v2 = (Vec3OpenGL){0, 0, 1};
		v3 = (Vec3OpenGL){1, 0, 1};
		v4 = (Vec3OpenGL){1, 1, 1};
		break;
	case dir_px:
		v1 = (Vec3OpenGL){1, 1, 0};
		v2 = (Vec3OpenGL){1, 0, 0};
		v3 = (Vec3OpenGL){0, 0, 0};
		v4 = (Vec3OpenGL){0, 1, 0};
		break;
	case dir_py:
		v1 = (Vec3OpenGL){1, 1, 1};
		v2 = (Vec3OpenGL){1, 0, 1};
		v3 = (Vec3OpenGL){1, 0, 0};
		v4 = (Vec3OpenGL){1, 1, 0};
		break;
	case dir_ny:
		v1 = (Vec3OpenGL){0, 1, 0};
		v2 = (Vec3OpenGL){0, 0, 0};
		v3 = (Vec3OpenGL){0, 0, 1};
		v4 = (Vec3OpenGL){0, 1, 1};
		break;
	case dir_pz:
		v1 = (Vec3OpenGL){0, 1, 0};
		v2 = (Vec3OpenGL){0, 1, 1};
		v3 = (Vec3OpenGL){1, 1, 1};
		v4 = (Vec3OpenGL){1, 1, 0};
		break;
	case dir_nz:
		v1 = (Vec3OpenGL){0, 0, 1};
		v2 = (Vec3OpenGL){0, 0, 0};
		v3 = (Vec3OpenGL){1, 0, 0};
		v4 = (Vec3OpenGL){1, 0, 1};
		break;
	default:
		ASSERT(0); 
	}

	Vec3OpenGL vb = v3_to_opengl(bp2p(pos));
	v1 = v3gl_add(v1, vb);
	v2 = v3gl_add(v2, vb);
	v3 = v3gl_add(v3, vb);
	v4 = v3gl_add(v4, vb);

	AtlasTexture texture = block_face_texture(block, face);
	// TODO: get atlas coords form texture...
	Uv uv_base = {
		.u = 0,
		.v = 0,
	};
	float u_step = 1;
	float v_step = 1;

	Uv uv1 = {u_step, v_step};
	Uv uv2 = {u_step, 0};
	Uv uv3 = {0, 0};
	Uv uv4 = {0, v_step};

	uv1.u += uv_base.u; uv1.v += uv_base.v;
	uv2.u += uv_base.u; uv2.v += uv_base.v;
	uv3.u += uv_base.u; uv3.v += uv_base.v;
	uv4.u += uv_base.u; uv4.v += uv_base.v;

	mb_append(mb, (MeshVertex){v1, uv1});
	mb_append(mb, (MeshVertex){v2, uv2});
	mb_append(mb, (MeshVertex){v4, uv4});
	mb_append(mb, (MeshVertex){v2, uv2});
	mb_append(mb, (MeshVertex){v3, uv3});
	mb_append(mb, (MeshVertex){v4, uv4});
}

void chunk_generate_mesh(Chunk *chunk, AdjacentChunks adjacent_chunks)
{
	ASSERT(chunk->generation_stage == chunk_generation_stage_awaits_mesh);
	MeshBuilder mb;
	mb_init(&mb);

	for (int x = 0; x < CHUNK_SIDELEN; x++)
	{
		for (int y = 0; y < CHUNK_SIDELEN; y++)
		{
			for (int z = 0; z < CHUNK_SIDELEN; z++)
			{
				BPos pos = {x, y, z};
				Block block = chunk->blocks[CHUNK_BLOCK_IDX_V(pos)];
				FaceCulling from = block_face_culling(block);
				for (Dir face = 0; face < dir_count; face++)
				{
					BPos norm = dir_normal(face);
					BPos adj = {
						x + norm.x,
						y + norm.y,
						z + norm.z,
					};
					// TODO: cull with adjacent chunk...
					if (from == face_culling_invisible) {
						continue;
					} else if (
						adj.x >= 0 && adj.x < CHUNK_SIDELEN &&
						adj.y >= 0 && adj.y < CHUNK_SIDELEN &&
						adj.z >= 0 && adj.z < CHUNK_SIDELEN)
					{
						Block adj_block = chunk->blocks[CHUNK_BLOCK_IDX_V(adj)];
						FaceCulling to = block_face_culling(adj_block);
						if (should_cull_face(from, to)) continue;
					}
					add_chunk_face(&mb, pos, block, face);
				}
			}
		}
	}

	chunk->mesh = mb_create(&mb);
	mb_deinit(&mb);
}

void chunk_unload(Chunk *chunk)
{
	switch (chunk->generation_stage)
	{
	case chunk_generation_stage_ready:
		mesh_deinit(&chunk->mesh);
	case chunk_generation_stage_awaits_mesh:
	case chunk_generation_stage_awaits_blocks:
		break;
	default:
		ASSERT(0);
		break;
	}
	chunk->generation_stage = 0;
}

void chunks_init(Chunks* chunks, CPos min, size_t sidelen)
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
	if (chunks->items == NULL) abort();

	for (size_t i = 0; i < chunk_count; i++)
	{
		chunk_init(&chunks->items[i]);
	}
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

static int is_world_within_area(CPos pos, ChunkArea area)
{
	return
		pos.x >= area.min.x &&
		pos.y >= area.min.y &&
		pos.z >= area.min.z &&
		pos.x < area.min.x + area.sidelen &&
		pos.y < area.min.y + area.sidelen &&
		pos.z < area.min.z + area.sidelen;
}

static int is_local_within_area(CPos lpos, ChunkArea area)
{
	return
		lpos.x >= 0 &&
		lpos.y >= 0 &&
		lpos.z >= 0 &&
		lpos.x < area.sidelen &&
		lpos.y < area.sidelen &&
		lpos.z < area.sidelen;
}

static CPos cp2lcp(CPos world, ChunkArea area)
{
	ASSERT(is_world_within_area(world, area));
	return (CPos) {
		imodulo(world.x - area.min.x + area.offset.x, (int)area.sidelen),
		imodulo(world.y - area.min.y + area.offset.y, (int)area.sidelen),
		imodulo(world.z - area.min.z + area.offset.z, (int)area.sidelen),
	};
}

static CPos lcp2cp(CPos lpos, ChunkArea area)
{
	ASSERT(is_local_within_area(lpos, area));
	return (CPos) {
		area.min.x + imodulo(lpos.x - area.offset.x, (int)area.sidelen),
		area.min.y + imodulo(lpos.y - area.offset.y, (int)area.sidelen),
		area.min.z + imodulo(lpos.z - area.offset.z, (int)area.sidelen),
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
				CPos lpos = {x, y, z};
				BPos world_min = cp2bp(lcp2cp(lpos, chunks->area));
				chunk_generate_blocks(chunk, perlin, heightmap_fbm, world_min);
			}
		}
	}
}

static Chunk* chunks_chunk(Chunks* chunks, CPos lpos)
{
	if (!is_local_within_area(lpos, chunks->area)) return NULL;
	return &chunks->items[CHUNKS_CHUNK_IDX_V(lpos, chunks->area.sidelen)];
}

static AdjacentChunks chunks_adjacent(Chunks* chunks, CPos lpos)
{
	AdjacentChunks adj;
	adj.items[dir_px] = chunks_chunk(chunks, (CPos){lpos.x + 1, lpos.y, lpos.z});
	adj.items[dir_nx] = chunks_chunk(chunks, (CPos){lpos.x - 1, lpos.y, lpos.z});
	adj.items[dir_py] = chunks_chunk(chunks, (CPos){lpos.x, lpos.y + 1, lpos.z});
	adj.items[dir_ny] = chunks_chunk(chunks, (CPos){lpos.x, lpos.y - 1, lpos.z});
	adj.items[dir_pz] = chunks_chunk(chunks, (CPos){lpos.x, lpos.y, lpos.z + 1});
	adj.items[dir_nz] = chunks_chunk(chunks, (CPos){lpos.x, lpos.y, lpos.z - 1});
	return adj;
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
				CPos lpos = {x, y, z};
				Chunk *chunk = &chunks->items[CHUNKS_CHUNK_IDX_V(lpos, sidelen)];
				chunk_generate_mesh(chunk, chunks_adjacent(chunks, lpos));
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
}

void chunks_draw(Chunks* chunks, Camera cam, Perspective p) {
	size_t sidelen = chunks->area.sidelen;
	for (size_t x = 0; x < sidelen; x++)
	{
		for (size_t y = 0; y < sidelen; y++)
		{
			for (size_t z = 0; z < sidelen; z++)
			{
				CPos lpos = {x, y, z};
				Chunk* chunk = &chunks->items[CHUNKS_CHUNK_IDX_V(lpos, sidelen)];
				Camera c = cam;
				c.pos = v3_add(cam.pos, bp2p(cp2bp(lcp2cp(lpos, chunks->area))));
				mesh_draw(&chunk->mesh, render_tmp_texture(), c, p);
			}
		}
	}
}
