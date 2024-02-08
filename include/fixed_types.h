#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <limits.h>
#include <stddef.h>

typedef float float32_t;
typedef double float64_t;

_Static_assert(sizeof(float32_t) * CHAR_BIT == 32, "float32_t is not a 32 bit floating pointer");
_Static_assert(sizeof(float64_t) * CHAR_BIT == 64, "float64_t is not a 64 bit floating pointer");

typedef uint32_t gl_handle;
typedef uint32_t gl_enum;
