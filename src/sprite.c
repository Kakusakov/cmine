#include "sprite.h"
#include "image.h"
#include "safety.h"
#include "glad.h"
#include <stdlib.h>

gl_handle load_pixel_texture(const char* filename) {
	Arena* temp = arena_init();
	gl_handle texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	Image* image = image_load_bmp(filename, temp);
	if (!image) {
		arena_deinit(temp);
		glDeleteTextures(1, &texture);
		return 0;
	}
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RGB,
		image->width,
		image->height,
		0, 
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		&image->pixels
	);
	arena_deinit(temp);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	return texture;
}

gl_handle create_ui_vao(UIVertices vertices) {
	static const uint32_t indices[] = {
		0, 1, 3,
		1, 2, 3
	};
	gl_handle vao;
	glGenVertexArrays(1, &vao);
	gl_handle vbo;
	gl_handle ebo;
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float32_t), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float32_t), (void*)(3 * sizeof(float32_t)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);
	return vao;
}
