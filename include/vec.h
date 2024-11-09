#pragma once
#include "types.h"
#include <stddef.h>
#include <stdlib.h>
#include <math.h>

// Macros:

#define V(T, S)\
Vec##S##T

#define VP(T, S, NAME)\
v##S##_##T##_##NAME

#define VUN(T, S)\
Vec##S##T##Union

#define GENERATE_BINOP(OPNAME, RFUNCTION, T, S)\
static inline V(T, S) VP(T, S, OPNAME)(V(T, S) a, V(T, S) b) {\
	VUN(T, S) ua = {a};\
	VUN(T, S) ub = {b};\
	VUN(T, S) ur = {0};\
	for (size_t i = 0; i < S; i++) {\
		ur.as_arr[i] = RFUNCTION(ua.as_arr[i], ub.as_arr[i]);\
	}\
	return ur.as_vec;\
}\

#define GENERATE_UNION(T, S)\
typedef union VUN(T, S) VUN(T, S);\
union VUN(T, S) {\
	V(T, S) as_vec;\
	T as_arr[S];\
};\

#define PRIMITIVE_OPP(T, OPNAME)\
T##_primitive_##OPNAME

#define GENERATE_PRIMITIVE_OPS(T, T0)\
static inline T PRIMITIVE_OPP(T, add)(T a, T b) {\
	return a + b;\
}\
static inline T PRIMITIVE_OPP(T, sub)(T a, T b) {\
	return a - b;\
}\
static inline T PRIMITIVE_OPP(T, mul)(T a, T b) {\
	return a * b;\
}\
static inline T PRIMITIVE_OPP(T, div)(T a, T b) {\
	if (b == T0) abort();\
	return a / b;\
}\

#define GENERATE_PRIMITIVE_VEC(T, S)\
typedef struct V(T, S) V(T, S);\
GENERATE_UNION(T, S)\
GENERATE_BINOP(add, PRIMITIVE_OPP(T, add), T, S)\
GENERATE_BINOP(sub, PRIMITIVE_OPP(T, sub), T, S)\
GENERATE_BINOP(mul, PRIMITIVE_OPP(T, mul), T, S)\
GENERATE_BINOP(div, PRIMITIVE_OPP(T, div), T, S)\

#define GENERATE_PRIMITIVE_INT_OPS(T)\
GENERATE_PRIMITIVE_OPS(T, 0)\
static inline T PRIMITIVE_OPP(T, mod)(T a, T b) {\
	if (b == 0) abort();\
	return (a % b + b) % b;\
}\

#define GENERATE_PRIMITIVE_INT_VEC(T, S)\
GENERATE_PRIMITIVE_VEC(T, S)\
GENERATE_BINOP(mod, PRIMITIVE_OPP(T, mod), T, S)\

// Generation:

GENERATE_PRIMITIVE_OPS(f32, 0.0)

typedef struct V(f32, 4) Vec4;
struct V(f32, 4) {
	f32 x;
	f32 y;
	f32 z;
	f32 w;
};
GENERATE_PRIMITIVE_VEC(f32, 4)
static inline Vec4 v4_new(f32 x, f32 y, f32 z, f32 w) {
	Vec4 result = {
		.x = x,
		.y = y,
		.z = z,
		.w = w,
	};
	return result;
}
static inline Vec4 v4_splat(f32 value) {
	return v4_new(value, value, value, value);
}
static inline Vec4 v4_add(Vec4 a, Vec4 b) {
	return v4_f32_add(a, b);
}
static inline Vec4 v4_sub(Vec4 a, Vec4 b) {
	return v4_f32_sub(a, b);
}
static inline Vec4 v4_neg(Vec4 v) {
	return v4_f32_sub(v4_splat(0), v);
}
static inline Vec4 v4_mul(Vec4 a, Vec4 b) {
	return v4_f32_mul(a, b);
}
static inline Vec4 v4_scale(Vec4 v, f32 value) {
	return v4_mul(v, v4_splat(value));
}
static inline Vec4 v4_div(Vec4 a, Vec4 b) {
	return v4_f32_div(a, b);
}
static inline f32 v4_dot(Vec4 a, Vec4 b) {
	return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;
}
static inline f32 v4_sqrlen(Vec4 v) {
	return v4_dot(v, v);
}
static inline f32 v4_len(Vec4 v) {
	return sqrtf(v4_sqrlen(v));
}
static inline Vec4 v4_normal(Vec4 v) {
	f32 len = v4_len(v);
	if (len == 0.0) abort();
	return v4_scale(v, 1/len);
}

