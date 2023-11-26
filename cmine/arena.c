#include "arena.h"

#include "safety.h"

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct Region Region;
struct Region {
	Region* next;
	size_t size;
	size_t capacity;
	uint8_t data[];
};

static Region* region_init(const size_t least_capacity) {
	static const size_t minimal_region_capacity = 8 * 1024;

	const size_t capacity = least_capacity < minimal_region_capacity ? 
		minimal_region_capacity : 
		least_capacity;

	Region* const region = smalloc(sizeof(Region) + capacity);
	region->next = NULL;
	region->size = 0;
	region->capacity = capacity;

	return region;
}

static void region_deinit(Region* const region) {
	free(region);
}

static void region_chain_deinit(Region* const region) {
	Region* current = region;
	while (current != NULL) {
		Region* const next = current->next;
		region_deinit(current);
		current = next;
	}
}

static void* region_try_alloc(
	Region* const region, 
	const size_t size, 
	const size_t alignment) 
{
	try(alignment != 0);
	const size_t padding = region->size % alignment;
	if (SIZE_MAX - padding >= size) {
		return NULL;
	}
	const size_t total = padding + size;
	if (region->capacity - region->size < total) {
		return NULL;
	}
	void* const result = &region->data[region->size + padding];
	region->size += total;
	return result;
}

static Region* region_before(Region* const begin, Region* const before) {
	try(begin != NULL);
	Region* current = begin;
	Region* next = begin->next;
	while (next != before) {
		try(next);
		current = next;
		next = next->next;
	}
	return current;
}

struct Arena {
	size_t prev_size;
	Region* empty;
	Region* first;
	Region* current;
};

Arena* arena_init(void) {
	Arena* arena = smalloc(sizeof(Arena));
	*arena = (Arena){
		.prev_size = 0,
		.empty = NULL,
		.first = NULL,
		.current = NULL,
	};
	return arena;
}

static Region* arena_fetch_region(Arena* const arena, const size_t size) {
	Region* empty_prev = arena->empty;
	if (empty_prev != NULL) {
		if (empty_prev->capacity >= size) {
			Region* const result = empty_prev;
			arena->empty = empty_prev->next;
			result->next = NULL;
			result->size = size;
			return result;
		}
		Region* empty_current = empty_prev->next;
		while (empty_current != NULL) {
			if (empty_current->capacity >= size) {
				Region* const result = empty_current;
				empty_prev->next = empty_current->next;
				result->next = NULL;
				result->size = size;
				return result;
			}
			empty_prev = empty_current;
			empty_current = empty_current->next;
		}
	}

	Region* const result = region_init(size);
	result->size = size;
	return result;
}

static void arena_fetch_first(Arena* const arena, const size_t size) {
	try(arena->first == NULL);
	arena->first = arena_fetch_region(arena, size);
	arena->current = arena->first;
	arena->prev_size = 0;
}

static void arena_fetch_current(Arena* const arena, const size_t size) {
	arena->current->next = arena_fetch_region(arena, size);
	arena->current = arena->current->next;
	arena->prev_size = 0;
}

void* arena_allocate(Arena* const arena, const size_t size, const size_t alignment) {
	if (arena->current == NULL) {
		arena_fetch_first(arena, size);
		return &arena->first->data;
	}

	arena->prev_size = arena->current->size;
	void* const result = region_try_alloc(arena->current, size, alignment);
	if (result == NULL) {
		arena_fetch_current(arena, size);
		return &arena->current->data;
	}

	return result;
}

void* arena_reallocate(Arena* const arena, const size_t size, const size_t alignment) {
	if (arena->current == NULL) {
		arena_fetch_first(arena, size);
		return &arena->first->data;
	}

	const uint8_t* const tmp_data = &arena->current->data[arena->prev_size];
	const size_t tmp_size = arena->current->size - arena->prev_size;

	arena->current->size = arena->prev_size;
	void* const result = region_try_alloc(arena->current, size, alignment);
	if (result == NULL) {
		if (arena->current->size == 0) {
			if (arena->current == arena->first) {
				arena_reset(arena);
				arena_fetch_first(arena, size);
			}
			else {
				Region* const last_empty = region_before(arena->empty, NULL);
				last_empty->next = arena->current;
				Region* const prev_region = region_before(arena->first, arena->current);
				arena->current = prev_region;
				arena->current->next = NULL;
				arena_fetch_current(arena, size);
			}
		}
		else {
			arena_fetch_current(arena, size);
		}

		try(!memcpy_s(&arena->current->data, arena->current->size, tmp_data, tmp_size));
		return &arena->current->data;
	}


	return result;
}

void arena_reset(Arena* const arena) {
	if (!arena->empty) {
		arena->empty = arena->first;
		arena->first = NULL;
		arena->current = NULL;
		arena->prev_size = 0;
		return;
	}
	Region* const last_empty = region_before(arena->empty, NULL);
	last_empty->next = arena->first;
	arena->first = NULL;
	arena->current = NULL;
}

void arena_deinit(Arena* const arena) {
	region_chain_deinit(arena->first);
	region_chain_deinit(arena->empty);

	arena->first = NULL;
	arena->current = NULL;
	arena->empty = NULL;
	arena->prev_size = 0;

	free(arena);
}
