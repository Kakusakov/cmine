#pragma once
#include <stdint.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef float f32;
typedef double f64;
typedef u8 bool;

#define true 1
#define false 0

// TODO: theese functions should be in appropriate header...
#include <math.h>
static inline i32 imodulo(i32 value, i32 modulo) {
	i32 result = value % modulo;
	return result >= 0 ? result : result + modulo;
}
static inline f32 fmodulo(f32 value, f32 modulo) {
	return value - floorf(value/modulo)*modulo;
}
