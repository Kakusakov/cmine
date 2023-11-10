#pragma once
#include "glad.h"

GLuint load_shader(const char* filename, GLenum shader_type);
GLuint link_shader_program(GLuint vertext_shader, GLuint fragment_shader);
GLuint load_shader_program(
	char* vertext_shader_filename,
	char* fragment_shader_filename
);
