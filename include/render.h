#pragma once
#include "mathf.h"
#include "glad.h"
#include "image.h"

GLuint load_pixel_texture(const Image *image);

int render_init(void);
void render_draw_quad(GLuint texture, Mat transform);
GLuint render_tmp_texture(void);
