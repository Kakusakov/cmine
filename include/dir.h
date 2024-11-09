#pragma once
#include "vec.h"

typedef struct Dir3 Dir3;
struct Dir3 {
	f32 x;
	f32 y;
	f32 z;
};

static inline Dir3 dir3_from_vec(Vec3 dir) {
	Vec3 normal = v3_normal(dir);
	Dir3 result = {
		.x = normal.x,
		.y = normal.y,
		.z = normal.z,
	};
	return result;
}
static inline Vec3 dir3_as_unit_vec(Dir3 dir) {
	return v3_new(dir.x, dir.y, dir.z);
}
static inline Vec3 dir3_with_magnitude(Dir3 dir, f32 mag) {
	return v3_scale(dir3_as_unit_vec(dir), mag);
}

typedef struct Dir2 Dir2;
struct Dir2 {
	f32 x;
	f32 y;
};

static inline Dir2 dir2_from_vec(Vec2 dir) {
	Vec2 normal = v2_normal(dir);
	Dir2 result = {
		.x = normal.x,
		.y = normal.y,
	};
	return result;
}
static inline Vec2 dir2_as_unit_vec(Dir2 dir) {
	return v2_new(dir.x, dir.y);
}
static inline Vec2 dir2_with_magnitude(Dir2 dir, f32 mag) {
	return v2_scale(dir2_as_unit_vec(dir), mag);
}
