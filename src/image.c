#include "image.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define ASSERT(x) assert(x)

typedef struct BmpFileHeader BmpFileHeader;
PACK(struct BmpFileHeader {
	uint8_t signature[2];
	uint32_t size;
	uint16_t reserved[2];
	uint32_t pixel_data_offset;
});

typedef struct BmpImageHeader40 BmpImageHeader40;
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

typedef struct BmpInfo BmpInfo;
struct BmpInfo
{
	BmpFileHeader file_header;
	uint32_t image_header_size;
	union {
		BmpImageHeader40 size40;
	} image_header;
};

static ImageError bmp_load_pixels_uncompressed_u24_or_u32(
	FILE *file,
	uint32_t pixel_data_offset,
	Image *image,
	size_t pixel_size,
	int flip_x,
	int flip_y)
{
	ASSERT(file);
	ASSERT(image);
	ASSERT(pixel_size == 3 || pixel_size == 4);
	if (image->width == 0 || 
		image->height == 0 ||
		pixel_data_offset > (uint32_t)INT32_MAX) return image_error_invalid;
	if (fseek(file, (long)pixel_data_offset, SEEK_SET)) return image_error_io;
	

	// Line size must be a multiple of 4.
	long line_padding_size = (long)((4 - ((image->width * pixel_size) % 4)) % 4);

	size_t y = flip_y ? image->height - 1 : 0;
	while (1)
	{	
		size_t x = flip_x ? image->width - 1 : 0;
		while(1)
		{
			union {
				uint32_t u32;
				Color32 color;
			} tmp;
			tmp.u32 = 0xFFFFFFFF;
			if (!fread(&tmp.color, pixel_size, 1, file)) return image_error_io;
			image->pixels[y * image->width + x] = (Color32) {
				.red = tmp.color.blue,
				.green = tmp.color.green,
				.blue = tmp.color.red,
				.alpha = tmp.color.alpha,
			};
			if (x == (flip_x ? 0 : image->width - 1)) break;
			flip_x ? x-- : x++;
		}
		if (fseek(file, line_padding_size, SEEK_CUR)) return image_error_io;
		if (y == (flip_y ? 0 : image->height - 1)) break;
		flip_y ? y-- : y++;
	}

	return image_error_ok;
}


static ImageError bmp_info_get(BmpInfo *info, FILE *file)
{
#define READ_VAR(var, file) fread(&var, sizeof(var), 1, file)
	if (fseek(file, 0, SEEK_SET)) return image_error_io;
	BmpFileHeader fh;
	if (!READ_VAR(fh, file)) return image_error_io;

	if (fh.signature[0] != 'B' || 
		fh.signature[1] != 'M' || 
		fh.reserved[0] || 
		fh.reserved[1]) return image_error_invalid;

	uint32_t size;
	if (!READ_VAR(size, file)) return image_error_io;
	switch(size) 
	{
		case 40: {
			if (!READ_VAR(info->image_header.size40, file)) return image_error_io;
		} break;
		default: return image_error_unsupported;
	}

	info->image_header_size = size;
	info->file_header = fh;
	return image_error_ok;
#undef READ_VAR
}

static ImageError image_bmp_from_file(Image *image, FILE *file)
{
	BmpInfo info;
	ImageError err = bmp_info_get(&info, file);
	if (err != image_error_ok) return err;

	switch(info.image_header_size) 
	{
		case 40: {
			BmpImageHeader40 ih = info.image_header.size40;
			int flip_x = ih.width < 0;
			int flip_y = ih.height < 0;
			int32_t width = labs(ih.width);
			int32_t height = labs(ih.height);

			if (ih.compression != 0) return image_error_unsupported;
			if (ih.bit_count != 24 && ih.bit_count != 32) return image_error_unsupported;


			if (SIZE_MAX / width / height < (int32_t)sizeof(Color32)) return image_error_alloc;
			Image image = {
				.width = (size_t)width,
				.height = (size_t)height,
				.pixels = malloc((size_t)width * (size_t)height * sizeof(Color32)),
			};
			if (!image.pixels) return image_error_alloc;
			
			err = bmp_load_pixels_uncompressed_u24_or_u32(
				file,
				info.file_header.pixel_data_offset,
				&image,
				(size_t)((ih.bit_count + 7) / 8),
				flip_x,
				flip_y);
			if (err != image_error_ok) 
			{
				free(image.pixels);
				return err;
			}
		} break;
		default: return image_error_unsupported;
	}

	return image_error_ok;
}

ImageError image_load_bmp(Image *image, const char *filepath)
{
	if (!image) return image_error_invalid;
	FILE* file = fopen(filepath, "rb");
	if (!file) return image_error_io;
	Image tmp;
	ImageError err = image_bmp_from_file(&tmp, file);
	fclose(file);
	if (err) return err;
	*image = tmp;
	return image_error_ok;
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
//static bool store_bmp_to_file(FILE* file, const BmpImage* Image) {
//	const uint32_t iamge_header_size = 40;
//	if (sizeof(size_t) > sizeof(uint32_t) && 
//		(
//			(size_t)UINT32_MAX < Image->width ||
//			(size_t)UINT32_MAX < Image->height ||
//			(size_t)UINT32_MAX < Image->pixel_size
//		)
//	) return false;
//	const uint32_t width = (uint32_t)Image->width;
//	const uint32_t height = (uint32_t)Image->height;
//	const uint32_t byte_count = (uint32_t)Image->pixel_size;
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
//			Image->pixel_array, 
//			Image->width, 
//			Image->height, 
//			Image->pixel_size, 
//			file
//		)
//	) return false;
//
//	return true;
//}
