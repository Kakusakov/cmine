#pragma once
#include "vec.h"

typedef struct mat4x4 mat4x4;
struct mat4x4 {
	float32_t v[4 * 4];
};

static inline mat4x4 mat4x4_matmul(const mat4x4 rows, const mat4x4 cols) {
	mat4x4 mat;
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

//static inline mat4x4 mat4x4_z_up_to_y_up(void) {
//	mat4x4 mat = mat4x4_null();
//	mat.v[5 * 0 + 0] = 1.0f;
//	mat.v[4 * 1 + 2] = 1.0f;
//	mat.v[4 * 2 + 1] = -1.0f;
//	mat.v[4 * 3 + 3] = 1.0f;
//	return mat;
//}

static inline mat4x4 mat4x4_null(void) {
	return (mat4x4) { 0, 0, 0 };
}

static inline mat4x4 mat4x4_identity(void) {
	mat4x4 mat = mat4x4_null();
	for (size_t i = 0; i < 4; i++) {
		mat.v[i * 4 + i] = 1.0f;
	}
	return mat;
}

static inline mat4x4 mat4x4_translate(vec3f translate) {
	mat4x4 mat = mat4x4_identity();
	mat.v[12] = translate.x;
	mat.v[13] = translate.y;
	mat.v[14] = translate.z;
	return mat;
}

static inline mat4x4 mat4x4_scale(vec3f scale) {
	mat4x4 mat = mat4x4_identity();
	mat.v[4 * 0 + 0] = scale.x;
	mat.v[4 * 1 + 1] = scale.y;
	mat.v[4 * 2 + 2] = scale.z;
	return mat;
}

static inline mat4x4 mat4x4_ortho(
	const float32_t left,
	const float32_t right,
	const float32_t bottom,
	const float32_t top,
	const float32_t near,
	const float32_t far)
{
	mat4x4 mat = mat4x4_null();

	mat.v[0 * 4 + 0] = 2.0f / (left - right);
	mat.v[1 * 4 + 1] = 2.0f / (bottom - top);
	mat.v[2 * 4 + 2] = -2.0f / (far - near);

	mat.v[3 * 4 + 0] = -(left + right) / (left - right);
	mat.v[3 * 4 + 1] = -(bottom + top) / (bottom - top);
	mat.v[3 * 4 + 2] = -(far + near) / (far - near);
	mat.v[3 * 4 + 3] = 1.0f;

	return mat;
}

static inline mat4x4 mat4x4_perspective(
	const float32_t aspect,
	const float32_t fov_y,
	const float32_t near,
	const float32_t far)
{
	mat4x4 mat = mat4x4_null();
	float32_t f = 1.0f / tanf(fov_y / 2.0f);

	mat.v[4 * 0 + 0] = f / aspect;
	mat.v[4 * 1 + 1] = f;
	mat.v[4 * 2 + 2] = (far + near) / (near - far);
	mat.v[4 * 2 + 3] = -1.0f;
	mat.v[4 * 3 + 2] = (2.0f * far * near) / (near - far);

	return mat;
}

static inline mat4x4 mat4x4_look_at(
	vec3f from,
	vec3f to,
	vec3f up)
{
	mat4x4 mat = mat4x4_null();
	vec3f delta = {
		to.x - from.x,
		to.y - from.y,
		to.z - from.z,
	};
	vec3f axis_z = vec3f_normalize(delta);
	vec3f axis_x = vec3f_normalize(vec3f_cross(up, axis_z));
	vec3f axis_y = vec3f_cross(axis_z, axis_x);

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

