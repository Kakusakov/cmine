#pragma once
#include "fixed_types.h"

struct BmpImage {
	uint8_t* pixel_array;
	uint32_t width;
	size_t height;
	size_t pixel_size;
	gl_enum texture_format;
	gl_enum texture_type;
};
typedef struct BmpImage BmpImage;

BmpImage load_bmp_image(const char* filename);
bool store_bmp_image(const char* filename, const BmpImage* image);
void free_bmp_image(BmpImage* image);
