#pragma once
#include <stddef.h>
#include <stdbool.h>
#include "pack.h"

// Coordinate system is right-handed, Y is up.

struct Vec3f {
	float values[3];
};
typedef struct Vec3f Vec3f;

PACK(struct Vec4f {
	float values[4];
});
typedef struct Vec4f Vec4f;

PACK(struct Mat4x4f {
	float values[4 * 4];
});
typedef struct Mat4x4f Mat4x4f;

#include <assert.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <float.h>

static inline bool f_aprox_eq(float a, float b) {
	return fabs(a - b) < FLT_EPSILON;
}

static inline bool vec3f_aprox_eq(Vec3f a, Vec3f b) {
	return f_aprox_eq(a.values[0], b.values[0]) &&
		f_aprox_eq(a.values[1], b.values[1]) &&
		f_aprox_eq(a.values[2], b.values[2]);
}

static inline float vec3f_at(Vec3f vec, size_t index) {
	assert(index < 3);
	return vec.values[index];
}

static inline float vec3f_x(Vec3f vec) {
	return vec3f_at(vec, 0);
}

static inline float vec3f_y(Vec3f vec) {
	return vec3f_at(vec, 1);
}

static inline float vec3f_z(Vec3f vec) {
	return vec3f_at(vec, 2);
}

static inline Vec3f vec3f_new(float x, float y, float z) {
	return (Vec3f) {
		x, y, z
	};
}

static inline Vec3f vec3f_right(void) {
	return vec3f_new(1.0f, 0.0f, 0.0f);
}

static inline Vec3f vec3f_up(void) {
	return vec3f_new(0.0f, 1.0f, 0.0f);
}

static inline Vec3f vec3f_forward(void) {
	return vec3f_new(0.0f, 0.0f, 1.0f);
}

static inline Vec3f vec3f_splat(float value) {
	return vec3f_new(value, value, value);
}

static inline Vec3f vec3f_zero(void) {
	return vec3f_splat(0);
}

static inline Vec3f vec3f_one(void) {
	return vec3f_splat(1);
}

static inline Vec3f vec3f_vecadd(Vec3f a, Vec3f b) {
	return vec3f_new(
		vec3f_x(a) + vec3f_x(b),
		vec3f_y(a) + vec3f_y(b),
		vec3f_z(a) + vec3f_z(b)
	);
}

static inline Vec3f vec3f_vecsub(Vec3f a, Vec3f b) {
	return vec3f_new(
		vec3f_x(a) - vec3f_x(b),
		vec3f_y(a) - vec3f_y(b),
		vec3f_z(a) - vec3f_z(b)
	);
}

static inline Vec3f vec3f_vecmul(Vec3f a, Vec3f b) {
	return vec3f_new(
		vec3f_x(a) * vec3f_x(b),
		vec3f_y(a) * vec3f_y(b),
		vec3f_z(a) * vec3f_z(b)
	);
}

static inline Vec3f vec3f_vecdiv(Vec3f a, Vec3f b) {
	return vec3f_new(
		vec3f_x(a) / vec3f_x(b),
		vec3f_y(a) / vec3f_y(b),
		vec3f_z(a) / vec3f_z(b)
	);
}

static inline Vec3f vec3f_add(Vec3f a, float b) {
	return vec3f_vecadd(a, vec3f_splat(b));
}

static inline Vec3f vec3f_sub(Vec3f a, float b) {
	return vec3f_vecsub(a, vec3f_splat(b));
}

static inline Vec3f vec3f_mul(Vec3f a, float b) {
	return vec3f_vecmul(a, vec3f_splat(b));
}

static inline Vec3f vec3f_div(Vec3f a, float b) {
	return vec3f_vecdiv(a, vec3f_splat(b));
}

static inline Vec3f vec3f_neg(Vec3f a) {
	return vec3f_vecsub(vec3f_zero(), a);
}

