#pragma once
#include "pack.h"
#include "safety.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <float.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

static inline bool f_aprox_eq(
	const float_t v1, 
	const float_t v2) 
{
	return fabs(((double)v1) - ((double)v2)) < DBL_EPSILON;
}

static inline float_t to_radians(const float_t degrees) {
	return degrees * (float_t)M_PI / (float_t)180.0;
}

static inline float_t to_degrees(const float_t radians) {
	return radians * (float_t)180.0 / (float_t)M_PI;
}

#define IMPLEMENT_VEC_STRUCT(struct_name, name, type, size, zero, one)\
PACK(struct struct_name {\
	type vs[size];\
});\
typedef struct struct_name struct_name;\
static inline type* name##_at(struct_name* const vec, const size_t index) {\
	TRY(index < size);\
	return &vec->vs[index];\
}\
static inline type name##_get(const struct_name vec, const size_t index) {\
	TRY(index < size);\
	return vec.vs[index];\
}\
static inline struct_name name##_splat(const type value) {\
	struct_name ret;\
	for (size_t i = 0; i < size; i++) {\
		ret.vs[i] = value;\
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
		ret.vs[i] = vec1.vs[i] + vec2.vs[i];\
	}\
	return ret;\
}\
static inline struct_name name##_vecsub(const struct_name vec1, const struct_name vec2) {\
	struct_name ret;\
	for (size_t i = 0; i < size; i++) {\
		ret.vs[i] = vec1.vs[i] - vec2.vs[i];\
	}\
	return ret;\
}\
static inline struct_name name##_neg(const struct_name vec) {\
	return name##_vecsub(name##_zero(), vec);\
}\
static inline struct_name name##_vecmul(const struct_name vec1, const struct_name vec2) {\
	struct_name ret;\
	for (size_t i = 0; i < size; i++) {\
		ret.vs[i] = vec1.vs[i] * vec2.vs[i];\
	}\
	return ret;\
}\
static inline struct_name name##_inv(const struct_name vec) {\
	struct_name ret;\
	for (size_t i = 0; i < size; i++) {\
		ret.vs[i] = one / vec.vs[i];\
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
}\
static inline bool name##_exact_veceq(const struct_name vec1, const struct_name vec2) {\
	bool ret = true;\
	for (size_t i = 0; ret && i < size; i++) {\
		ret = ret & (vec1.vs[i] == vec2.vs[i]);\
	}\
	return ret;\
}\
static inline bool name##_exact_vecgt(const struct_name vec1, const struct_name vec2) {\
	bool ret = true;\
	for (size_t i = 0; ret && i < size; i++) {\
		ret = ret & (vec1.vs[i] > vec2.vs[i]);\
	}\
	return ret;\
}\
static inline bool name##_exact_veclt(const struct_name vec1, const struct_name vec2) {\
	bool ret = true;\
	for (size_t i = 0; ret && i < size; i++) {\
		ret = ret & (vec1.vs[i] < vec2.vs[i]);\
	}\
	return ret;\
}\
static inline bool name##_exact_eq(const struct_name vec, const type value) {\
	return name##_exact_veceq(vec, name##_splat(value));\
}\
static inline bool name##_exact_gt(const struct_name vec, const type value) {\
	return name##_exact_vecgt(vec, name##_splat(value));\
}\
static inline bool name##_exact_lt(const struct_name vec, const type value) {\
	return name##_exact_veclt(vec, name##_splat(value));\
}

#define IMPLEMENT_FLOAT_VEC_EXTENSION(struct_name, name, type, size, zero, sqrt_fn) \
static inline bool name##_aprox_eq(const struct_name vec1, const struct_name vec2) {\
	bool ret = true;\
	for (size_t i = 0; ret && i < size; i++) {\
		ret = ret & f_aprox_eq(vec1.vs[i], vec2.vs[i]);\
	}\
	return ret;\
}\
static inline type name##_element_sum(const struct_name vec) {\
	type ret = zero;\
	for (size_t i = 0; i < size; i++) {\
		ret += vec.vs[i];\
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
	return sqrt_fn(name##_sqrlen(vec));\
}\
static inline struct_name name##_normalize(const struct_name vec) {\
	return name##_div(vec, name##_len(vec));\
}

