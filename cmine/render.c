#include "render.h"
#include "glad.h"

#include "safety.h"

#include <stdio.h>

static char* read_file(const char* filename) {
	FILE* f = fopen(filename, "rb");
	if (!f) goto err0;
	if (fseek(f, 0, SEEK_END)) goto err1;

	size_t size = ftell(f);
	if (fseek(f, 0, SEEK_SET)) goto err1;

	char* buf = smalloc(size + 1);
	if (!fread(buf, size, 1, f)) goto err2;

	buf[size] = 0;
	fclose(f);
	return buf;

err2:
	sfree(buf);
err1:
	fclose(f);
err0:
	return NULL;
}

gl_handle load_shader(const char* filename, gl_enum shader_type) {
	gl_handle shader = glCreateShader(shader_type);
	if (!shader) goto err0;

	char* source = read_file(filename);
	//printf("%s", source);
	if (!source) goto err1;

	glShaderSource(shader, 1, &source, NULL);
	glCompileShader(shader);
	int32_t success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) goto err2;

	return shader;

err2:
	int32_t size;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &size);
	char* log = smalloc(size);
	glGetShaderInfoLog(shader, size, NULL, log);
	fprintf(
		stderr,
		"\nCaught runtime error:\n"
		"\tOpenGL failed to compile a shader.\n"
		"\tShader type = `0x%x`\n"
		"\tShader filename = `%s`\n"
		"\tInfo log = `%s`\n"
		"\tResuming...",
		shader_type,
		filename,
		log
	);

	sfree(source);
err1:
	glDeleteShader(shader);
err0:
	return 0;
}

gl_handle link_shader_program(gl_handle vertext_shader, gl_handle fragment_shader) {
	gl_handle prog = glCreateProgram();
	if (!prog) goto err0;

	glAttachShader(prog, vertext_shader);
	glAttachShader(prog, fragment_shader);
	glLinkProgram(prog);
	int32_t success;
	glGetProgramiv(prog, GL_LINK_STATUS, &success);
	if (!success) goto err1;

	return prog;

err1:
	int32_t size;
	glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &size);
	char* log = smalloc(size);
	glGetProgramInfoLog(prog, size, NULL, log);
	fprintf(
		stderr,
		"\nCaught runtime error:\n"
		"\tOpenGL failed to link a shader program.\n"
		"\tInfo log = `%s`\n"
		"\tResuming...",
		log
	);

	glDeleteProgram(prog);
err0:
	return 0;
}

gl_handle load_shader_program(
	const char* vertext_shader_filename, 
	const char* fragment_shader_filename) 
{
	gl_handle vsh = load_shader(vertext_shader_filename, GL_VERTEX_SHADER);
	if (!vsh) goto err0;
	gl_handle fsh = load_shader(fragment_shader_filename, GL_FRAGMENT_SHADER);
	if (!fsh) goto err1;
	gl_handle prog = link_shader_program(vsh, fsh);
	if (!prog) goto err2;

	glDeleteShader(fsh);
	glDeleteShader(vsh);

	return prog;
err2:
	glDeleteShader(fsh);
err1:
	glDeleteShader(vsh);
err0:
	return 0;
}
