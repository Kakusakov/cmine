#include "arena.h"

#include "safety.h"

#include <stdlib.h>

Region *region_alloc(size_t least_capacity) {
	static const size_t minimal_region_capacity = 8 * 1024 / sizeof(uintptr_t);

	size_t capacity = least_capacity < minimal_region_capacity ? 
		minimal_region_capacity : 
		least_capacity;

	Region *region = smalloc(sizeof(Region) + capacity * sizeof(uintptr_t));
	region->next = NULL;
	region->count = 0;
	region->capacity = capacity;

	return region;
}

void region_free(Region* region) {
	free(region);
}

Arena arena_new(void) {
	return (Arena) {
		.first = NULL,
		.current = NULL,
	};
}

void* arena_alloc(Arena *arena, size_t size) {
	size_t count = (size + sizeof(uintptr_t) - 1) / sizeof(uintptr_t);

	if (arena->current == NULL) {
		TRY(arena->first == NULL);
		arena->first = region_alloc(count);
		arena->current = arena->first;
	}

	while (
		arena->current->capacity - arena->current->count < count && 
		arena->current->next != NULL
	) arena->current = arena->current->next;

	if (arena->current->capacity - arena->current->count < count) {
		arena->current->next = region_alloc(count);
	}

	void *result = &arena->current->data[arena->current->count];
	arena->current->count += count;

	return result;
}

void arena_reset(Arena *arena) {
	for (Region *region = arena->first; 
		region != NULL; 
		region = region->next
	) region->count = 0;
	arena->current = arena->first;
}

void arena_free(Arena *arena) {
	Region *region = arena->first; 
	while (region != NULL) {
		Region *next = region->next;
		free(region);
		region = next;
	}

	arena->first = NULL;
	arena->current = NULL;
}
