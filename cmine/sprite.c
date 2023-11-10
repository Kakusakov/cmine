#include "sprite.h"

#include "image.h"

#include <stdlib.h>

GLuint load_pixel_texture(const char* filename) {
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	BmpImage image = load_bmp_image(filename);
	if (!image.pixel_array) {
		glDeleteTextures(1, &texture);
		return 0;
	}
	glTexImage2D(
		GL_TEXTURE_2D, 
		0, 
		GL_RGB, 
		image.width, 
		image.height,
		0, 
		image.texture_format, 
		image.texture_type,
		image.pixel_array
	);
	free_bmp_image(&image);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	return texture;
}

GLuint create_ui_vao(UiVertices vertices) {
	static const unsigned int indices[] = {
		0, 1, 3,
		1, 2, 3
	};
	GLuint vao;
	glGenVertexArrays(1, &vao);
	GLuint vbo;
	GLuint ebo;
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);
	return vao;
}