static inline Vec3f vec3f_left(void) {
	return vec3f_neg(vec3f_right());
}

static inline Vec3f vec3f_down(void) {
	return vec3f_neg(vec3f_up());
}

static inline Vec3f vec3f_backward(void) {
	return vec3f_neg(vec3f_forward());
}

static inline float vec3f_axis_sum(Vec3f vec) {
	return vec3f_x(vec) + vec3f_y(vec) + vec3f_z(vec);
}

static inline float vec3f_dot(Vec3f a, Vec3f b) {
	return vec3f_axis_sum(vec3f_vecmul(a, b));
}

static inline Vec3f vec3f_cross(Vec3f a, Vec3f b) {
	return vec3f_new(
		vec3f_y(a) * vec3f_z(b) - vec3f_z(a) * vec3f_y(b),
		vec3f_z(a) * vec3f_x(b) - vec3f_x(a) * vec3f_z(b),
		vec3f_x(a) * vec3f_y(b) - vec3f_y(a) * vec3f_x(b)
	);
}

static inline float vec3f_sqrlen(Vec3f a) {
	return vec3f_dot(a, a);
}

static inline float vec3f_len(Vec3f a) {
	return sqrtf(vec3f_sqrlen(a));
}

static inline Vec3f vec3f_normalize(Vec3f a) {
	return vec3f_div(a, vec3f_len(a));
}

static inline float vec4f_at(Vec4f vec, size_t index) {
	assert(index < 4);
	return vec.values[index];
}

static inline float vec4f_x(Vec4f vec) {
	return vec4f_at(vec, 0);
}

static inline float vec4f_y(Vec4f vec) {
	return vec4f_at(vec, 1);
}

static inline float vec4f_z(Vec4f vec) {
	return vec4f_at(vec, 2);
}

static inline float vec4f_w(Vec4f vec) {
	return vec4f_at(vec, 3);
}

static inline Vec3f vec4f_xyz(Vec4f vec) {
	return vec3f_new(vec4f_x(vec), vec4f_w(vec), vec4f_z(vec));
}

static inline Vec4f vec4f_new(float x, float y, float z, float w) {
	return (Vec4f) {
		x, y, z, w
	};
}

static inline Vec4f vec4f_splat(float value) {
	return vec4f_new(value, value, value, value);
}

static inline Vec4f vec4f_plane(Vec3f point, Vec3f normal) {
	Vec3f normalized = vec3f_normalize(normal);
	return vec4f_new(
		vec3f_x(normalized),
		vec3f_y(normalized),
		vec3f_z(normalized),
		-vec3f_dot(point, normalized)
	);
}

static inline Vec4f vec4f_vecadd(Vec4f a, Vec4f b) {
	return vec4f_new(
		vec4f_x(a) + vec4f_x(b),
		vec4f_y(a) + vec4f_y(b),
		vec4f_z(a) + vec4f_z(b),
		vec4f_w(a) + vec4f_w(b)
	);
}

static inline Vec4f vec4f_vecsub(Vec4f a, Vec4f b) {
	return vec4f_new(
		vec4f_x(a) - vec4f_x(b),
		vec4f_y(a) - vec4f_y(b),
		vec4f_z(a) - vec4f_z(b),
		vec4f_w(a) - vec4f_w(b)
	);
}

static inline Vec4f vec4f_vecmul(Vec4f a, Vec4f b) {
	return vec4f_new(
		vec4f_x(a) * vec4f_x(b),
		vec4f_y(a) * vec4f_y(b),
		vec4f_z(a) * vec4f_z(b),
		vec4f_w(a) * vec4f_w(b)
	);
}

static inline Vec4f vec4f_vecdiv(Vec4f a, Vec4f b) {
	return vec4f_new(
		vec4f_x(a) / vec4f_x(b),
		vec4f_y(a) / vec4f_y(b),
		vec4f_z(a) / vec4f_z(b),
		vec4f_w(a) / vec4f_w(b)
	);
}