typedef struct V(f32, 3) Vec3;
struct V(f32, 3) {
	f32 x;
	f32 y;
	f32 z;
};
GENERATE_PRIMITIVE_VEC(f32, 3)
static inline Vec3 v3_new(f32 x, f32 y, f32 z) {
	Vec3 result = {
		.x = x,
		.y = y,
		.z = z,
	};
	return result;
}
static inline Vec3 v3_splat(f32 value) {
	return v3_new(value, value, value);
}
static inline Vec3 v3_add(Vec3 a, Vec3 b) {
	return v3_f32_add(a, b);
}
static inline Vec3 v3_sub(Vec3 a, Vec3 b) {
	return v3_f32_sub(a, b);
}
static inline Vec3 v3_neg(Vec3 v) {
	return v3_f32_sub(v3_splat(0), v);
}
static inline Vec3 v3_mul(Vec3 a, Vec3 b) {
	return v3_f32_mul(a, b);
}
static inline Vec3 v3_scale(Vec3 v, f32 value) {
	return v3_mul(v, v3_splat(value));
}
static inline Vec3 v3_div(Vec3 a, Vec3 b) {
	return v3_f32_div(a, b);
}
static inline f32 v3_dot(Vec3 a, Vec3 b) {
	return a.x*b.x + a.y*b.y + a.z*b.z;
}
static inline Vec3 v3_cross(Vec3 a, Vec3 b) {
	Vec3 result = {
		.x = a.z*b.y - a.y*b.z,
		.y = a.x*b.z - a.z*b.x,
		.z = a.y*b.x - a.x*b.y,
	};
	return result;
}
static inline f32 v3_sqrlen(Vec3 v) {
	return v3_dot(v, v);
}
static inline f32 v3_len(Vec3 v) {
	return sqrtf(v3_sqrlen(v));
}
static inline Vec3 v3_normal(Vec3 v) {
	f32 len = v3_len(v);
	if (len == 0.0) abort();
	return v3_scale(v, 1/len);
}

typedef struct V(f32, 2) Vec2;
struct V(f32, 2) {
	f32 x;
	f32 y;
};
GENERATE_PRIMITIVE_VEC(f32, 2)
static inline Vec2 v2_new(f32 x, f32 y) {
	Vec2 result = {
		.x = x,
		.y = y,
	};
	return result;
}
static inline Vec2 v2_splat(f32 value) {
	return v2_new(value, value);
}
static inline Vec2 v2_add(Vec2 a, Vec2 b) {
	return v2_f32_add(a, b);
}
static inline Vec2 v2_sub(Vec2 a, Vec2 b) {
	return v2_f32_sub(a, b);
}
static inline Vec2 v2_neg(Vec2 v) {
	return v2_f32_sub(v2_splat(0), v);
}
static inline Vec2 v2_mul(Vec2 a, Vec2 b) {
	return v2_f32_mul(a, b);
}
static inline Vec2 v2_scale(Vec2 v, f32 value) {
	return v2_mul(v, v2_splat(value));
}
static inline Vec2 v2_div(Vec2 a, Vec2 b) {
	return v2_f32_div(a, b);
}
static inline f32 v2_dot(Vec2 a, Vec2 b) {
	return a.x*b.x + a.y*b.y;
}
static inline f32 v2_sqrlen(Vec2 v) {
	return v2_dot(v, v);
}
static inline f32 v2_len(Vec2 v) {
	return sqrtf(v2_sqrlen(v));
}
static inline Vec2 v2_normal(Vec2 v) {
	f32 len = v2_len(v);
	if (len == 0.0) abort();
	return v2_scale(v, 1/len);
}

GENERATE_PRIMITIVE_INT_OPS(i32)

typedef struct V(i32, 3) IVec3;
struct V(i32, 3) {
	i32 x;
	i32 y;
	i32 z;
};
GENERATE_PRIMITIVE_INT_VEC(i32, 3)
static inline IVec3 iv3_new(i32 x, i32 y, i32 z) {
	IVec3 result = {
		.x = x,
		.y = y,
		.z = z,
	};
	return result;
}
static inline IVec3 iv3_splat(i32 value) {
	return iv3_new(value, value, value);
}
static inline IVec3 iv3_add(IVec3 a, IVec3 b) {
	return v3_i32_add(a, b);
}
static inline IVec3 iv3_sub(IVec3 a, IVec3 b) {
	return v3_i32_sub(a, b);
}
static inline IVec3 iv3_neg(IVec3 v) {
	return v3_i32_sub(iv3_splat(0), v);
}
static inline IVec3 iv3_mul(IVec3 a, IVec3 b) {
	return v3_i32_mul(a, b);
}
static inline IVec3 iv3_div(IVec3 a, IVec3 b) {
	return v3_i32_div(a, b);
}
static inline IVec3 iv3_mod(IVec3 a, IVec3 b) {
	return v3_i32_mod(a, b);
}

