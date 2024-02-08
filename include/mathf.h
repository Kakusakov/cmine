#pragma once
#include <math.h>
#include "fixed_types.h"
#include "safety.h"

#define PI 3.14159265358979323846

static inline float32_t to_radians(const float32_t degrees) {
	return degrees * (float32_t)PI / (float32_t)180.0;
}
static inline float32_t to_degrees(const float32_t radians) {
	return radians * (float32_t)180.0 / (float32_t)PI;
}

typedef struct Vec3f Vec3f;
struct Vec3f {
	float32_t x;
	float32_t y;
	float32_t z;
};

static inline Vec3f vec3f_add_v(Vec3f a, Vec3f b) {
	return (Vec3f) { a.x + b.x, a.y + b.y, a.z + b.z };
}
static inline Vec3f vec3f_sub_v(Vec3f a, Vec3f b) {
	return (Vec3f) { a.x - b.x, a.y - b.y, a.z - b.z };
}
static inline Vec3f vec3f_mul_v(Vec3f a, Vec3f b) {
	return (Vec3f) { a.x * b.x, a.y * b.y, a.z * b.z };
}
static inline Vec3f vec3f_div_v(Vec3f a, Vec3f b) {
	try(b.x != 0.0f && b.y != 0.0f && b.z != 0.0f);
	return (Vec3f) { a.x / b.x, a.y / b.y, a.z / b.z };
}

static inline Vec3f vec3f_add(Vec3f a, float32_t value) {
	return (Vec3f) { a.x + value, a.y + value, a.z + value };
}
static inline Vec3f vec3f_sub(Vec3f a, float32_t value) {
	return (Vec3f) { a.x - value, a.y - value, a.z - value };
}
static inline Vec3f vec3f_mul(Vec3f a, float32_t value) {
	return (Vec3f) { a.x * value, a.y * value, a.z * value };
}
static inline Vec3f vec3f_div(Vec3f a, float32_t value) {
	try(value != 0.0f);
	return (Vec3f) { a.x / value, a.y / value, a.z / value };
}

static inline float32_t vec3f_dot(Vec3f a, Vec3f b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}
static inline Vec3f vec3f_cross(Vec3f a, Vec3f b) {
	return (Vec3f) {
		a.y* b.z - a.z * b.y,
			a.z* b.x - a.x * b.z,
			a.x* b.y - a.y * b.x,
	};
}

static inline float32_t vec3f_sqrlen(Vec3f vec) {
	return vec.x * vec.x + vec.y * vec.y + vec.z * vec.z;
}
static inline float32_t vec3f_len(Vec3f vec) {
	return sqrtf(vec3f_sqrlen(vec));
}
static inline Vec3f vec3f_neg(Vec3f vec) {
	return (Vec3f) { -vec.x, -vec.y, -vec.z };
}
static inline Vec3f vec3f_normalize(Vec3f vec) {
	return vec3f_div(vec, vec3f_len(vec));
}