#define IMPLEMENT_INT_VEC_EXTENSION(struct_name, name, type, size)\
static inline struct_name name##_rem(const struct_name vec, const type value) {\
	struct_name ret;\
	for (size_t i = 0; i < size; i++) {\
		ret.vs[i] = vec.vs[i] % value;\
	}\
	return ret;\
}

#define IMPLEMENT_VEC_CONVERSION(struct_name1, struct_name2, name1, name2, type1, type2, size)\
static inline struct_name1 name1##_from_##name2(const struct_name2 vec) {\
	struct_name1 ret;\
	for (size_t i = 0; i < size; i++) {\
		ret.vs[i] = (type1)vec.vs[i];\
	}\
	return ret;\
}\
static inline struct_name2 name2##_from_##name1(const struct_name1 vec) {\
	struct_name2 ret;\
	for (size_t i = 0; i < size; i++) {\
		ret.vs[i] = (type2)vec.vs[i];\
	}\
	return ret;\
}

/*#define VEC_OUTER_FOR(struct_name, iter_name, begin, end, expr)\
{\
	const struct_name __for_##iter_name##_begin = begin;\
	const struct_name __for_##iter_name##_end = end;\
	struct_name iter_name;
	expr\
}

#define VEC_INNER_FOR(iter_name, index)\
for (iter_name##.vs[index] = __for_##iter_name##_begin##.vs[index];\
	iter_name##.vs[index] < __for_##iter_name##_end##.vs[index];\
	iter_name##.vs[index]++)

// Fuck custom for loops, this is way too much voodoo.
VEC_OUTER_FOR(Vec3i32, local_position, vec3i32_zero(), vec3i32_splat(CHUNK_SIDELEN),
VEC_INNER_FOR(local_position, 0) {
	const int32_t x = chunk_position.vs[0] + local_position.vs[0];
	VEC_INNER_FOR(local_position, 2) {
		const int32_t z = chunk_position.vs[2] + local_position.vs[2];
		const int32_t height = (int32_t)fbm2(
			&settings->perlin_seed,
			&settings->heightmap_fbm,
			(float)x,
			(float)z);
		VEC_INNER_FOR(local_position, 1) {
			const int32_t y = chunk_y + dy;
			BlockId* const block = chunk_block_at(chunk, local_position);
			*block = y > height ? BLOCK_AIR : BLOCK_STONE;
		}
	}
}
)*/

#define IMPLEMENT_FLOAT_VEC_STRUCT(struct_name, name, type, size, zero, one, sqrt_fn)\
IMPLEMENT_VEC_STRUCT(struct_name, name, type, size, zero, one)\
IMPLEMENT_FLOAT_VEC_EXTENSION(struct_name, name, type, size, zero, sqrt_fn)

#define IMPLEMENT_INT_VEC_STRUCT(struct_name, name, type, size, zero, one)\
IMPLEMENT_VEC_STRUCT(struct_name, name, type, size, zero, one)\
IMPLEMENT_INT_VEC_EXTENSION(struct_name, name, type, size)


IMPLEMENT_FLOAT_VEC_STRUCT(Vec2f, vec2f, float_t, 2, 0.0f, 1.0f, sqrtf);
IMPLEMENT_FLOAT_VEC_STRUCT(Vec3f, vec3f, float_t, 3, 0.0f, 1.0f, sqrtf);
IMPLEMENT_FLOAT_VEC_STRUCT(Vec4f, vec4f, float_t, 4, 0.0f, 1.0f, sqrtf);

IMPLEMENT_INT_VEC_STRUCT(Vec2s, vec2s, size_t, 2, 0, 1);
IMPLEMENT_INT_VEC_STRUCT(Vec3s, vec3s, size_t, 3, 0, 1);

IMPLEMENT_INT_VEC_STRUCT(Vec2i32, vec2i32, int32_t, 2, 0, 1);
IMPLEMENT_INT_VEC_STRUCT(Vec3i32, vec3i32, int32_t, 3, 0, 1);

IMPLEMENT_FLOAT_VEC_STRUCT(Mat4x4f, mat4x4f, float_t, 16, 0, 1, sqrtf);

IMPLEMENT_VEC_CONVERSION(Vec2f, Vec2s, vec2f, vec2s, float_t, size_t, 2);
IMPLEMENT_VEC_CONVERSION(Vec3f, Vec3s, vec3f, vec3s, float_t, size_t, 3);

