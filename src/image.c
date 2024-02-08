#include "image.h"
#include "safety.h"

#include <string.h>
#include <stdlib.h>  // Why in the world is abs() located at stdlib.h and not at math.h???

PACK(struct BmpFileHeader {
	uint8_t signature[2];
	uint32_t size;
	uint16_t reserved[2];
	uint32_t pixel_data_offset;
});
typedef struct BmpFileHeader BmpFileHeader;

PACK(struct BmpImageHeader40 {
	int32_t width;
	int32_t height;
	uint16_t color_planes_count;
	uint16_t bit_count;
	uint32_t compression;
	uint32_t image_size;
	uint32_t prefered_ppm_x;
	uint32_t prefered_ppm_y;
	uint32_t color_count;
	uint32_t important_color_count;
});
typedef struct BmpImageHeader40 BmpImageHeader40;

static const uint8_t bmp_signature[2] = { 'B', 'M' };

static Color color_from_bgra(uint32_t bgra_color) {
	Color temp = {0};
	memcpy(&temp, &bgra_color, sizeof(bgra_color));
	return (Color) {
		.red = temp.blue,
		.green = temp.green,
		.blue = temp.red,
		.alpha = temp.alpha,
	};
}

static bool load_uncompressed_bmp_data(
	FILE* file,
	Image* image,
	size_t pixel_size,
	bool flip_x,
	bool flip_y) 
{
	if (!image->width || !image->height || !pixel_size || !file) return false;

	// Line size must be a multiple of 4.
	uint32_t line_padding_size = (4 - ((image->width * pixel_size) % 4)) % 4;

	for (size_t y = flip_y ? image->height - 1 : 0;
		flip_y ? y != SIZE_MAX : y < image->height;
		flip_y ? y-- : y++) 
	{
		for (size_t x = flip_x ? image->width - 1 : 0;
			flip_x ? x != SIZE_MAX : x < image->width;
			flip_x ? x-- : x++) 
		{
			uint32_t bgra_color = ~0;
			if (!fread(&bgra_color, pixel_size, 1, file)) return false;
			image->pixels[y * image->width + x] = color_from_bgra(bgra_color);
		}
		if (fseek(file, line_padding_size, SEEK_CUR)) return false;
	}

	return true;
}

static Image* image_init(Arena* arena, size_t width, size_t height) {
	Image* image = arena_allocate(arena, sizeof(Image) + width * height * sizeof(Color), _Alignof(Image));
	image->width = width;
	image->height = height;
	return image;
}

Image* image_try_from_bmp_file(FILE* file, Arena* arena) {
	BmpFileHeader fh;
	if (!fread(&fh, sizeof(fh), 1, file)) return NULL;
	if (fh.signature[0] != bmp_signature[0] ||
		fh.signature[1] != bmp_signature[1] ||
		fh.reserved[0] || 
		fh.reserved[1]) return NULL;

	uint32_t size;
	if (!fread(&size, sizeof(size), 1, file)) return NULL;
	if (size == 40) {
		BmpImageHeader40 ih;
		if (!fread(&ih, sizeof(ih), 1, file)) return NULL;
		if (ih.color_planes_count != 1) return NULL;

		bool flip_x = ih.width < 0;
		bool flip_y = ih.height < 0;
		if (sizeof(size_t) < sizeof(uint32_t) && (
			((uint32_t)SIZE_MAX) < ((uint32_t)labs(ih.width)) ||
			((uint32_t)SIZE_MAX) < ((uint32_t)labs(ih.height)) ||
			((uint32_t)SIZE_MAX) < ((uint32_t)ih.bit_count))) return NULL;

		size_t width = labs(ih.width);
		size_t height = labs(ih.height);
		if (SIZE_MAX - 7 < (size_t)ih.bit_count) return NULL;
		size_t byte_count = ((size_t)ih.bit_count + 7) / 8;
		if (SIZE_MAX / width / height < byte_count) return NULL;
		size_t image_size = width * height * byte_count;

		if (ih.compression != 0) return NULL;
		if (ih.bit_count != 24 && ih.bit_count != 32) return NULL;

		Image* image = image_init(arena, width, height);
		if (!load_uncompressed_bmp_data(
			file,
			image,
			byte_count,
			flip_x,
			flip_y)) 
		{
			arena_reallocate(arena, 0, 0);
			return NULL;
		}
		return image;
	}
	return NULL;
}