typedef struct V(i32, 2) IVec2;
struct V(i32, 2) {
	i32 x;
	i32 y;
};
GENERATE_PRIMITIVE_INT_VEC(i32, 2)
static inline IVec2 iv2_new(i32 x, i32 y) {
	IVec2 result = {
		.x = x,
		.y = y,
	};
	return result;
}
static inline IVec2 iv2_splat(i32 value) {
	return iv2_new(value, value);
}
static inline IVec2 iv2_add(IVec2 a, IVec2 b) {
	return v2_i32_add(a, b);
}
static inline IVec2 iv2_sub(IVec2 a, IVec2 b) {
	return v2_i32_sub(a, b);
}
static inline IVec2 iv2_neg(IVec2 v) {
	return v2_i32_sub(iv2_splat(0), v);
}
static inline IVec2 iv2_mul(IVec2 a, IVec2 b) {
	return v2_i32_mul(a, b);
}
static inline IVec2 iv2_div(IVec2 a, IVec2 b) {
	return v2_i32_div(a, b);
}
static inline IVec2 iv2_mod(IVec2 a, IVec2 b) {
	return v2_i32_mod(a, b);
}

// Transformations

static inline Vec4 v3_extend(Vec3 v, f32 w) {
	return v4_new(v.x, v.y, v.z, w);
}
static inline Vec3 v2_extend(Vec2 v, f32 z) {
	return v3_new(v.x, v.y, z);
}
static inline IVec3 iv2_extend(IVec2 v, i32 z) {
	return iv3_new(v.x, v.y, z);
}

static inline Vec3 v4_xyz(Vec4 v) {
	return v3_new(v.x, v.y, v.z);
}

typedef f32 f32gl;
GENERATE_PRIMITIVE_OPS(f32gl, 0.0)

typedef struct V(f32gl, 3) Vec3OpenGL;
struct V(f32gl, 3) {
	f32 x;
	f32 y;
	f32 z;
};
GENERATE_PRIMITIVE_VEC(f32gl, 3)
static inline Vec3OpenGL vgl_new(f32 x, f32 y, f32 z) {
	Vec3OpenGL result = {
		.x = x,
		.y = y,
		.z = z,
	};
	return result;
}
static inline Vec3OpenGL vgl_splat(f32 value) {
	return vgl_new(value, value, value);
}
static inline Vec3OpenGL vgl_add(Vec3OpenGL a, Vec3OpenGL b) {
	return v3_f32gl_add(a, b);
}
static inline Vec3OpenGL vgl_sub(Vec3OpenGL a, Vec3OpenGL b) {
	return v3_f32gl_sub(a, b);
}
static inline Vec3OpenGL vgl_neg(Vec3OpenGL v) {
	return vgl_sub(vgl_splat(0), v);
}
static inline Vec3OpenGL vgl_mul(Vec3OpenGL a, Vec3OpenGL b) {
	return v3_f32gl_mul(a, b);
}
static inline Vec3OpenGL vgl_scale(Vec3OpenGL v, f32 value) {
	return v3_f32gl_mul(v, vgl_splat(value));
}
static inline Vec3OpenGL vgl_div(Vec3OpenGL a, Vec3OpenGL b) {
	return v3_f32gl_div(a, b);
}
static inline f32 vgl_dot(Vec3OpenGL a, Vec3OpenGL b) {
	return a.x*b.x + a.y*b.y + a.z*b.z;
}
static inline Vec3OpenGL vgl_cross(Vec3OpenGL a, Vec3OpenGL b) {
	Vec3OpenGL result = {
		.x = a.y*b.z - a.z*b.y,
		.y = a.z*b.x - a.x*b.z,
		.z = a.x*b.y - a.y*b.x,
	};
	return result;
}
static inline f32 vgl_sqrlen(Vec3OpenGL v) {
	return vgl_dot(v, v);
}
static inline f32 vgl_len(Vec3OpenGL v) {
	return sqrtf(vgl_sqrlen(v));
}
static inline Vec3OpenGL vgl_normal(Vec3OpenGL v) {
	f32 len = vgl_len(v);
	if (len == 0.0) abort();
	return vgl_scale(v, 1/len);
}
static inline Vec3OpenGL v3_opengl(Vec3 v) {
	// left-handed z-up to right-handed y-up
	return vgl_new(v.y, v.z, -v.x);
}
