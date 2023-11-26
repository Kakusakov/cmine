#pragma once
#include "fixed_types.h"
#include "glad.h"

struct UiSprite {
	GLuint vao;
	GLuint prog;
	GLuint texture;
};
typedef struct UiSprite UiSprite;

struct UiVertex {
	float32_t x;
	float32_t y;
	float32_t z;
	float32_t uv_x;
	float32_t uv_y;
};
typedef struct UiVertex UiVertex;

struct UiVertices {
	UiVertex values[4];
};
typedef struct UiVertices UiVertices;

GLuint load_pixel_texture(const char* filename);
GLuint create_ui_vao(UiVertices vertices);
