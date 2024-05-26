#pragma once
#include "pack.h"
#include <math.h>

#define PI 3.14159265358979323846
#define DEG_TO_RAD (PI / 180.0)
#define RAD_TO_DEG (180.0 / PI)

typedef struct Vec3 Vec3;
PACK(struct Vec3
{
	float x;
	float y;
	float z;
});

static inline Vec3 vsplat(float value)
{
	return (Vec3) {
		.x = value,
		.y = value,
		.z = value,
	};
}

static inline Vec3 vadd(Vec3 a, Vec3 b)
{
	return (Vec3) {
		.x = a.x + b.x,
		.y = a.y + b.y,
		.z = a.z + b.z,
	};
}

static inline Vec3 vsub(Vec3 a, Vec3 b)
{
	return (Vec3) {
		.x = a.x - b.x,
		.y = a.y - b.y,
		.z = a.z - b.z,
	};
}

static inline Vec3 vmul(Vec3 a, Vec3 b)
{
	return (Vec3) {
		.x = a.x * b.x,
		.y = a.y * b.y,
		.z = a.z * b.z,
	};
}

static inline Vec3 vdiv(Vec3 a, Vec3 b)
{
	return (Vec3) {
		.x = a.x / b.x,
		.y = a.y / b.y,
		.z = a.z / b.z,
	};
}

static inline float vdot(Vec3 a, Vec3 b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

static inline Vec3 vcross(Vec3 a, Vec3 b)
{
	return (Vec3) {
		.x = a.y * b.z - a.z * b.y,
		.y = a.z * b.x - a.x * b.z,
		.z = a.x * b.y - a.y * b.x,
	};
}

static inline float vsqrlen(Vec3 a)
{
	return vdot(a, a);
}

static inline float vlen(Vec3 a)
{
	return sqrtf(vsqrlen(a));
}

static inline Vec3 vnormalize(Vec3 a)
{
	float len = vlen(a);
	if (len == 0) return (Vec3) { 0 };
	return (Vec3) {
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

#define MAT_IDENTITY ((Mat){  \
	1.0f, 0.0f, 0.0f, 0.0f,   \
	0.0f, 1.0f, 0.0f, 0.0f,   \
	0.0f, 0.0f, 1.0f, 0.0f,   \
	0.0f, 0.0f, 0.0f, 1.0f,   \
})

static inline Mat mat_mul(Mat rows, Mat cols) {
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

static inline Mat mat_translate(Mat mat, Vec3 translate) {
	Mat m = MAT_IDENTITY;
	m.v[MAT_IDX(0, 3)] = translate.x;
	m.v[MAT_IDX(1, 3)] = translate.y;
	m.v[MAT_IDX(2, 3)] = translate.z;
	return mat_mul(mat, m);
}

static inline Mat mat_scale(Mat mat, Vec3 scale) {
	Mat m = MAT_IDENTITY;
	m.v[MAT_IDX(1, 1)] = scale.x;
	m.v[MAT_IDX(2, 2)] = scale.y;
	m.v[MAT_IDX(3, 3)] = scale.z;
	return mat_mul(mat, m);
}

static inline Mat mat_ortho(
	Mat mat,
	float left,   float right,
	float bottom, float top,
	float near,   float far)
{
	Mat m = { 0 };
	m.v[MAT_IDX(0, 0)] = 2.0f / (left - right);
	m.v[MAT_IDX(1, 1)] = 2.0f / (bottom - top);
	m.v[MAT_IDX(2, 2)] = -2.0f / (far - near);

	m.v[MAT_IDX(0, 3)] = -(left + right) / (left - right);
	m.v[MAT_IDX(1, 3)] = -(bottom + top) / (bottom - top);
	m.v[MAT_IDX(2, 3)] = -(far + near) / (far - near);
	m.v[MAT_IDX(3, 3)] = 1.0f;
	return mat_mul(mat, m);
}

static inline Mat mat_perspective(
	Mat mat,
	float aspect, float fovz,
	float near,   float far)
{
	Mat m = { 0 };
	float f = 1.0f / tanf(fovz / 2.0f);

	m.v[MAT_IDX(0, 0)] = f / aspect;
	m.v[MAT_IDX(1, 1)] = f;
	m.v[MAT_IDX(2, 2)] = (far + near) / (near - far);

	m.v[MAT_IDX(3, 2)] = -1.0f;
	m.v[MAT_IDX(2, 3)] = (2.0f * far * near) / (near - far);
	return mat_mul(mat, m);
}

static inline Mat mat_look_at(
	Mat mat,
	Vec3 from,
	Vec3 to,
	Vec3 up)
{
	Mat m = { 0 };
	Vec3 delta = {
		to.x - from.x,
		to.y - from.y,
		to.z - from.z,
	};
	Vec3 axis_z = vnormalize(delta);
	Vec3 axis_x = vnormalize(vcross(up, axis_z));
	Vec3 axis_y = vcross(axis_z, axis_x);

	m.v[MAT_IDX(0, 0)] = axis_x.x;
	m.v[MAT_IDX(1, 0)] = axis_y.x;
	m.v[MAT_IDX(2, 0)] = axis_z.x;

	m.v[MAT_IDX(0, 1)] = axis_x.y;
	m.v[MAT_IDX(1, 1)] = axis_y.y;
	m.v[MAT_IDX(2, 1)] = axis_z.y;

	m.v[MAT_IDX(0, 2)] = axis_x.z;
	m.v[MAT_IDX(1, 2)] = axis_y.z;
	m.v[MAT_IDX(2, 2)] = axis_z.z;

	m.v[MAT_IDX(0, 3)] = vdot(axis_x, from);
	m.v[MAT_IDX(1, 3)] = vdot(axis_y, from);
	m.v[MAT_IDX(2, 3)] = vdot(axis_z, from);
	m.v[MAT_IDX(3, 3)] = 1.0f;
	return mat_mul(mat, m);
}

static inline int mod(int a, int b) {
	int result = a % b;
	return result >= 0 ? result : result + b;
}


//static inline Vec4f vec4f_plane(const Vec3f point, const Vec3f normal) {
//	const Vec3f normalized = vnormalize(normal);
//	return vec4f_new(
//		normalized.vs[0],
//		normalized.vs[1],
//		normalized.vs[2],
//		-vdot(point, normalized)
//	);
//}
//
//static inline Vec3f vec4f_plane_intersect_line(
//	const Vec4f plane,
//	const Vec3f point,
//	const Vec3f dir)
//{
//	return vvecadd(
//		point,
//		vmul(
//			vneg(dir),
//			vec4f_dot(
//				vec4f_new(
//					point.vs[0],
//					point.vs[1],
//					point.vs[2],
//					1.0f),
//				plane) / vdot(
//					vnew(
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
