#include "safety.h"
#include <stdlib.h>
#include <stdio.h>

static void (*satexit_callback)(void) = NULL;

static void _smalloc_fail(size_t size) {
	fprintf(
		stderr,
		"\nCaught runtime error:\n"
		"\tsmalloc failed to allocate `%zu` bytes of memory.\n"
		"\tExiting...\n",
		size
	);
	sexit(1);
}
void _stry_fail(const char* expression, const char* filename, unsigned line_number) {
	fprintf(
		stderr,
		"\nCaught error in file `%s` line `%u`:\n"
		"\texpression `%s` was evaluated to false inside of a try block.\n"
		"\tExiting...\n", 
		filename, 
		line_number,
		expression
	);
	sexit(1);
}
void* smalloc(size_t size) {
	void* ptr = malloc(size);
	if (!ptr) _smalloc_fail(size);
	return ptr;
}
void* srealloc(void* block, size_t size) {
	void* ptr = smalloc(size);
	free(block);
	return ptr;
}

void satexit(void (*callback)(void)) {
	satexit_callback = callback;
}
void sexit(int error_code) {
	static int is_exiting = 0;
	if (is_exiting) abort();
	else is_exiting = 1;
	if (satexit_callback) satexit_callback();
	exit(error_code);
}
