#include "fixed_types.h"

uint32_t hash_u32(uint32_t u) {
	u = (u ^ 61) ^ (u >> 16);
	u = u + (u << 3);
	u = u ^ (u >> 4);
	u = u * 0x27d4eb2d;
	u = u ^ (u >> 15);
	return u;
}
