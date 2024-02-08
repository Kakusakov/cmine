#pragma once
#include "fixed_types.h"

gl_handle load_shader(const char* filename, gl_enum shader_type);
gl_handle link_shader_program(gl_handle vertext_shader, gl_handle fragment_shader);
gl_handle load_shader_program(
	const char* vertext_shader_filename,
	const char* fragment_shader_filename
);
