#pragma once
#include <stddef.h>

typedef struct Arena Arena;

Arena* arena_init(void);
void arena_deinit(Arena* const arena);

void* arena_alloc(Arena* const arena, const size_t size, const size_t alignment);
void* arena_realloc(Arena* const arena, const size_t size, const size_t alignment);
void arena_reset(Arena* const arena);
