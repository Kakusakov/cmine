#pragma once
#include "types.h"
#include <math.h>

#define PI 3.14159265358979323846f
#define DEG_TO_RAD (PI / 180.0f)
#define RAD_TO_DEG (180.0f / PI)

typedef struct Angle Angle;
struct Angle {
	f32 rad;
};
static inline Angle angle_from_radians(f32 rad) {
	Angle result = {
		.rad = rad,
	};
	return result;
}
static inline Angle angle_from_degrees(f32 deg) {
	return angle_from_radians(deg * DEG_TO_RAD);
}
static inline f32 angle_as_radians(Angle ang) {
	return ang.rad;
}
static inline f32 angle_as_degrees(Angle ang) {
	return angle_as_radians(ang) * RAD_TO_DEG;
}
static inline f32 angle_sin(Angle ang) {
	return sinf(angle_as_radians(ang));
}
static inline f32 angle_cos(Angle ang) {
	return cosf(angle_as_radians(ang));
}
static inline Angle angle_add(Angle a, Angle b) {
	return angle_from_radians(angle_as_radians(a) + angle_as_radians(b));
}
static inline Angle angle_sub(Angle a, Angle b) {
	return angle_from_radians(angle_as_radians(a) - angle_as_radians(b));
}
static inline Angle angle_inverse(Angle ang) {
	return angle_from_radians(-angle_as_radians(ang));
}
static inline Angle angle_clamp(Angle ang, Angle min, Angle max) {
	f32 rad = angle_as_radians(ang);
	if (rad < angle_as_radians(min)) return min;
	if (rad > angle_as_radians(max)) return max;
	return ang;
}
static inline Angle angle_normal(Angle ang) {
	return angle_from_radians(fmodulo(angle_as_radians(ang), PI*2));
}
static inline Angle angle_scale(Angle ang, f32 scale) {
	return angle_from_radians(angle_as_radians(ang)*scale);
}