Image* image_load_bmp(const char* filename, Arena* arena) {
	FILE* file;
	try(file = fopen(filename, "rb"));
	Image* image;
	try(image = image_try_from_bmp_file(file, arena));
	return image;
}

//struct BmpImage {
//	size_t width;
//	size_t height;
//	size_t pixel_size;
//
//	uint8_t pixel_array[];
//};
//typedef struct BmpImgae BmpImage;
// 
//static bool store_uncompressed_bmp_data(
//	const uint8_t* buf,
//	size_t width,
//	size_t height,
//	size_t pixel_size,
//	FILE* file
//) {
//	// Line size must be a multiple of 4.
//	const uint32_t line_padding_size = (4 - ((width * pixel_size) % 4)) % 4;
//
//	for (size_t y = 0; y < height; y++) {
//		for (size_t x = 0; x < width; x++) {
//			if (!fwrite(&buf[(y * width + x) * pixel_size], pixel_size, 1, file)) return false;
//		}
//		if (fseek(file, line_padding_size, SEEK_CUR)) return false;
//	}
//
//	return true;
//}
//
//static bool store_bmp_to_file(FILE* file, const BmpImage* image) {
//	const uint32_t iamge_header_size = 40;
//	if (sizeof(size_t) > sizeof(uint32_t) && 
//		(
//			(size_t)UINT32_MAX < image->width ||
//			(size_t)UINT32_MAX < image->height ||
//			(size_t)UINT32_MAX < image->pixel_size
//		)
//	) return false;
//	const uint32_t width = (uint32_t)image->width;
//	const uint32_t height = (uint32_t)image->height;
//	const uint32_t byte_count = (uint32_t)image->pixel_size;
//
//	if (UINT32_MAX / width / height < byte_count) return false;
//	const uint32_t line_padding_size = (4 - ((width * byte_count) % 4)) % 4;
//	if (UINT32_MAX - width * byte_count < line_padding_size) return false;
//	if (UINT32_MAX / (width * byte_count + line_padding_size) < height) return false;
//	const uint32_t image_size = (width * byte_count + line_padding_size) * height;
//	const BmpImageHeader40 ih = {
//		.width = width,
//		.height = height,
//		.bit_count = byte_count * 8,
//		.compression = 0,
//		.color_planes_count = 1,
//		.image_size = image_size,
//		.prefered_ppm_x = 0,
//		.prefered_ppm_y = 0,
//		.color_count = 0,
//		.important_color_count = 0
//	};
//	const uint32_t pixel_array_offset = (
//		sizeof(BmpFileHeader) + sizeof(uint32_t) + sizeof(BmpImageHeader40)
//	);
//	if (UINT32_MAX - pixel_array_offset < image_size) return false;
//	const uint32_t file_size = pixel_array_offset + image_size;
//	const BmpFileHeader fh = {
//		.signature = { bmp_signature[0], bmp_signature[0] },
//		.reserved = { 0, 0 },
//		.pixel_data_offset = pixel_array_offset,
//		.size = file_size
//	};
//	if (!fwrite(&fh, sizeof(fh), 1, file) ||
//		!fwrite(&iamge_header_size, sizeof(iamge_header_size), 1, file) ||
//		!store_uncompressed_bmp_data(
//			image->pixel_array, 
//			image->width, 
//			image->height, 
//			image->pixel_size, 
//			file
//		)
//	) return false;
//
//	return true;
//}
