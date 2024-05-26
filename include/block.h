#pragma once
#include "vmath.h"
#include "assert.h"
#define ASSERT(x) assert(x)

// Coordinate system:
// X is forward
// Y is right
// Z is up

typedef struct BPos BPos;
struct BPos {
	int x;
	int y;
	int z;
};

typedef struct CPos CPos;
struct CPos {
	int x;
	int y;
	int z;
};

// Converts from the game's coordinate system to opengl coordinate system.
static inline Vec3 p2gl(Vec3 pos)
{
	return (Vec3) {
		.x = pos.y,
		.y = pos.z,
		.z = -pos.x,
	};
}

static inline Vec3 bp2p(BPos pos) {
	return (Vec3) {
		.x = (float)pos.x,
		.y = (float)pos.y,
		.z = (float)pos.z,
	};
}

// A direction on any 3D grid.
typedef enum Dir Dir;
enum Dir {
	dir_px,
	dir_nx,
	dir_py,
	dir_ny,
	dir_pz,
	dir_nz,
	dir_count
};

inline Dir dir_inverse(Dir dir) {
	switch (dir) {
	case dir_px: return dir_nx;
	case dir_nx: return dir_px;
	case dir_py: return dir_ny;
	case dir_ny: return dir_py;
	case dir_pz: return dir_nz;
	case dir_nz: return dir_pz;
	default:
		ASSERT(0);
	}
}

inline BPos dir_normal(Dir dir) {
	BPos n = { 0 };
	switch (dir) {
	case dir_px: n.x += 1; break;
	case dir_nx: n.x -= 1; break;
	case dir_py: n.y += 1; break;
	case dir_ny: n.y -= 1; break;
	case dir_pz: n.z += 1; break;
	case dir_nz: n.z -= 1; break;
	default:
		ASSERT(0);
	}
	return n;
}

typedef unsigned char Block;
enum
{
	block_unknown,
	block_air,
	block_stone,
	block_count
};

typedef enum AtlasTexture AtlasTexture;
enum AtlasTexture {
	atlas_none,
	atlas_stone,
	atlas_count
};

typedef enum FaceCulling FaceCulling;
enum FaceCulling
{
	// Surface never has a texure.
	face_culling_invisible,
	// Surface has a texture only if the adjacent face is invisible.
	face_culling_transparent,
	// Surface has a texture only if the adjacent face is invisible or transparent.
	face_culling_solid,
	face_culling_count
};

inline AtlasTexture block_face_texture(Block block, Dir face) {
	switch (block) {
	case block_air:   return atlas_none;
	case block_stone: return atlas_stone;
	default:
		ASSERT(0);
	}
}

inline FaceCulling block_face_culling(Block block) {
	switch (block) {
	case block_air:   return face_culling_invisible;
	case block_stone: return face_culling_solid;
	default:
		ASSERT(0);
	}
}

inline int should_cull_face(FaceCulling from, FaceCulling to) {
	return from <= to;
}
