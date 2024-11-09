#pragma once
#include <stdlib.h>
typedef struct Alloc Alloc;
struct Alloc {
	void* allocator;
	void (*allocate)(void* allocator, void** ptr, size_t size);
	void (*reallocate)(void* allocator, void** ptr, size_t size);
	void (*deallocate)(void* allocator, void** ptr);
};

static inline void allocate(Alloc* self, void** ptr, size_t size) {
	self->allocate(self->allocator, ptr, size);
}
static inline void reallocate(Alloc* self, void** ptr, size_t size) {
	self->reallocate(self->allocator, ptr, size);
}
static inline void deallocate(Alloc* self, void** ptr) {
	self->deallocate(self->allocator, ptr);
}

static inline void std_allocator_allocate(void* null, void** ptr, size_t size) {
	void* result = malloc(size);
	if (result == NULL) abort();
	*ptr = result;
}
static inline void std_allocator_reallocate(void* null, void** ptr, size_t size) {
	void* result = realloc(*ptr, size);
	if (result == NULL) abort();
	*ptr = result;
}
static inline void std_allocator_deallocate(void* null, void** ptr) {
	free(*ptr);
	*ptr = NULL;
}
static inline Alloc* std_allocator_alloc(void) {
	static Alloc alloc = {
		.allocator = NULL,
		.allocate = std_allocator_allocate,
		.reallocate = std_allocator_reallocate,
		.deallocate = std_allocator_deallocate,
	};
	return &alloc;
}
