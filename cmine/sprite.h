#pragma once
#include "fixed_types.h"

struct ui_sprite {
	gl_handle vao;
	gl_handle prog;
	gl_handle texture;
};
typedef struct ui_sprite ui_sprite;

struct ui_vertex {
	float32_t x;
	float32_t y;
	float32_t z;
	float32_t uv_x;
	float32_t uv_y;
};
typedef struct ui_vertex ui_vertex;

struct ui_vertices {
	ui_vertex values[4];
};
typedef struct ui_vertices ui_vertices;

gl_handle load_pixel_texture(const char* filename);
gl_handle create_ui_vao(ui_vertices vertices);
