#include "image.h"
#include "glad.h"

#include "pack.h"
#include "safety.h"

#include <stdio.h>

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

static void* load_uncompressed_bmp_data(
	size_t width,
	size_t height,
	size_t pixel_size,
	int flip_x,
	int flip_y,
	FILE* file
) {
	if (!width || !height || !pixel_size || !file) return NULL;
	uint8_t* buf = smalloc(width * height * pixel_size);

	// Line size must be a multiple of 4.
	const uint32_t line_padding_size = (4 - ((width * pixel_size) % 4)) % 4;

	for (size_t y = flip_y ? height - 1 : 0;
		flip_y ? y != SIZE_MAX : y < height;
		flip_y ? y-- : y++
	) {
		for (size_t x = flip_x ? width - 1 : 0; 
			flip_x ? x != SIZE_MAX : x < width;
			flip_x ? x-- : x++
		) {
			if (!fread(&buf[(y * width + x) * pixel_size], pixel_size, 1, file)) goto err0;
		}
		if (fseek(file, line_padding_size, SEEK_CUR)) goto err0;
	}

	return buf;

err0:
	sfree(buf);
	return NULL;
}

static BmpImage load_bmp_from_file(FILE* file) {
	BmpFileHeader fh;
	BmpImage image = { 0 };
	if (!fread(&fh, sizeof(fh), 1, file)) return image;
	if (fh.signature[0] != bmp_signature[0] ||
		fh.signature[1] != bmp_signature[1]
	) return image;
	if (fh.reserved[0] || fh.reserved[1]) return image;

	uint32_t size;
	if (!fread(&size, sizeof(size), 1, file)) return image;
	switch (size) {
	case 40:
	{
		BmpImageHeader40 ih;
		if (!fread(&ih, sizeof(ih), 1, file)) return image;
		if (ih.color_planes_count != 1) return image;

		const int flip_x = ih.width < 0;
		const int flip_y = ih.height < 0;
		if (sizeof(size_t) < sizeof(uint32_t) &&
			(
				(uint32_t)SIZE_MAX < abs(ih.width) ||
				(uint32_t)SIZE_MAX < abs(ih.height) ||
				(uint32_t)SIZE_MAX < ih.bit_count
			)
		) return image;
		const size_t width = abs(ih.width);
		const size_t height = abs(ih.height);
		if (SIZE_MAX - 7 < (size_t)ih.bit_count) return image;
		const size_t byte_count = ((size_t)ih.bit_count + 7) / 8;
		if (SIZE_MAX / width / height < byte_count) return image;
		const size_t image_size = width * height * byte_count;

		uint8_t* buf;
		gl_enum texture_format;
		gl_enum texture_type;
		if (ih.compression == 0) {
			switch (ih.bit_count) {
			case 24:
				texture_format = GL_BGR;
				texture_type = GL_UNSIGNED_BYTE;
				break;
			case 32:
				texture_format = GL_BGRA;
				texture_type = GL_UNSIGNED_BYTE;
				break;
			default:
				return image;
			}

			buf = load_uncompressed_bmp_data(
				width,
				height,
				byte_count,
				flip_x,
				flip_y,
				file
			);
		}
		else return image;

		if (buf) {
			image.pixel_array = buf;
			image.width = width;
			image.height = height;
			image.pixel_size = byte_count;
			image.texture_format = texture_format;
			image.texture_type = texture_type;
		}
	}
	default:
		break;
	}

	return image;
}

BmpImage load_bmp_image(const char* filename) {
	BmpImage image = { 0 };

	FILE* file = fopen(filename, "rb");
	if (!file) return image;

	image = load_bmp_from_file(file);
	fclose(file);
	return image;
}

static bool store_uncompressed_bmp_data(
	const uint8_t* buf,
	size_t width,
	size_t height,
	size_t pixel_size,
	FILE* file
) {
	// Line size must be a multiple of 4.
	const uint32_t line_padding_size = (4 - ((width * pixel_size) % 4)) % 4;

	for (size_t y = 0; y < height; y++) {
		for (size_t x = 0; x < width; x++) {
			if (!fwrite(&buf[(y * width + x) * pixel_size], pixel_size, 1, file)) return false;
		}
		if (fseek(file, line_padding_size, SEEK_CUR)) return false;
	}

	return true;
}


static bool store_bmp_to_file(FILE* file, const BmpImage* image) {
	const uint32_t iamge_header_size = 40;
	if (sizeof(size_t) > sizeof(uint32_t) && 
		(
			(size_t)UINT32_MAX < image->width ||
			(size_t)UINT32_MAX < image->height ||
			(size_t)UINT32_MAX < image->pixel_size
		)
	) return false;
	const uint32_t width = (uint32_t)image->width;
	const uint32_t height = (uint32_t)image->height;
	const uint32_t byte_count = (uint32_t)image->pixel_size;

	if (UINT32_MAX / width / height < byte_count) return false;
	const uint32_t line_padding_size = (4 - ((width * byte_count) % 4)) % 4;
	if (UINT32_MAX - width * byte_count < line_padding_size) return false;
	if (UINT32_MAX / (width * byte_count + line_padding_size) < height) return false;
	const uint32_t image_size = (width * byte_count + line_padding_size) * height;
	const BmpImageHeader40 ih = {
		.width = width,
		.height = height,
		.bit_count = byte_count * 8,
		.compression = 0,
		.color_planes_count = 1,
		.image_size = image_size,
		.prefered_ppm_x = 0,
		.prefered_ppm_y = 0,
		.color_count = 0,
		.important_color_count = 0
	};
	const uint32_t pixel_array_offset = (
		sizeof(BmpFileHeader) + sizeof(uint32_t) + sizeof(BmpImageHeader40)
	);
	if (UINT32_MAX - pixel_array_offset < image_size) return false;
	const uint32_t file_size = pixel_array_offset + image_size;
	const BmpFileHeader fh = {
		.signature = { bmp_signature[0], bmp_signature[0] },
		.reserved = { 0, 0 },
		.pixel_data_offset = pixel_array_offset,
		.size = file_size
	};
	if (!fwrite(&fh, sizeof(fh), 1, file) ||
		!fwrite(&iamge_header_size, sizeof(iamge_header_size), 1, file) ||
		!store_uncompressed_bmp_data(
			image->pixel_array, 
			image->width, 
			image->height, 
			image->pixel_size, 
			file
		)
	) return false;

	return true;
}

bool store_bmp_image(const char* filename, const BmpImage* image) {
	FILE* file = fopen(filename, "wb");
	if (!file) return false;

	bool success = store_bmp_to_file(file, image);
	fclose(file);

	return success;
}

void free_bmp_image(BmpImage* image) {
	sfree(image->pixel_array);
	*image = (BmpImage){ 0 };
}