IMPLEMENT_VEC_CONVERSION(Vec2f, Vec2i32, vec2f, vec2i32, float_t, int32_t, 2);
IMPLEMENT_VEC_CONVERSION(Vec3f, Vec3i32, vec3f, vec3i32, float_t, int32_t, 3);

IMPLEMENT_VEC_CONVERSION(Vec2s, Vec2i32, vec2s, vec2i32, size_t, int32_t, 2);
IMPLEMENT_VEC_CONVERSION(Vec3s, Vec3i32, vec3s, vec3i32, size_t, int32_t, 3);


static inline Vec3f vec3f_new(
	const float_t x, 
	const float_t y, 
	const float_t z)
{
	Vec3f vec;
	vec.vs[0] = x;
	vec.vs[1] = y;
	vec.vs[2] = z;
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

static inline Vec3f vec3f_cross(const Vec3f a, const Vec3f b) {
	return vec3f_new(
		a.vs[1] * b.vs[2] - a.vs[2] * b.vs[1],
		a.vs[2] * b.vs[0] - a.vs[0] * b.vs[2],
		a.vs[0] * b.vs[1] - a.vs[1] * b.vs[0]);
}

static inline Vec4f vec4f_new(
	const float_t x, 
	const float_t y, 
	const float_t z, 
	const float_t w)
{
	Vec4f vec;
	vec.vs[0] = x;
	vec.vs[1] = y;
	vec.vs[2] = z;
	vec.vs[3] = w;
	return vec;
}

static inline Vec4f vec4f_plane(const Vec3f point, const Vec3f normal) {
	const Vec3f normalized = vec3f_normalize(normal);
	return vec4f_new(
		normalized.vs[0],
		normalized.vs[1],
		normalized.vs[2],
		-vec3f_dot(point, normalized)
	);
}

static inline Vec3f vec4f_plane_intersect_line(
	const Vec4f plane, 
	const Vec3f point, 
	const Vec3f dir) 
{
	return vec3f_vecadd(
		point,
		vec3f_mul(
			vec3f_neg(dir),
			vec4f_dot(
				vec4f_new(
					point.vs[0],
					point.vs[1],
					point.vs[2],
					1.0f),
				plane) / vec3f_dot(
					vec3f_new(
						plane.vs[0],
						plane.vs[1],
						plane.vs[2]),
					dir)));
}

static inline float_t mat4x4f_get_element(
	const Mat4x4f mat, 
	const size_t row, 
	const size_t col) 
{
	TRY(row < 4);
	TRY(col < 4);
	return mat.vs[row * 4 + col];
}

static inline Vec4f mat4x4f_get_row(
	const Mat4x4f mat, 
	const size_t index) 
{
	TRY(index < 4);
	Vec4f ret;

	ret.vs[0] = mat.vs[index * 4];
	ret.vs[1] = mat.vs[index * 4 + 1];
	ret.vs[2] = mat.vs[index * 4 + 2];
	ret.vs[3] = mat.vs[index * 4 + 3];

	return ret;
}

static inline Vec4f mat4x4f_get_col(
	const Mat4x4f mat, 
	const size_t index) 
{
	TRY(index < 4);
	Vec4f ret;

	ret.vs[0] = mat.vs[index];
	ret.vs[1] = mat.vs[index + 4];
	ret.vs[2] = mat.vs[index + 8];
	ret.vs[3] = mat.vs[index + 12];

	return ret;
}

static inline Mat4x4f mat4x4f_matmul(const Mat4x4f mat1, const Mat4x4f mat2) {
	Mat4x4f mat;

	// Actually it should be this...
	/*mat.vs[0] = vec4f_dot(mat4x4f_get_row(mat1, 0), mat4x4f_get_col(mat2, 0));
	mat.vs[1] = vec4f_dot(mat4x4f_get_row(mat1, 0), mat4x4f_get_col(mat2, 0));
	mat.vs[2] = vec4f_dot(mat4x4f_get_row(mat1, 0), mat4x4f_get_col(mat2, 0));
	mat.vs[3] = vec4f_dot(mat4x4f_get_row(mat1, 0), mat4x4f_get_col(mat2, 0));

	mat.vs[4] = vec4f_dot(mat4x4f_get_row(mat1, 1), mat4x4f_get_col(mat2, 1));
	mat.vs[5] = vec4f_dot(mat4x4f_get_row(mat1, 1), mat4x4f_get_col(mat2, 1));
	mat.vs[6] = vec4f_dot(mat4x4f_get_row(mat1, 1), mat4x4f_get_col(mat2, 1));
	mat.vs[7] = vec4f_dot(mat4x4f_get_row(mat1, 1), mat4x4f_get_col(mat2, 1));

	mat.vs[8] = vec4f_dot(mat4x4f_get_row(mat1, 2), mat4x4f_get_col(mat2, 2));
	mat.vs[9] = vec4f_dot(mat4x4f_get_row(mat1, 2), mat4x4f_get_col(mat2, 2));
	mat.vs[10] = vec4f_dot(mat4x4f_get_row(mat1, 2), mat4x4f_get_col(mat2, 2));
	mat.vs[11] = vec4f_dot(mat4x4f_get_row(mat1, 2), mat4x4f_get_col(mat2, 2));

	mat.vs[12] = vec4f_dot(mat4x4f_get_row(mat1, 3), mat4x4f_get_col(mat2, 3));
	mat.vs[13] = vec4f_dot(mat4x4f_get_row(mat1, 3), mat4x4f_get_col(mat2, 3));
	mat.vs[14] = vec4f_dot(mat4x4f_get_row(mat1, 3), mat4x4f_get_col(mat2, 3));
	mat.vs[15] = vec4f_dot(mat4x4f_get_row(mat1, 3), mat4x4f_get_col(mat2, 3));*/

	mat.vs[0] = vec4f_dot(mat4x4f_get_row(mat1, 0), mat4x4f_get_row(mat2, 0));
	mat.vs[1] = vec4f_dot(mat4x4f_get_row(mat1, 0), mat4x4f_get_row(mat2, 1));
	mat.vs[2] = vec4f_dot(mat4x4f_get_row(mat1, 0), mat4x4f_get_row(mat2, 2));
	mat.vs[3] = vec4f_dot(mat4x4f_get_row(mat1, 0), mat4x4f_get_row(mat2, 3));

	mat.vs[4] = vec4f_dot(mat4x4f_get_row(mat1, 1), mat4x4f_get_row(mat2, 0));
	mat.vs[5] = vec4f_dot(mat4x4f_get_row(mat1, 1), mat4x4f_get_row(mat2, 1));
	mat.vs[6] = vec4f_dot(mat4x4f_get_row(mat1, 1), mat4x4f_get_row(mat2, 2));
	mat.vs[7] = vec4f_dot(mat4x4f_get_row(mat1, 1), mat4x4f_get_row(mat2, 3));

	mat.vs[8] = vec4f_dot(mat4x4f_get_row(mat1, 2), mat4x4f_get_row(mat2, 0));
	mat.vs[9] = vec4f_dot(mat4x4f_get_row(mat1, 2), mat4x4f_get_row(mat2, 1));
	mat.vs[10] = vec4f_dot(mat4x4f_get_row(mat1, 2), mat4x4f_get_row(mat2, 2));
	mat.vs[11] = vec4f_dot(mat4x4f_get_row(mat1, 2), mat4x4f_get_row(mat2, 3));

	mat.vs[12] = vec4f_dot(mat4x4f_get_row(mat1, 3), mat4x4f_get_row(mat2, 0));
	mat.vs[13] = vec4f_dot(mat4x4f_get_row(mat1, 3), mat4x4f_get_row(mat2, 1));
	mat.vs[14] = vec4f_dot(mat4x4f_get_row(mat1, 3), mat4x4f_get_row(mat2, 2));
	mat.vs[15] = vec4f_dot(mat4x4f_get_row(mat1, 3), mat4x4f_get_row(mat2, 3));

	return mat;
}

static inline Mat4x4f mat4x4f_diagonal_mat(const Vec4f value) {
	Mat4x4f mat;

	mat.vs[0] = value.vs[0];
	mat.vs[1] = 0.0f;
	mat.vs[2] = 0.0f;
	mat.vs[3] = 0.0f;

	mat.vs[4] = 0.0f;
	mat.vs[5] = value.vs[1];
	mat.vs[6] = 0.0f;
	mat.vs[7] = 0.0f;

	mat.vs[8] = 0.0f;
	mat.vs[9] = 0.0f;
	mat.vs[10] = value.vs[2];
	mat.vs[11] = 0.0f;

	mat.vs[12] = 0.0f;
	mat.vs[13] = 0.0f;
	mat.vs[14] = 0.0f;
	mat.vs[15] = value.vs[3];

	return mat;
}

static inline Mat4x4f mat4x4f_scalar_mat(const float_t value) {
	return mat4x4f_diagonal_mat(vec4f_new(value, value, value, value));
}

static inline Mat4x4f mat4x4f_identity_mat(void) {
	return mat4x4f_scalar_mat(1.0f);
}

static inline Mat4x4f mat4x4f_null_mat(void) {
	return mat4x4f_scalar_mat(0.0f);
}

static inline Mat4x4f mat4x4f_translate_mat(const Vec3f translate) {
	Mat4x4f mat;

	mat.vs[0] = 1.0f;
	mat.vs[1] = 0.0f;
	mat.vs[2] = 0.0f;
	mat.vs[3] = translate.vs[0];

	mat.vs[4] = 0.0f;
	mat.vs[5] = 1.0f;
	mat.vs[6] = 0.0f;
	mat.vs[7] = translate.vs[1];

	mat.vs[8] = 0.0f;
	mat.vs[9] = 0.0f;
	mat.vs[10] = 1.0f;
	mat.vs[11] = translate.vs[2];

	mat.vs[12] = 0.0f;
	mat.vs[13] = 0.0f;
	mat.vs[14] = 0.0f;
	mat.vs[15] = 1.0f;

	return mat;
}

static inline Mat4x4f mat4x4f_scale_mat(const Vec3f scale) {
	Mat4x4f mat;

	mat.vs[0] = scale.vs[0];
	mat.vs[1] = 0.0f;
	mat.vs[2] = 0.0f;
	mat.vs[3] = 0.0f;

	mat.vs[4] = 0.0f;
	mat.vs[5] = scale.vs[1];
	mat.vs[6] = 0.0f;
	mat.vs[7] = 0.0f;

	mat.vs[8] = 0.0f;
	mat.vs[9] = 0.0f;
	mat.vs[10] = scale.vs[2];
	mat.vs[11] = 0.0f;

	mat.vs[12] = 0.0f;
	mat.vs[13] = 0.0f;
	mat.vs[14] = 0.0f;
	mat.vs[15] = 1.0f;

	return mat;
}

static inline Mat4x4f mat4x4f_rotate_axis_x_mat(const float_t radians) {
	Mat4x4f mat;
	const float_t c = cosf(radians);
	const float_t s = sinf(radians);

	mat.vs[0] = 1.0f;
	mat.vs[1] = 0.0f;
	mat.vs[2] = 0.0f;
	mat.vs[3] = 0.0f;

	mat.vs[4] = 0.0f;
	mat.vs[5] = c;
	mat.vs[6] = s;
	mat.vs[7] = 0.0f;

	mat.vs[8] = 0.0f;
	mat.vs[9] = -s;
	mat.vs[10] = c;
	mat.vs[11] = 0.0f;

	mat.vs[12] = 0.0f;
	mat.vs[13] = 0.0f;
	mat.vs[14] = 0.0f;
	mat.vs[15] = 1.0f;

	return mat;
}

static inline Mat4x4f mat4x4f_rotate_axis_y(const float_t radians) {
	Mat4x4f mat;
	const float_t c = cosf(radians);
	const float_t s = sinf(radians);

	mat.vs[0] = c;
	mat.vs[1] = 0.0f;
	mat.vs[2] = -s;
	mat.vs[3] = 0.0f;

	mat.vs[4] = 0.0f;
	mat.vs[5] = 1.0f;
	mat.vs[6] = 0.0f;
	mat.vs[7] = 0.0f;

	mat.vs[8] = s;
	mat.vs[9] = 0.0f;
	mat.vs[10] = c;
	mat.vs[11] = 0.0f;

	mat.vs[12] = 0.0f;
	mat.vs[13] = 0.0f;
	mat.vs[14] = 0.0f;
	mat.vs[15] = 1.0f;

	return mat;
}

static inline Mat4x4f mat4x4f_rotate_axis_z(const float_t radians) {
	Mat4x4f mat;
	const float_t c = cosf(radians);
	const float_t s = sinf(radians);

	mat.vs[0] = c;
	mat.vs[1] = -s;
	mat.vs[2] = 0.0f;
	mat.vs[3] = 0.0f;

	mat.vs[4] = s;
	mat.vs[5] = c;
	mat.vs[6] = 0.0f;
	mat.vs[7] = 0.0f;

	mat.vs[8] = 0.0f;
	mat.vs[9] = 0.0f;
	mat.vs[10] = 1.0f;
	mat.vs[11] = 0.0f;

	mat.vs[12] = 0.0f;
	mat.vs[13] = 0.0f;
	mat.vs[14] = 0.0f;
	mat.vs[15] = 1.0f;

	return mat;
}

static inline Mat4x4f mat4x4f_ortho_mat(
	const float_t left,
	const float_t right,
	const float_t bottom,
	const float_t top,
	const float_t near,
	const float_t far) 
{
	Mat4x4f mat;

	mat.vs[0] = 2.0f / (left - right);
	mat.vs[1] = 0.0f;
	mat.vs[2] = 0.0f;
	mat.vs[3] = -(left + right) / (left - right);

	mat.vs[4] = 0.0f;
	mat.vs[5] = 2.0f / (bottom - top);
	mat.vs[6] = 0.0f;
	mat.vs[7] = -(bottom + top) / (bottom - top);

	mat.vs[8] = 0.0f;
	mat.vs[9] = 0.0f;
	mat.vs[10] = -2.0f / (far - near);
	mat.vs[11] = -(far + near) / (far - near);

	mat.vs[12] = 0.0f;
	mat.vs[13] = 0.0f;
	mat.vs[14] = 0.0f;
	mat.vs[15] = 1.0f;

	return mat;
}

static inline Mat4x4f mat4x4f_perspective_mat(
	const float_t aspect,
	const float_t fov_y,
	const float_t near,
	const float_t far) 
{
	Mat4x4f mat;
	const float_t f = 1.0f / tanf(fov_y / 2.0f);

	mat.vs[0] = f / aspect;
	mat.vs[1] = 0.0f;
	mat.vs[2] = 0.0f;
	mat.vs[3] = 0.0f;

	mat.vs[4] = 0.0f;
	mat.vs[5] = f;
	mat.vs[6] = 0.0f;
	mat.vs[7] = 0.0f;

	mat.vs[8] = 0.0f;
	mat.vs[9] = 0.0f;
	mat.vs[10] = (far + near) / (near - far);
	mat.vs[11] = (2.0f * far * near) / (near - far);

	mat.vs[12] = 0.0f;
	mat.vs[13] = 0.0f;
	mat.vs[14] = -1.0f;
	mat.vs[15] = 0.0f;

	return mat;
}

static inline Mat4x4f mat4x4f_look_at_mat(
	const Vec3f from,
	const Vec3f to,
	const Vec3f up)
{
	Mat4x4f mat;
	const Vec3f axis_z = vec3f_normalize(vec3f_vecsub(to, from));
	const Vec3f axis_x = vec3f_normalize(vec3f_cross(up, axis_z));
	const Vec3f axis_y = vec3f_cross(axis_z, axis_x);

	mat.vs[0] = axis_x.vs[0];
	mat.vs[1] = axis_x.vs[1];
	mat.vs[2] = axis_x.vs[2];
	mat.vs[3] = vec3f_dot(axis_x, from);

	mat.vs[4] = axis_y.vs[0];
	mat.vs[5] = axis_y.vs[1];
	mat.vs[6] = axis_y.vs[2];
	mat.vs[7] = vec3f_dot(axis_y, from);

	mat.vs[8] = axis_z.vs[0];
	mat.vs[9] = axis_z.vs[1];
	mat.vs[10] = axis_z.vs[2];
	mat.vs[11] = vec3f_dot(axis_z, from);

	mat.vs[12] = 0.0f;
	mat.vs[13] = 0.0f;
	mat.vs[14] = 0.0f;
	mat.vs[15] = 1.0f;

	return mat;
}
