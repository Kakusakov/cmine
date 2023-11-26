#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <limits.h>

typedef float float32_t;
typedef double float64_t;

_Static_assert(sizeof(float32_t) * CHAR_BIT == 32, "f32 is not a 32 bit floating pointer");
_Static_assert(sizeof(float64_t) * CHAR_BIT == 64, "f64 is not a 64 bit floating pointer");
