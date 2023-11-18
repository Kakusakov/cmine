#pragma once
#include "pack.h"
#include "safety.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <float.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

static inline bool f_aprox_eq(float a, float b) {
	return fabs(a - b) < FLT_EPSILON;
}

#define IMPLEMENT_VEC_STRUCT(struct_name, name, type, size, zero, one)\
PACK(struct struct_name {\
	type values[size];\
});\
typedef struct struct_name struct_name;\
static inline type* name##_at(struct_name* const vec, const size_t index) {\
	TRY(index < size);\
	return &vec->values[index];\
}\
static inline type name##_get(const struct_name vec, const size_t index) {\
	TRY(index < size);\
	return vec.values[index];\
}\
static inline struct_name name##_splat(const type value) {\
	struct_name ret;\
	for (size_t i = 0; i < size; i++) {\
		ret.values[i] = value;\
	}\
	return ret;\
}\
static inline struct_name name##_one(void) {\
	return name##_splat(one);\
}\
static inline struct_name name##_zero(void) {\
	return name##_splat(zero);\
}\
static inline struct_name name##_vecadd(const struct_name vec1, const struct_name vec2) {\
	struct_name ret;\
	for (size_t i = 0; i < size; i++) {\
		ret.values[i] = vec1.values[i] + vec2.values[i];\
	}\
	return ret;\
}\
static inline struct_name name##_vecsub(const struct_name vec1, const struct_name vec2) {\
	struct_name ret;\
	for (size_t i = 0; i < size; i++) {\
		ret.values[i] = vec1.values[i] - vec2.values[i];\
	}\
	return ret;\
}\
static inline struct_name name##_neg(const struct_name vec) {\
	return name##_vecsub(name##_zero(), vec);\
}\
static inline struct_name name##_vecmul(const struct_name vec1, const struct_name vec2) {\
	struct_name ret;\
	for (size_t i = 0; i < size; i++) {\
		ret.values[i] = vec1.values[i] * vec2.values[i];\
	}\
	return ret;\
}\
static inline struct_name name##_inv(const struct_name vec) {\
	struct_name ret;\
	for (size_t i = 0; i < size; i++) {\
		ret.values[i] = one / vec.values[i];\
	}\
	return ret;\
}\
static inline struct_name name##_vecdiv(const struct_name vec1, const struct_name vec2) {\
	return name##_vecmul(vec1, name##_inv(vec2));\
}\
static inline struct_name name##_add(const struct_name vec, const type value) {\
	return name##_vecadd(vec, name##_splat(value));\
}\
static inline struct_name name##_sub(const struct_name vec, const type value) {\
	return name##_vecsub(vec, name##_splat(value));\
}\
static inline struct_name name##_mul(const struct_name vec, const type value) {\
	return name##_vecmul(vec, name##_splat(value));\
}\
static inline struct_name name##_div(const struct_name vec, const type value) {\
	return name##_vecdiv(vec, name##_splat(value));\
}

#define IMPLEMENT_FLOAT_VEC_STRUCT(struct_name, name, type, size, zero, one, sqrt) \
IMPLEMENT_VEC_STRUCT(struct_name, name, type, size, zero, one)\
static inline bool name##_aprox_eq(const struct_name vec1, const struct_name vec2) {\
	bool ret = true;\
	for (size_t i = 0; ret && i < size; i++) {\
		ret = ret & f_aprox_eq(vec1.values[i], vec2.values[i]);\
	}\
	return ret;\
}\
static inline type name##_element_sum(const struct_name vec) {\
	type ret = zero;\
	for (size_t i = 0; i < size; i++) {\
		ret += vec.values[i];\
	}\
	return ret;\
}\
static inline type name##_dot(const struct_name vec1, const struct_name vec2) {\
	return name##_element_sum(name##_vecmul(vec1, vec2));\
}\
static inline type name##_sqrlen(const struct_name vec) {\
	return name##_dot(vec, vec);\
}\
static inline type name##_len(const struct_name vec) {\
	return sqrt(name##_sqrlen(vec));\
}\
static inline struct_name name##_normalize(const struct_name vec) {\
	return name##_div(vec, name##_len(vec));\
}

IMPLEMENT_FLOAT_VEC_STRUCT(Vec2f, vec2f, float_t, 2, 0.0f, 1.0f, sqrtf);
IMPLEMENT_FLOAT_VEC_STRUCT(Vec3f, vec3f, float_t, 3, 0.0f, 1.0f, sqrtf);
IMPLEMENT_FLOAT_VEC_STRUCT(Vec4f, vec4f, float_t, 4, 0.0f, 1.0f, sqrtf);

