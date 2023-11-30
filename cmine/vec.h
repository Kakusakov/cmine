#pragma once
#include "fixed_types.h"
#define _USE_MATH_DEFINES
#include <math.h>

typedef struct vec2f vec2f;
struct vec2f {
	float32_t x;
	float32_t y;
};

typedef struct vec3f vec3f;
struct vec3f {
	float32_t x;
	float32_t y;
	float32_t z;
};

typedef struct vec2i vec2i;
struct vec2i {
	int32_t x;
	int32_t y;
};

typedef struct vec3i vec3i;
struct vec3i {
	int32_t x;
	int32_t y;
	int32_t z;
};

typedef struct vec2size vec2size;
struct vec2size {
	size_t x;
	size_t y;
};

typedef struct vec3size vec3size;
struct vec3size {
	size_t x;
	size_t y;
	size_t z;
};

static inline vec3f vec3f_right(void) {
	return (vec3f) { 1.0f, 0.0f, 0.0f };
}
static inline vec3f vec3f_up(void) {
	return (vec3f) { 0.0f, 1.0f, 0.0f };
}
static inline vec3f vec3f_forward(void) {
	return (vec3f) { 0.0f, 0.0f, 1.0f };
}
static inline vec3f vec3f_left(void) {
	vec3f vec = vec3f_right();
	return (vec3f) { -vec.x, -vec.y, -vec.z };
}
static inline vec3f vec3f_down(void) {
	vec3f vec = vec3f_up();
	return (vec3f) { -vec.x, -vec.y, -vec.z };
}
static inline vec3f vec3f_backward(void) {
	vec3f vec = vec3f_forward();
	return (vec3f) { -vec.x, -vec.y, -vec.z };
}

static inline float32_t vec3f_len(vec3f vec) {
	return sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
}

static inline vec3f vec3f_normalize(vec3f vec) {
	float32_t len = vec3f_len(vec);
	return (vec3f) { vec.x / len, vec.y / len, vec.z / len };
}

static inline float32_t vec3f_dot(vec3f a, vec3f b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

static inline vec3f vec3f_cross(vec3f a, vec3f b) {
	return (vec3f) {
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x,
	};
}

static inline float32_t to_radians(const float32_t degrees) {
	return degrees * (float32_t)M_PI / (float32_t)180.0;
}

static inline float32_t to_degrees(const float32_t radians) {
	return radians * (float32_t)180.0 / (float32_t)M_PI;
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
