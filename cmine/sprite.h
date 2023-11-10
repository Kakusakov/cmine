#pragma once
#include "glad.h"

struct UiSprite {
	GLuint vao;
	GLuint prog;
	GLuint texture;
};
typedef struct UiSprite UiSprite;

struct UiVertex {
	float x;
	float y;
	float z;
	float uv_x;
	float uv_y;
};
typedef struct UiVertex UiVertex;

struct UiVertices {
	UiVertex values[4];
};
typedef struct UiVertices UiVertices;

GLuint load_pixel_texture(const char* filename);
GLuint create_ui_vao(UiVertices vertices);