static inline Vec3f vec3f_splat(float32_t value) {
	return (Vec3f) { value, value, value };
}
static inline Vec3f vec3f_right(void) {
	return (Vec3f) { 1.0f, 0.0f, 0.0f };
}
static inline Vec3f vec3f_up(void) {
	return (Vec3f) { 0.0f, 1.0f, 0.0f };
}
static inline Vec3f vec3f_forward(void) {
	return (Vec3f) { 0.0f, 0.0f, 1.0f };
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

typedef struct Mat4x4 Mat4x4;
struct Mat4x4 {
	float32_t v[4 * 4];
};

static inline Mat4x4 mat4x4_matmul(Mat4x4 rows, Mat4x4 cols) {
	Mat4x4 mat;
	for (size_t y = 0; y < 4; y++) {
		for (size_t x = 0; x < 4; x++) {
			float32_t dot = 0.0f;
			for (size_t i = 0; i < 4; i++) {
				dot += rows.v[y * 4 + i] * cols.v[i * 4 + x];
			}
			mat.v[y * 4 + x] = dot;
		}
	}
	return mat;
}

static inline Mat4x4 mat4x4_null(void) {
	return (Mat4x4) { 0 };
}

static inline Mat4x4 mat4x4_identity(void) {
	Mat4x4 mat = mat4x4_null();
	for (size_t i = 0; i < 4; i++) {
		mat.v[i * 4 + i] = 1.0f;
	}
	return mat;
}

static inline Mat4x4 mat4x4_translate(Vec3f translate) {
	Mat4x4 mat = mat4x4_identity();
	mat.v[12] = translate.x;
	mat.v[13] = translate.y;
	mat.v[14] = translate.z;
	return mat;
}

static inline Mat4x4 mat4x4_scale(Vec3f scale) {
	Mat4x4 mat = mat4x4_identity();
	mat.v[4 * 0 + 0] = scale.x;
	mat.v[4 * 1 + 1] = scale.y;
	mat.v[4 * 2 + 2] = scale.z;
	return mat;
}

static inline Mat4x4 mat4x4_ortho(
	float32_t left,
	float32_t right,
	float32_t bottom,
	float32_t top,
	float32_t near,
	float32_t far)
{
	Mat4x4 mat = mat4x4_null();

	mat.v[0 * 4 + 0] = 2.0f / (left - right);
	mat.v[1 * 4 + 1] = 2.0f / (bottom - top);
	mat.v[2 * 4 + 2] = -2.0f / (far - near);

	mat.v[3 * 4 + 0] = -(left + right) / (left - right);
	mat.v[3 * 4 + 1] = -(bottom + top) / (bottom - top);
	mat.v[3 * 4 + 2] = -(far + near) / (far - near);
	mat.v[3 * 4 + 3] = 1.0f;

	return mat;
}

static inline Mat4x4 mat4x4_perspective(
	float32_t aspect,
	float32_t fov_y,
	float32_t near,
	float32_t far)
{
	Mat4x4 mat = mat4x4_null();
	float32_t f = 1.0f / tanf(fov_y / 2.0f);

	mat.v[4 * 0 + 0] = f / aspect;
	mat.v[4 * 1 + 1] = f;
	mat.v[4 * 2 + 2] = (far + near) / (near - far);
	mat.v[4 * 2 + 3] = -1.0f;
	mat.v[4 * 3 + 2] = (2.0f * far * near) / (near - far);

	return mat;
}

static inline Mat4x4 mat4x4_look_at(
	Vec3f from,
	Vec3f to,
	Vec3f up)
{
	Mat4x4 mat = mat4x4_null();
	Vec3f delta = {
		to.x - from.x,
		to.y - from.y,
		to.z - from.z,
	};
	Vec3f axis_z = vec3f_normalize(delta);
	Vec3f axis_x = vec3f_normalize(vec3f_cross(up, axis_z));
	Vec3f axis_y = vec3f_cross(axis_z, axis_x);

	mat.v[0] = axis_x.x;
	mat.v[1] = axis_y.x;
	mat.v[2] = axis_z.x;

	mat.v[4] = axis_x.y;
	mat.v[5] = axis_y.y;
	mat.v[6] = axis_z.y;

	mat.v[8] = axis_x.z;
	mat.v[9] = axis_y.z;
	mat.v[10] = axis_z.z;

	mat.v[12] = vec3f_dot(axis_x, from);
	mat.v[13] = vec3f_dot(axis_y, from);
	mat.v[14] = vec3f_dot(axis_z, from);
	mat.v[15] = 1.0f;

	return mat;
}

//static inline Vec4f vec4f_plane(const Vec3f point, const Vec3f normal) {
//	const Vec3f normalized = vec3f_normalize(normal);
//	return vec4f_new(
//		normalized.vs[0],
//		normalized.vs[1],
//		normalized.vs[2],
//		-vec3f_dot(point, normalized)
//	);
//}
//
//static inline Vec3f vec4f_plane_intersect_line(
//	const Vec4f plane,
//	const Vec3f point,
//	const Vec3f dir)
//{
//	return vec3f_vecadd(
//		point,
//		vec3f_mul(
//			vec3f_neg(dir),
//			vec4f_dot(
//				vec4f_new(
//					point.vs[0],
//					point.vs[1],
//					point.vs[2],
//					1.0f),
//				plane) / vec3f_dot(
//					vec3f_new(
//						plane.vs[0],
//						plane.vs[1],
//						plane.vs[2]),
//					dir)));
//}

//static inline mat4x4 mat4x4_z_up_to_y_up(void) {
//	mat4x4 mat = mat4x4_null();
//	mat.v[5 * 0 + 0] = 1.0f;
//	mat.v[4 * 1 + 2] = 1.0f;
//	mat.v[4 * 2 + 1] = -1.0f;
//	mat.v[4 * 3 + 3] = 1.0f;
//	return mat;
//}
