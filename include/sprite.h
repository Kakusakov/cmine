#pragma once
#include "fixed_types.h"

struct UISprite {
	gl_handle vao;
	gl_handle prog;
	gl_handle texture;
};
typedef struct UISprite UISprite;

struct UIVertex {
	float32_t x;
	float32_t y;
	float32_t z;
	float32_t uv_x;
	float32_t uv_y;
};
typedef struct UIVertex UIVertex;

struct UIVertices {
	UIVertex values[4];
};
typedef struct UIVertices UIVertices;

gl_handle load_pixel_texture(const char* filename);
gl_handle create_ui_vao(UIVertices vertices);
