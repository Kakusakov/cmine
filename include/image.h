#pragma once
#include "pack.h"
#include <stdint.h>

typedef struct Color32 Color32;
PACK(struct Color32
{
	uint8_t red;
	uint8_t green;
	uint8_t blue;
	uint8_t alpha;
});
#define COLOR32_PINK ((Color32){ 0xFF, 0x96, 0x9F, 0xFF })
#define COLOR32_GREEN ((Color32){ 0xB3, 0xFF, 0x70, 0xFF })
#define COLOR32_BLUE ((Color32){ 0x00, 0x6A, 0xFF, 0xFF })
#define COLOR32_BLACK ((Color32){ 0x00, 0x00, 0x00, 0xFF })

typedef enum ImageError ImageError;
enum ImageError
{
	image_error_ok,  // No error.
	image_error_io,  // Unbale to perform operation because of an IO error.
	image_error_alloc,  // Failed to allocate sufficient amount of memory to perform operation.
	image_error_invalid,  // Input data is invalid or somehow corrupted.
	image_error_unsupported,  // Operation not supported for this type of input.
};

typedef struct Image Image;
struct Image
{
	size_t width;
	size_t height;
	Color32 *pixels;
};

ImageError image_load_bmp(Image *image, const char *filepath);