static inline float vec4f_axis_sum(Vec4f vec) {
	return vec4f_x(vec) + vec4f_y(vec) + vec4f_z(vec) + vec4f_w(vec);
}

static inline float vec4f_dot(Vec4f a, Vec4f b) {
	return vec4f_axis_sum(vec4f_vecmul(a, b));
}

static inline Vec3f vec4f_plane_intersect_line(Vec4f plane, Vec3f point, Vec3f dir) {
	return vec3f_vecadd(
		point,
		vec3f_mul(
			vec3f_neg(dir),
			vec4f_dot(
				vec4f_new(
					vec3f_x(point),
					vec3f_y(point),
					vec3f_z(point),
					1.0f
				),
				plane
			) / vec3f_dot(vec4f_xyz(plane), dir)
		)
	);
}

static inline float mat4x4f_at(Mat4x4f mat, size_t row, size_t col) {
	assert(row < 4);
	assert(col < 4);
	return mat.values[row * 4 + col];
}

static inline Vec4f mat4x4f_row(Mat4x4f mat, size_t index) {
	// assert(index < 4);
	return vec4f_new(
		mat4x4f_at(mat, index, 0),
		mat4x4f_at(mat, index, 1),
		mat4x4f_at(mat, index, 2),
		mat4x4f_at(mat, index, 3)
	);
}

static inline Vec4f mat4x4f_col(Mat4x4f mat, size_t index) {
	// assert(index < 4);
	return vec4f_new(
		mat4x4f_at(mat, 0, index),
		mat4x4f_at(mat, 1, index),
		mat4x4f_at(mat, 2, index),
		mat4x4f_at(mat, 3, index)
	);
}

static inline Mat4x4f mat4x4f_matmul(Mat4x4f a, Mat4x4f b) {
	return (Mat4x4f) {
		vec4f_dot(mat4x4f_row(a, 0), mat4x4f_row(b, 0)),
		vec4f_dot(mat4x4f_row(a, 0), mat4x4f_row(b, 1)),
		vec4f_dot(mat4x4f_row(a, 0), mat4x4f_row(b, 2)),
		vec4f_dot(mat4x4f_row(a, 0), mat4x4f_row(b, 3)),

		vec4f_dot(mat4x4f_row(a, 1), mat4x4f_row(b, 0)),
		vec4f_dot(mat4x4f_row(a, 1), mat4x4f_row(b, 1)),
		vec4f_dot(mat4x4f_row(a, 1), mat4x4f_row(b, 2)),
		vec4f_dot(mat4x4f_row(a, 1), mat4x4f_row(b, 3)),

		vec4f_dot(mat4x4f_row(a, 2), mat4x4f_row(b, 0)),
		vec4f_dot(mat4x4f_row(a, 2), mat4x4f_row(b, 1)),
		vec4f_dot(mat4x4f_row(a, 2), mat4x4f_row(b, 2)),
		vec4f_dot(mat4x4f_row(a, 2), mat4x4f_row(b, 3)),

		vec4f_dot(mat4x4f_row(a, 3), mat4x4f_row(b, 0)),
		vec4f_dot(mat4x4f_row(a, 3), mat4x4f_row(b, 1)),
		vec4f_dot(mat4x4f_row(a, 3), mat4x4f_row(b, 2)),
		vec4f_dot(mat4x4f_row(a, 3), mat4x4f_row(b, 3))
	};
}

static inline Mat4x4f mat4x4f_diagonal_mat(Vec4f value) {
	return (Mat4x4f) {
		vec4f_x(value), 0.0f, 0.0f, 0.0f,
		0.0f, vec4f_y(value), 0.0f, 0.0f,
		0.0f, 0.0f, vec4f_z(value), 0.0f,
		0.0f, 0.0f, 0.0f, vec4f_w(value)
	};
}

static inline Mat4x4f mat4x4f_scalar_mat(float value) {
	return mat4x4f_diagonal_mat(vec4f_new(value, value, value, value));
}

