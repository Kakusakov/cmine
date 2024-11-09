#pragma once
#include "quat.h"

typedef struct Mat4x4 Mat4x4;
struct Mat4x4 {
	f32 arr[4*4];
};
static inline Mat4x4 mat4x4_mul(Mat4x4 a, Mat4x4 b) {
	Mat4x4 result;
	for (size_t y = 0; y < 4; y++) {
		for (size_t x = 0; x < 4; x++) {
			result.arr[y*4 + x] =
				a.arr[4*y + 0]*b.arr[4*0 + x] +
				a.arr[4*y + 1]*b.arr[4*1 + x] +
				a.arr[4*y + 2]*b.arr[4*2 + x] +
				a.arr[4*y + 3]*b.arr[4*3 + x];
		}
	}
	return result;
}

typedef struct Mat3x3 Mat3x3;
struct Mat3x3 {
	f32 arr[3*3];
};
static inline Mat3x3 mat3x3_from_rows(
	Vec3OpenGL x,
	Vec3OpenGL y,
	Vec3OpenGL z
) {
	Mat3x3 result = {
		x.x, x.y, x.z,
		y.x, y.y, y.z,
		z.x, z.y, z.z,
	};
	return result;
}
static inline Mat3x3 mat3x3_from_cols(
	Vec3OpenGL x,
	Vec3OpenGL y,
	Vec3OpenGL z
) {
	Mat3x3 result = {
		x.x, y.x, z.x,
		x.y, y.y, z.y,
		x.z, y.z, z.z,
	};
	return result;
}
static inline Mat3x3 mat3x3_from_scale(Vec3 v) {
	Vec3OpenGL t = v3_opengl(v);
	Mat3x3 result = {
		t.x, 0, 0,
		0, t.y, 0,
		0, 0, t.z,
	};
	return result;
}
static inline Mat3x3 mat3x3_from_rotation(Quat q) {
	QuatOpenGL t = quat_opengl(q);
	f32 x = t.x;
	f32 y = t.y;
	f32 z = t.z;
	f32 s = t.w;
	Mat3x3 result = {
		1 - 2*y*y - 2*z*z, 2*x*y - 2*s*z, 2*x*z + 2*s*y,
		2*x*y + 2*s*z, 1 - 2*x*x - 2*z*z, 2*y*z - 2*s*x,
		2*x*z - 2*s*y, 2*y*z + 2*s*x, 1 - 2*x*x - 2*y*y,
	};
	return result;
}
static inline Mat3x3 mat3x3_mul(Mat3x3 a, Mat3x3 b) {
	Mat3x3 result;
	for (size_t y = 0; y < 3; y++) {
		for (size_t x = 0; x < 3; x++) {
			result.arr[y * 3 + x] =
				a.arr[3*y + 0]*b.arr[3*0 + x] +
				a.arr[3*y + 1]*b.arr[3*1 + x] +
				a.arr[3*y + 2]*b.arr[3*2 + x];
		}
	}
	return result;
}

typedef struct Mat4x3 Mat4x3;
struct Mat4x3 {
	Mat3x3 rotscale;
	Vec3OpenGL position;
};
static inline Mat4x4 mat4x3_to_transform(Mat4x3 m) {
	Mat4x4 result = {
		m.rotscale.arr[0], m.rotscale.arr[1], m.rotscale.arr[2], 0,
		m.rotscale.arr[3], m.rotscale.arr[4], m.rotscale.arr[5], 0,
		m.rotscale.arr[6], m.rotscale.arr[7], m.rotscale.arr[8], 0,
		m.position.x,      m.position.y,      m.position.z,      1,
	};
	return result;
}
static inline Mat4x3 mat4x3_look_at(
	Vec3 from,
	Dir3 dir,
	Dir3 up)
{
	Vec3OpenGL ax_z = v3_opengl(dir3_as_unit_vec(dir));
	Vec3OpenGL ax_x = vgl_normal(vgl_cross(v3_opengl(dir3_as_unit_vec(up)), ax_z));
	Vec3OpenGL ax_y = vgl_cross(ax_z, ax_x);

	Vec3OpenGL position = vgl_new(
		vgl_dot(ax_x, v3_opengl(from)),
		vgl_dot(ax_y, v3_opengl(from)),
		vgl_dot(ax_z, v3_opengl(from))
	);
	Mat3x3 rotscale = mat3x3_from_cols(
		ax_x,
		ax_y,
		ax_z
	);
	Mat4x3 result = {
		.position = position,
		.rotscale = rotscale,
	};
	return result;
}/*
static inline void mat4x3_translate(Mat4x3* m, Vec3 v) {
	Vec3OpenGL t = v3_opengl(v);
	m->position.x += t.x;
	m->position.y += t.y;
	m->position.z += t.z;
}
static inline void mat4x3_scale(Mat4x3* m, Vec3 v) {
	m->rotscale = mat3x3_mul(mat3x3_from_scale(v), m->rotscale);
}
static inline void mat4x3_rotate(Mat4x3* m, Quat q) {
	m->rotscale = mat3x3_mul(mat3x3_from_rotation(q), m->rotscale);
}*/

static inline Mat4x4 mat4x4_ortho(
	f32 left,   f32 right,
	f32 bottom, f32 top,
	f32 near,   f32 far
) {
	Vec3 p = v3_new(
		-(left + right)/(left - right),
		-(bottom + top)/(bottom - top),
		-(far + near)/(far - near)
	);
	Vec3 s = v3_new(
		2/(left - right),
		2/(bottom - top),
		-2/(far - near)
	);
	Mat4x4 result = {
		s.x, 0,   0,   0,
		0,   s.y, 0,   0,
		0,   0,   s.z, 0,
		p.x, p.y, p.z, 1,
	};
	return result;
}
static inline Mat4x4 mat4x4_persp(
	f32 aspect, f32 fovz,
	f32 near,   f32 far
) {
	f32 f = 1/tanf(fovz/2);
	Vec3 s = v3_new(
		f / aspect,
		f,
		(far + near)/(near - far)
	);
	f32 z = (2*far*near)/(near - far);
	Mat4x4 result = {
		s.x, 0,   0,    0,
		0,   s.y, 0,    0,
		0,   0,   s.z, -1,
		0,   0,   z,    0,
	};
	return result;
}
