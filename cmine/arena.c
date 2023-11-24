#include "arena.h"

#include "safety.h"

#include <stdlib.h>
#include <stdint.h>

typedef struct Region Region;
struct Region {
	Region* next;
	size_t count;
	size_t capacity;
	uintptr_t data[];
};

static Region* region_init(const size_t least_capacity) {
	static const size_t minimal_region_capacity = 8 * 1024 / sizeof(uintptr_t);

	const size_t capacity = least_capacity < minimal_region_capacity ? 
		minimal_region_capacity : 
		least_capacity;

	Region* const region = smalloc(sizeof(Region) + capacity * sizeof(uintptr_t));
	region->next = NULL;
	region->count = 0;
	region->capacity = capacity;

	return region;
}

static void region_deinit(Region* const region) {
	free(region);
}

struct Arena {
	size_t prev_count;
	Region* first;
	Region* current;
};

Arena* arena_init(void) {
	Arena* arena = smalloc(sizeof(Arena));
	*arena = (Arena){
		.prev_count = 0,
		.first = NULL,
		.current = NULL,
	};
	return arena;
}

void* arena_alloc(Arena* const arena, const size_t size) {
	const size_t count = (size + sizeof(uintptr_t) - 1) / sizeof(uintptr_t);
	arena->prev_count += count;

	if (arena->current == NULL) {
		try(arena->first == NULL);
		arena->first = region_init(count);
		arena->current = arena->first;
		arena->prev_count = 0;
	}

	while (
		arena->current->capacity - arena->current->count < count &&
		arena->current->next != NULL)
	{
		arena->current = arena->current->next;
		arena->prev_count = 0;
	}

	if (arena->current->capacity - arena->current->count < count) {
		arena->current->next = region_init(count);
		arena->prev_count = 0;
	}

	void* const result = &arena->current->data[arena->current->count];
	arena->current->count += count;

	return result;
}

void* arena_realloc(Arena* const arena, const size_t size) {
	arena->current->count = arena->prev_count;
	return arena_alloc(arena, size);
}

void arena_reset(Arena* const arena) {
	for (Region *region = arena->first; 
		region != NULL; 
		region = region->next
	) region->count = 0;
	arena->current = arena->first;
}

void arena_deinit(Arena* const arena) {
	Region* region = arena->first;
	while (region != NULL) {
		Region* const next = region->next;
		region_deinit(region);
		region = next;
	}

	arena->first = NULL;
	arena->current = NULL;
	arena->prev_count = 0;

	free(arena);
}