static inline Mat4x4f mat4x4f_identity_mat(void) {
	return mat4x4f_scalar_mat(1.0f);
}

static inline Mat4x4f mat4x4f_null_mat(void) {
	return mat4x4f_scalar_mat(0.0f);
}

static inline Mat4x4f mat4x4f_translate_mat(Vec3f delta) {
	return (Mat4x4f) {
		1.0f, 0.0f, 0.0f, vec3f_x(delta),
		0.0f, 1.0f, 0.0f, vec3f_y(delta),
		0.0f, 0.0f, 1.0f, vec3f_z(delta),
		0.0f, 0.0f, 0.0f, 1.0f
	};
}

static inline Mat4x4f mat4x4f_scale_mat(Vec3f delta) {
	return (Mat4x4f) {
		vec3f_x(delta), 0.0f, 0.0f, 0.0f,
		0.0f, vec3f_y(delta), 0.0f, 0.0f,
		0.0f, 0.0f, vec3f_z(delta), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
}

static inline Mat4x4f mat4x4f_rotate_axis_x_mat(float radians) {
	float c = cosf(radians);
	float s = sinf(radians);

	return (Mat4x4f) {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, c, s, 0.0f,
		0.0f, -s, c, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
}

static inline Mat4x4f mat4x4f_rotate_axis_y(float radians) {
	float c = cosf(radians);
	float s = sinf(radians);

	return (Mat4x4f) {
		c, 0.0f, -s, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		s, 0.0f, c, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
}

static inline Mat4x4f mat4x4f_rotate_axis_z(float radians) {
	float c = cosf(radians);
	float s = sinf(radians);

	return (Mat4x4f) {
		c, -s, 0.0f, 0.0f,
		s, c, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
}

static inline Mat4x4f mat4x4f_ortho_mat(
	float left,
	float right,
	float bottom,
	float top,
	float near,
	float far
) {
	return (Mat4x4f) {
		2.0f / (left - right), 0.0f, 0.0f, -(left + right) / (left - right),
		0.0f, 2.0f / (bottom - top), 0.0f, -(bottom + top) / (bottom - top),
		0.0f, 0.0f, -2.0f / (far - near), -(far + near) / (far - near),
		0.0f, 0.0f, 0.0f, 1.0f
	};
}

static inline Mat4x4f mat4x4f_perspective_mat(
	float aspect,
	float fov_y,
	float near,
	float far
) {
	float f = 1.0f / tanf(fov_y / 2.0f);
	return (Mat4x4f) {
		f / aspect, 0.0f, 0.0f, 0.0f,
		0.0f, f, 0.0f, 0.0f,
		0.0f, 0.0f, (far + near) / (near - far), (2.0f * far * near) / (near - far),
		0.0f, 0.0f, -1.0f, 0.0f
	};
}

static inline Mat4x4f mat4x4f_look_at_mat(
	Vec3f from,
	Vec3f to,
	Vec3f up
) {
	Vec3f axis_z = vec3f_normalize(vec3f_vecsub(to, from));
	Vec3f axis_x = vec3f_normalize(vec3f_cross(up, axis_z));
	Vec3f axis_y = vec3f_cross(axis_z, axis_x);

	return (Mat4x4f) {
		vec3f_x(axis_x), vec3f_y(axis_x), vec3f_z(axis_x), vec3f_dot(axis_x, from),
		vec3f_x(axis_y), vec3f_y(axis_y), vec3f_z(axis_y), vec3f_dot(axis_y, from),
		vec3f_x(axis_z), vec3f_y(axis_z), vec3f_z(axis_z), vec3f_dot(axis_z, from),
		0.0f, 0.0f, 0.0f, 1.0f
	};
}

static inline float to_radians(float degrees) {
	return degrees * M_PI / 180.0f;
}

static inline float to_degrees(float radians) {
	return radians * 180.0f / M_PI;
}
