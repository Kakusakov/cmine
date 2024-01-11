#pragma once
#include "fixed_types.h"
#include "safety.h"

static inline int32_t modi(int32_t a, int32_t b) {
	int32_t result = a % b;
	return result >= 0 ? result : result + b;
}

typedef struct Vec3i Vec3i;
struct Vec3i {
	int32_t x;
	int32_t y;
	int32_t z;
};

static inline Vec3i vec3i_add_v(Vec3i a, Vec3i b) {
	return (Vec3i) { a.x + b.x, a.y + b.y, a.z + b.z };
}
static inline Vec3i vec3i_sub_v(Vec3i a, Vec3i b) {
	return (Vec3i) { a.x - b.x, a.y - b.y, a.z - b.z };
}
static inline Vec3i vec3i_mul_v(Vec3i a, Vec3i b) {
	return (Vec3i) { a.x* b.x, a.y* b.y, a.z* b.z };
}
static inline Vec3i vec3i_div_v(Vec3i a, Vec3i b) {
	try(b.x != 0 && b.y != 0 && b.z != 0);
	return (Vec3i) { a.x / b.x, a.y / b.y, a.z / b.z };
}
static inline Vec3i vec3i_rem_v(Vec3i a, Vec3i b) {
	try(b.x != 0 && b.y != 0 && b.z != 0);
	return (Vec3i) { a.x % b.x, a.y % b.y, a.z % b.z };
}
static inline Vec3i vec3i_mod_v(Vec3i a, Vec3i b) {
	try(b.x != 0 && b.y != 0 && b.z != 0);
	return (Vec3i) { modi(a.x, b.x), modi(a.y, b.y), modi(a.z, b.z) };
}

static inline Vec3i vec3i_add(Vec3i a, int32_t value) {
	return (Vec3i) { a.x + value, a.y + value, a.z + value };
}
static inline Vec3i vec3i_sub(Vec3i a, int32_t value) {
	return (Vec3i) { a.x - value, a.y - value, a.z - value };
}
static inline Vec3i vec3i_mul(Vec3i a, int32_t value) {
	return (Vec3i) { a.x* value, a.y* value, a.z* value };
}
static inline Vec3i vec3i_div(Vec3i a, int32_t value) {
	try(value != 0);
	return (Vec3i) { a.x / value, a.y / value, a.z / value };
}
static inline Vec3i vec3i_rem(Vec3i a, int32_t value) {
	try(value != 0);
	return (Vec3i) { a.x % value, a.y % value, a.z % value };
}
static inline Vec3i vec3i_mod(Vec3i a, int32_t value) {
	try(value != 0);
	return (Vec3i) { modi(a.x, value), modi(a.y, value), modi(a.z, value) };
}