IMPLEMENT_VEC_STRUCT(Vec2s, vec2s, size_t, 2, 0, 1);
IMPLEMENT_VEC_STRUCT(Vec3s, vec3s, size_t, 3, 0, 1);

IMPLEMENT_VEC_STRUCT(Vec2i32, vec2i32, int32_t, 2, 0, 1);
IMPLEMENT_VEC_STRUCT(Vec3i32, vec3i32, int32_t, 3, 0, 1);

IMPLEMENT_VEC_STRUCT(Mat4x4f, mat4x4f, float_t, 16, 0, 1);

static inline Vec3f vec3f_new(const float x, const float y, const float z) {
	Vec3f vec;
	vec.values[0] = x;
	vec.values[1] = y;
	vec.values[2] = z;
	return vec;
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
static inline Vec3f vec3f_left(void) {
	return vec3f_neg(vec3f_right());
}
static inline Vec3f vec3f_down(void) {
	return vec3f_neg(vec3f_up());
}
static inline Vec3f vec3f_backward(void) {
	return vec3f_neg(vec3f_forward());
}

static inline Vec3f vec3f_cross(Vec3f a, Vec3f b) {
	return vec3f_new(
		a.values[1] * b.values[2] - a.values[2] * b.values[1],
		a.values[2] * b.values[0] - a.values[0] * b.values[2],
		a.values[0] * b.values[1] - a.values[1] * b.values[0]
	);
}

static inline Vec4f vec4f_new(const float x, const float y, const float z, const float w) {
	Vec4f vec;
	vec.values[0] = x;
	vec.values[1] = y;
	vec.values[2] = z;
	vec.values[3] = w;
	return vec;
}

static inline Vec4f vec4f_plane(Vec3f point, Vec3f normal) {
	Vec3f normalized = vec3f_normalize(normal);
	return vec4f_new(
		normalized.values[0],
		normalized.values[1],
		normalized.values[2],
		-vec3f_dot(point, normalized)
	);
}

static inline Vec3f vec4f_plane_intersect_line(Vec4f plane, Vec3f point, Vec3f dir) {
	return vec3f_vecadd(
		point,
		vec3f_mul(
			vec3f_neg(dir),
			vec4f_dot(
				vec4f_new(
					point.values[0],
					point.values[1],
					point.values[2],
					1.0f),
				plane) / vec3f_dot(
					vec3f_new(
						plane.values[0],
						plane.values[1],
						plane.values[2]),
					dir)));
}

static inline float mat4x4f_get_element(Mat4x4f mat, size_t row, size_t col) {
	TRY(row < 4);
	TRY(col < 4);
	return mat.values[row * 4 + col];
}

static inline Vec4f mat4x4f_get_row(Mat4x4f mat, size_t index) {
	// assert(index < 4);
	return vec4f_new(
		mat4x4f_get_element(mat, index, 0),
		mat4x4f_get_element(mat, index, 1),
		mat4x4f_get_element(mat, index, 2),
		mat4x4f_get_element(mat, index, 3)
	);
}

static inline Vec4f mat4x4f_col(Mat4x4f mat, size_t index) {
	// assert(index < 4);
	return vec4f_new(
		mat4x4f_get_element(mat, 0, index),
		mat4x4f_get_element(mat, 1, index),
		mat4x4f_get_element(mat, 2, index),
		mat4x4f_get_element(mat, 3, index)
	);
}

static inline Mat4x4f mat4x4f_matmul(Mat4x4f a, Mat4x4f b) {
	Mat4x4f mat;

	mat.values[0] = vec4f_dot(mat4x4f_get_row(a, 0), mat4x4f_get_row(b, 0));
	mat.values[1] = vec4f_dot(mat4x4f_get_row(a, 0), mat4x4f_get_row(b, 1));
	mat.values[2] = vec4f_dot(mat4x4f_get_row(a, 0), mat4x4f_get_row(b, 2));
	mat.values[3] = vec4f_dot(mat4x4f_get_row(a, 0), mat4x4f_get_row(b, 3));

	mat.values[4] = vec4f_dot(mat4x4f_get_row(a, 1), mat4x4f_get_row(b, 0));
	mat.values[5] = vec4f_dot(mat4x4f_get_row(a, 1), mat4x4f_get_row(b, 1));
	mat.values[6] = vec4f_dot(mat4x4f_get_row(a, 1), mat4x4f_get_row(b, 2));
	mat.values[7] = vec4f_dot(mat4x4f_get_row(a, 1), mat4x4f_get_row(b, 3));

	mat.values[8] = vec4f_dot(mat4x4f_get_row(a, 2), mat4x4f_get_row(b, 0));
	mat.values[9] = vec4f_dot(mat4x4f_get_row(a, 2), mat4x4f_get_row(b, 1));
	mat.values[10] = vec4f_dot(mat4x4f_get_row(a, 2), mat4x4f_get_row(b, 2));
	mat.values[11] = vec4f_dot(mat4x4f_get_row(a, 2), mat4x4f_get_row(b, 3));

	mat.values[12] = vec4f_dot(mat4x4f_get_row(a, 3), mat4x4f_get_row(b, 0));
	mat.values[13] = vec4f_dot(mat4x4f_get_row(a, 3), mat4x4f_get_row(b, 1));
	mat.values[14] = vec4f_dot(mat4x4f_get_row(a, 3), mat4x4f_get_row(b, 2));
	mat.values[15] = vec4f_dot(mat4x4f_get_row(a, 3), mat4x4f_get_row(b, 3));

	return mat;
}

static inline Mat4x4f mat4x4f_diagonal_mat(Vec4f value) {
	Mat4x4f mat;

	mat.values[0] = value.values[0];
	mat.values[1] = 0.0f;
	mat.values[2] = 0.0f;
	mat.values[3] = 0.0f;

	mat.values[4] = 0.0f;
	mat.values[5] = value.values[1];
	mat.values[6] = 0.0f;
	mat.values[7] = 0.0f;

	mat.values[8] = 0.0f;
	mat.values[9] = 0.0f;
	mat.values[10] = value.values[2];
	mat.values[11] = 0.0f;

	mat.values[12] = 0.0f;
	mat.values[13] = 0.0f;
	mat.values[14] = 0.0f;
	mat.values[15] = value.values[3];

	return mat;
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
	Mat4x4f mat;

	mat.values[0] = 1.0f;
	mat.values[1] = 0.0f;
	mat.values[2] = 0.0f;
	mat.values[3] = delta.values[0];

	mat.values[4] = 0.0f;
	mat.values[5] = 1.0f;
	mat.values[6] = 0.0f;
	mat.values[7] = delta.values[1];

	mat.values[8] = 0.0f;
	mat.values[9] = 0.0f;
	mat.values[10] = 1.0f;
	mat.values[11] = delta.values[2];

	mat.values[12] = 0.0f;
	mat.values[13] = 0.0f;
	mat.values[14] = 0.0f;
	mat.values[15] = 1.0f;

	return mat;
}

static inline Mat4x4f mat4x4f_scale_mat(Vec3f delta) {
	Mat4x4f mat;

	mat.values[0] = delta.values[0];
	mat.values[1] = 0.0f;
	mat.values[2] = 0.0f;
	mat.values[3] = 0.0f;

	mat.values[4] = 0.0f;
	mat.values[5] = delta.values[1];
	mat.values[6] = 0.0f;
	mat.values[7] = 0.0f;

	mat.values[8] = 0.0f;
	mat.values[9] = 0.0f;
	mat.values[10] = delta.values[2];
	mat.values[11] = 0.0f;

	mat.values[12] = 0.0f;
	mat.values[13] = 0.0f;
	mat.values[14] = 0.0f;
	mat.values[15] = 1.0f;

	return mat;
}

static inline Mat4x4f mat4x4f_rotate_axis_x_mat(float radians) {
	Mat4x4f mat;
	float c = cosf(radians);
	float s = sinf(radians);

	mat.values[0] = 1.0f;
	mat.values[1] = 0.0f;
	mat.values[2] = 0.0f;
	mat.values[3] = 0.0f;

	mat.values[4] = 0.0f;
	mat.values[5] = c;
	mat.values[6] = s;
	mat.values[7] = 0.0f;

	mat.values[8] = 0.0f;
	mat.values[9] = -s;
	mat.values[10] = c;
	mat.values[11] = 0.0f;

	mat.values[12] = 0.0f;
	mat.values[13] = 0.0f;
	mat.values[14] = 0.0f;
	mat.values[15] = 1.0f;

	return mat;
}

static inline Mat4x4f mat4x4f_rotate_axis_y(float radians) {
	Mat4x4f mat;
	float c = cosf(radians);
	float s = sinf(radians);

	mat.values[0] = c;
	mat.values[1] = 0.0f;
	mat.values[2] = -s;
	mat.values[3] = 0.0f;

	mat.values[4] = 0.0f;
	mat.values[5] = 1.0f;
	mat.values[6] = 0.0f;
	mat.values[7] = 0.0f;

	mat.values[8] = s;
	mat.values[9] = 0.0f;
	mat.values[10] = c;
	mat.values[11] = 0.0f;

	mat.values[12] = 0.0f;
	mat.values[13] = 0.0f;
	mat.values[14] = 0.0f;
	mat.values[15] = 1.0f;

	return mat;
}

static inline Mat4x4f mat4x4f_rotate_axis_z(float radians) {
	Mat4x4f mat;
	float c = cosf(radians);
	float s = sinf(radians);

	mat.values[0] = c;
	mat.values[1] = -s;
	mat.values[2] = 0.0f;
	mat.values[3] = 0.0f;

	mat.values[4] = s;
	mat.values[5] = c;
	mat.values[6] = 0.0f;
	mat.values[7] = 0.0f;

	mat.values[8] = 0.0f;
	mat.values[9] = 0.0f;
	mat.values[10] = 1.0f;
	mat.values[11] = 0.0f;

	mat.values[12] = 0.0f;
	mat.values[13] = 0.0f;
	mat.values[14] = 0.0f;
	mat.values[15] = 1.0f;

	return mat;
}

static inline Mat4x4f mat4x4f_ortho_mat(
	float left,
	float right,
	float bottom,
	float top,
	float near,
	float far
) {
	Mat4x4f mat;

	mat.values[0] = 2.0f / (left - right);
	mat.values[1] = 0.0f;
	mat.values[2] = 0.0f;
	mat.values[3] = -(left + right) / (left - right);

	mat.values[4] = 0.0f;
	mat.values[5] = 2.0f / (bottom - top);
	mat.values[6] = 0.0f;
	mat.values[7] = -(bottom + top) / (bottom - top);

	mat.values[8] = 0.0f;
	mat.values[9] = 0.0f;
	mat.values[10] = -2.0f / (far - near);
	mat.values[11] = -(far + near) / (far - near);

	mat.values[12] = 0.0f;
	mat.values[13] = 0.0f;
	mat.values[14] = 0.0f;
	mat.values[15] = 1.0f;

	return mat;
}

static inline Mat4x4f mat4x4f_perspective_mat(
	float aspect,
	float fov_y,
	float near,
	float far
) {
	Mat4x4f mat;
	float f = 1.0f / tanf(fov_y / 2.0f);

	mat.values[0] = f / aspect;
	mat.values[1] = 0.0f;
	mat.values[2] = 0.0f;
	mat.values[3] = 0.0f;

	mat.values[4] = 0.0f;
	mat.values[5] = f;
	mat.values[6] = 0.0f;
	mat.values[7] = 0.0f;

	mat.values[8] = 0.0f;
	mat.values[9] = 0.0f;
	mat.values[10] = (far + near) / (near - far);
	mat.values[11] = (2.0f * far * near) / (near - far);

	mat.values[12] = 0.0f;
	mat.values[13] = 0.0f;
	mat.values[14] = -1.0f;
	mat.values[15] = 0.0f;

	return mat;
}

static inline Mat4x4f mat4x4f_look_at_mat(
	Vec3f from,
	Vec3f to,
	Vec3f up
) {
	Mat4x4f mat;

	Vec3f axis_z = vec3f_normalize(vec3f_vecsub(to, from));
	Vec3f axis_x = vec3f_normalize(vec3f_cross(up, axis_z));
	Vec3f axis_y = vec3f_cross(axis_z, axis_x);

	mat.values[0] = axis_x.values[0];
	mat.values[1] = axis_x.values[1];
	mat.values[2] = axis_x.values[2];
	mat.values[3] = vec3f_dot(axis_x, from);

	mat.values[4] = axis_y.values[0];
	mat.values[5] = axis_y.values[1];
	mat.values[6] = axis_y.values[2];
	mat.values[7] = vec3f_dot(axis_y, from);

	mat.values[8] = axis_z.values[0];
	mat.values[9] = axis_z.values[1];
	mat.values[10] = axis_z.values[2];
	mat.values[11] = vec3f_dot(axis_z, from);

	mat.values[12] = 0.0f;
	mat.values[13] = 0.0f;
	mat.values[14] = 0.0f;
	mat.values[15] = 1.0f;

	return mat;
}

static inline float to_radians(float degrees) {
	return degrees * (float)M_PI / 180.0f;
}

static inline float to_degrees(float radians) {
	return radians * 180.0f / (float)M_PI;
}
