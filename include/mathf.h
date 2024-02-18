#pragma once
#include <math.h>

static inline int mod(int a, int b) {
	int result = a % b;
	return result >= 0 ? result : result + b;
}

#define PI 3.14159265358979323846
#define DEG_TO_RAD (PI / 180.0)
#define RAD_TO_DEG (180.0 / PI)

typedef struct Vec2f Vec2f;
struct Vec2f
{
	float x;
	float y;
};

typedef struct Vec3f Vec3f;
struct Vec3f
{
	float x;
	float y;
	float z;
};

typedef struct Vec2f Vec2f;
struct Vec2i
{
	int x;
	int y;
};

typedef struct Vec3i Vec3i;
struct Vec3i
{
	int x;
	int y;
	int z;
};

static inline float vec3f_dot(Vec3f a, Vec3f b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

static inline Vec3f vec3f_cross(Vec3f a, Vec3f b)
{
	return (Vec3f){
		.x = a.y * b.z - a.z * b.y,
		.y = a.z * b.x - a.x * b.z,
		.z = a.x * b.y - a.y * b.x,
	};
}

static inline float vec3f_sqrlen(Vec3f a)
{
	return vec3f_dot(a, a);
}

static inline float vec3f_len(Vec3f a)
{
	return sqrtf(vec3f_sqrlen(a));
}

static inline Vec3f vec3f_normalize(Vec3f a)
{
	float len = vec3f_len(a);
	if (len == 0) return (Vec3f){0};
	return (Vec3f){
		.x = a.x / len,
		.y = a.y / len,
		.z = a.z / len,
	};
}

typedef struct Mat Mat;
struct Mat {
	float v[4 * 4];
};

#define MAT_IDX(x, y) (y * 4 + x)
#define MAT_IDX_V(v) MAT_IDX(v.x, v.y)

static inline Mat matmul(Mat rows, Mat cols) {
	Mat mat;
	for (int y = 0; y < 4; y++) {
		for (int x = 0; x < 4; x++) {
			mat.v[MAT_IDX(x, y)] =
				rows.v[MAT_IDX(0, y)] * cols.v[MAT_IDX(x, 0)] +
				rows.v[MAT_IDX(1, y)] * cols.v[MAT_IDX(x, 1)] +
				rows.v[MAT_IDX(2, y)] * cols.v[MAT_IDX(x, 2)] +
				rows.v[MAT_IDX(3, y)] * cols.v[MAT_IDX(x, 3)];
		}
	}
	return mat;
}

#define MAT_IDENTITY ((Mat){  \
	1.0f, 0.0f, 0.0f, 0.0f,   \
	0.0f, 1.0f, 0.0f, 0.0f,   \
	0.0f, 0.0f, 1.0f, 0.0f,   \
	0.0f, 0.0f, 0.0f, 1.0f,   \
})

static inline Vec3f vec3f_opengl_style(Vec3f a)
{
	return (Vec3f) {
		.x = a.y,
		.y = a.z,
		.z = a.x,
	};
}

static inline Mat mat_translate(Mat mat, Vec3f translate) {
	translate = vec3f_opengl_style(translate);
	Mat m = MAT_IDENTITY;
	m.v[MAT_IDX(0, 3)] = translate.x;
	m.v[MAT_IDX(1, 3)] = translate.y;
	m.v[MAT_IDX(2, 3)] = translate.z;
	return matmul(mat, m);
}

static inline Mat mat_scale(Mat mat, Vec3f scale) {
	scale = vec3f_opengl_style(scale);
	Mat m = MAT_IDENTITY;
	m.v[MAT_IDX(1, 1)] = scale.x;
	m.v[MAT_IDX(2, 2)] = scale.y;
	m.v[MAT_IDX(3, 3)] = scale.z;
	return matmul(mat, m);
}

static inline Mat mat_ortho(
	Mat mat,
	float left,   float right,
	float bottom, float top, 
	float near,   float far)
{
	Mat m = {0};
	m.v[MAT_IDX(0, 0)] = 2.0f / (left - right);
	m.v[MAT_IDX(1, 1)] = 2.0f / (bottom - top);
	m.v[MAT_IDX(2, 2)] = -2.0f / (far - near);

	m.v[MAT_IDX(0, 3)] = -(left + right) / (left - right);
	m.v[MAT_IDX(1, 3)] = -(bottom + top) / (bottom - top);
	m.v[MAT_IDX(2, 3)] = -(far + near) / (far - near);
	m.v[MAT_IDX(3, 3)] = 1.0f;
	return matmul(mat, m);
}

static inline Mat mat_perspective(
	Mat mat,
	float aspect, float fov_z,
	float near,   float far)
{
	Mat m = {0};
	float f = 1.0f / tanf(fov_z / 2.0f);
	
	m.v[MAT_IDX(0, 0)] = f / aspect;
	m.v[MAT_IDX(1, 1)] = f;
	m.v[MAT_IDX(2, 2)] = (far + near) / (near - far);

	m.v[MAT_IDX(3, 2)] = -1.0f;
	m.v[MAT_IDX(2, 3)] = (2.0f * far * near) / (near - far);
	return matmul(mat, m);
}

static inline Mat mat_look_at(
	Mat mat,
	Vec3f from,
	Vec3f to,
	Vec3f up)
{
	from = vec3f_opengl_style(from);
	to = vec3f_opengl_style(to);
	up = vec3f_opengl_style(up);
	Mat m = {0};
	Vec3f delta = {
		to.x - from.x,
		to.y - from.y,
		to.z - from.z,
	};
	Vec3f axis_z = vec3f_normalize(delta);
	Vec3f axis_x = vec3f_normalize(vec3f_cross(up, axis_z));
	Vec3f axis_y = vec3f_cross(axis_z, axis_x);

	m.v[MAT_IDX(0, 0)] = axis_x.x;
	m.v[MAT_IDX(1, 0)] = axis_y.x;
	m.v[MAT_IDX(2, 0)] = axis_z.x;

	m.v[MAT_IDX(0, 1)] = axis_x.y;
	m.v[MAT_IDX(1, 1)] = axis_y.y;
	m.v[MAT_IDX(2, 1)] = axis_z.y;

	m.v[MAT_IDX(0, 2)] = axis_x.z;
	m.v[MAT_IDX(1, 2)] = axis_y.z;
	m.v[MAT_IDX(2, 2)] = axis_z.z;

	m.v[MAT_IDX(0, 3)] = vec3f_dot(axis_x, from);
	m.v[MAT_IDX(1, 3)] = vec3f_dot(axis_y, from);
	m.v[MAT_IDX(2, 3)] = vec3f_dot(axis_z, from);
	m.v[MAT_IDX(3, 3)] = 1.0f;
	return matmul(mat, m);
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
