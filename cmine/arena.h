#pragma once
#include "fixed_types.h"

typedef struct Arena Arena;

Arena* arena_init(void);
void arena_deinit(Arena* const arena);

void* arena_allocate(Arena* const arena, const size_t size, const size_t alignment);
void* arena_reallocate(Arena* const arena, const size_t size, const size_t alignment);
void arena_reset(Arena* const arena);

#define arena_alloc(arena, count, type) ((type*)arena_allocate(arena, sizeof(type) * count, _Alignof(type)))
#define arena_realloc(arena, count, type) ((type*)arena_reallocate(arena, sizeof(type) * count, _Alignof(type)))
