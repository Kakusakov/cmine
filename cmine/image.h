#pragma once
#include "fixed_types.h"
#include "arena.h"
#include "pack.h"

#include <stdio.h>

PACK(struct Color {
	uint8_t red;
	uint8_t green;
	uint8_t blue;
	uint8_t alpha;
});
typedef struct Color Color;

struct Image {
	size_t width;
	size_t height;
	Color pixels[];
};
typedef struct Image Image;

Image* image_try_from_bmp_file(FILE* file, Arena* arena);
Image* image_load_bmp(const char* filename, Arena* arena);

