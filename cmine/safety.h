#pragma once
#include <stddef.h>

void _stry_fail(const char* expression, const char* filename, unsigned line_number);
#define TRY(expression) (void)(!!(expression) || (_stry_fail(#expression, __FILE__, __LINE__), 0))

inline void* smalloc(size_t size);
//inline void* srealloc(void* block, size_t size);
void satexit(void (* callback)(void));
void sexit(int error_code);
