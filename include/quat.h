#pragma once
#include "dir.h"
#include "angle.h"

typedef Vec4 Quat;
static inline Quat quat_from_axis_angle(Dir3 axis, Angle angle) {
	Angle halfang = angle_scale(angle, 0.5f);
	Vec3 v = dir3_with_magnitude(axis, angle_sin(halfang));
	return (Vec4){v.x, v.y, v.z, angle_cos(halfang)};
}
static inline Quat quat_from_eulers(Angle xang, Angle yang, Angle zang) {
	// zyx rotation order
	Angle xhalfang = angle_scale(xang, 0.5f);
	Angle yhalfang = angle_scale(yang, 0.5f);
	Angle zhalfang = angle_scale(zang, 0.5f);
	f32 sx = angle_sin(xhalfang);
	f32 cx = angle_cos(xhalfang);
	f32 sy = angle_sin(yhalfang);
	f32 cy = angle_cos(yhalfang);
	f32 sz = angle_sin(zhalfang);
	f32 cz = angle_cos(zhalfang);
	return (Vec4) {
		sx*cy*cz - cx*sy*sz,
		cx*sy*cz + sx*cy*sz,
		cx*cy*sz - sx*sy*cz,
		cx*cy*cz + sx*sy*sz
	};
}
static inline Quat quat_conj(Quat q) {
	return (Vec4){-q.x, -q.y, -q.z, q.w};
}
static inline Quat quat_mul(Quat a, Quat b) {
	Vec3 a_ax = v4_xyz(a);
	Vec3 b_ax = v4_xyz(b);
	Vec3 v = v3_add(
		v3_add(
			v3_scale(b_ax, a.w),
			v3_scale(a_ax, b.w)
		),
		v3_cross(a_ax, b_ax)
	);
	f32 w = a.w*b.w - v3_dot(a_ax, b_ax);
	return (Vec4){v.x, v.y, v.z, w};
}
static inline Vec3 quat_apply(Quat q, Vec3 v) {
	Quat qv = v3_extend(v, 0);
	Quat qr = quat_mul(quat_mul(q, qv), quat_conj(q));
	return v4_xyz(qr);
}

typedef struct QuatOpenGL QuatOpenGL;
struct QuatOpenGL {
	f32 x;
	f32 y;
	f32 z;
	f32 w;
};
static inline QuatOpenGL quat_opengl(Quat q) {
	QuatOpenGL result = {
		.x = -q.y,  // or maybe q.y
		.y = -q.z,  // or maybe q.z
		.z = q.x,  // or maybe -q.x
		.w = q.w,
	};
	return result;
}
