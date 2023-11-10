#pragma once
#include <stdint.h>

struct Region {
	struct Region* next;
	size_t count;
	size_t capacity;
	uintptr_t data[];
};
typedef struct Region Region;

Region* region_alloc(size_t least_capacity);
void region_free(Region* region);

struct Arena {
	Region* first;
	Region* current;
};
typedef struct Arena Arena;

Arena arena_new(void);
void* arena_alloc(Arena* arena, size_t size);
void arena_reset(Arena* arena);
void arena_free(Arena* arena);
