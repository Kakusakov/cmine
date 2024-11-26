#pragma once
#include "types.h"
#include <stddef.h>
#include <stdlib.h>
#include <math.h>

#define GENERATE_VEC(T, S, VEC, BODY)\
typedef union VEC VEC;\
union VEC {\
	struct {\
		BODY\
	};\
	T values[S];\
};\

#define GENERATE_VEC_FNS(T, S, VEC, PREFIX)\
static inline VEC PREFIX##splat(T value) {\
	VEC result = {0};\
	for (size_t i = 0; i < S; i++) {\
		result.values[i] = value;\
	}\
	return result;\
}\
static inline VEC PREFIX##add(VEC a, VEC b) {\
	VEC result = {0};\
	for (size_t i = 0; i < S; i++) {\
		result.values[i] = a.values[i] + b.values[i];\
	}\
	return result;\
}\
static inline VEC PREFIX##sub(VEC a, VEC b) {\
	VEC result = {0};\
	for (size_t i = 0; i < S; i++) {\
		result.values[i] = a.values[i] - b.values[i];\
	}\
	return result;\
}\
static inline VEC PREFIX##mul(VEC a, VEC b) {\
	VEC result = {0};\
	for (size_t i = 0; i < S; i++) {\
		result.values[i] = a.values[i] * b.values[i];\
	}\
	return result;\
}\
static inline VEC PREFIX##div(VEC a, VEC b) {\
	VEC result = {0};\
	for (size_t i = 0; i < S; i++) {\
		if (b.values[i] == 0) abort();\
		result.values[i] = a.values[i] / b.values[i];\
	}\
	return result;\
}\
static inline VEC PREFIX##neg(VEC a) {\
	return PREFIX##sub(PREFIX##splat(0), a);\
}\

#define GENERATE_IVEC_FNS(T, S, VEC, PREFIX)\
static inline VEC PREFIX##mod(VEC a, VEC b) {\
	VEC result = {0};\
	for (size_t i = 0; i < S; i++) {\
		if (b.values[i] == 0) abort();\
		a.values[i] = (a.values[i] % b.values[i] + b.values[i]) % b.values[i];\
	}\
	return result;\
}\

#define GENERATE_FVEC_FNS(T, S, VEC, PREFIX, SQRT_FN)\
static inline VEC PREFIX##scale(VEC a, T value) {\
	return PREFIX##mul(PREFIX##splat(value), a);\
}\
static inline T PREFIX##dot(VEC a, VEC b) {\
	T result = 0;\
	for (size_t i = 0; i < S; i++) {\
		result += a.values[i] * b.values[i];\
	}\
	return result;\
}\
static inline T PREFIX##sqrlen(VEC a) {\
	return PREFIX##dot(a, a);\
}\
static inline T PREFIX##len(VEC a) {\
	return SQRT_FN(PREFIX##sqrlen(a));\
}\
static inline VEC PREFIX##normal(VEC a) {\
	T len = PREFIX##len(a);\
	if (len == 0) abort();\
	return PREFIX##scale(a, 1 / len);\
}\

GENERATE_VEC(f32, 2, Vec2, f32 x; f32 y;)
GENERATE_VEC_FNS(f32, 2, Vec2, v2_);
GENERATE_FVEC_FNS(f32, 2, Vec2, v2_, sqrtf);

GENERATE_VEC(f32, 3, Vec3, f32 x; f32 y; f32 z;)
GENERATE_VEC_FNS(f32, 3, Vec3, v3_);
GENERATE_FVEC_FNS(f32, 3, Vec3, v3_, sqrtf);
static inline Vec3 v3_cross(Vec3 a, Vec3 b) {
	return (Vec3) {
		.x = a.z * b.y - a.y * b.z,
		.y = a.x * b.z - a.z * b.x,
		.z = a.y * b.x - a.x * b.y,
	};
}

GENERATE_VEC(f32, 4, Vec4, f32 x; f32 y; f32 z; f32 w;)
GENERATE_VEC_FNS(f32, 4, Vec4, v4_);
GENERATE_FVEC_FNS(f32, 4, Vec4, v4_, sqrtf);
static inline Vec3 v4_xyz(Vec4 a) {
	return (Vec3) {
		.x = a.x,
		.y = a.y,
		.z = a.z,
	};
}

GENERATE_VEC(i32, 2, IVec2, i32 x; i32 y;)
GENERATE_VEC_FNS(i32, 2, IVec2, iv2_);
GENERATE_IVEC_FNS(i32, 2, IVec2, iv2_);

GENERATE_VEC(i32, 3, IVec3, i32 x; i32 y; i32 z;)
GENERATE_VEC_FNS(i32, 3, IVec3, iv3_);
GENERATE_IVEC_FNS(i32, 3, IVec3, iv3_);

GENERATE_VEC(f32, 3, Vec3OpenGL, f32 x; f32 y; f32 z;)
GENERATE_VEC_FNS(f32, 3, Vec3OpenGL, v3gl_);
GENERATE_FVEC_FNS(f32, 3, Vec3OpenGL, v3gl_, sqrtf);
static inline Vec3OpenGL v3gl_cross(Vec3OpenGL a, Vec3OpenGL b) {
	return (Vec3OpenGL) {
		.x = a.y * b.z - a.z * b.y,
		.y = a.z * b.x - a.x * b.z,
		.z = a.x * b.y - a.y * b.x,
	};
}

static inline Vec3 v2_extend(Vec2 v, f32 z) {
	return (Vec3) {
		.x = v.x,
			.y = v.y,
			.z = z,
	};
}
static inline IVec3 iv2_extend(IVec2 v, i32 z) {
	return (IVec3) {
		.x = v.x,
		.y = v.y,
		.z = z,
	};
}
static inline Vec4 v3_extend(Vec3 v, f32 w) {
	return (Vec4) {
		.x = v.x,
		.y = v.y,
		.z = v.z,
		.w = w,
	};
}

static inline Vec3OpenGL v3_to_opengl(Vec3 a) {
	// left-handed z-up to right-handed y-up
	return (Vec3OpenGL) {
		.x = a.y,
		.y = a.z,
		.z = -a.x,
	};
}
