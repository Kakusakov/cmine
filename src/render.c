#include "render.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "app.h"
#define CMINE_ENABLE_GL_DEBUG

#ifdef CMINE_ENABLE_GL_DEBUG
static const char *gl_message_source_name(GLenum source)
{
	switch (source) {
	case GL_DEBUG_SOURCE_API:             return "GL_DEBUG_SOURCE_API";
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   return "GL_DEBUG_SOURCE_WINDOW_SYSTEM";
	case GL_DEBUG_SOURCE_SHADER_COMPILER: return "GL_DEBUG_SOURCE_SHADER_COMPILER";
	case GL_DEBUG_SOURCE_THIRD_PARTY:     return "GL_DEBUG_SOURCE_THIRD_PARTY";
	case GL_DEBUG_SOURCE_APPLICATION:     return "GL_DEBUG_SOURCE_APPLICATION";
	case GL_DEBUG_SOURCE_OTHER:           return "GL_DEBUG_SOURCE_OTHER";
	default:                              return "[unknown OpenGL message source]";
	}
}

static const char *gl_message_type_name(GLenum type)
{
	switch (type) {
	case GL_DEBUG_TYPE_ERROR:               return "GL_DEBUG_TYPE_ERROR";
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR";
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  return "GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR";
	case GL_DEBUG_TYPE_PORTABILITY:         return "GL_DEBUG_TYPE_PORTABILITY";
	case GL_DEBUG_TYPE_PERFORMANCE:         return "GL_DEBUG_TYPE_PERFORMANCE";
	case GL_DEBUG_TYPE_MARKER:              return "GL_DEBUG_TYPE_MARKER";
	case GL_DEBUG_TYPE_PUSH_GROUP:          return "GL_DEBUG_TYPE_PUSH_GROUP";
	case GL_DEBUG_TYPE_POP_GROUP:           return "GL_DEBUG_TYPE_POP_GROUP";
	case GL_DEBUG_TYPE_OTHER:               return "GL_DEBUG_TYPE_OTHER";
	default:                                return "[unknown OpenGL message type]";
	}
}

static const char *gl_message_severity_name(GLenum severity)
{
	switch (severity) {
	case GL_DEBUG_SEVERITY_HIGH:         return "GL_DEBUG_SEVERITY_HIGH";
	case GL_DEBUG_SEVERITY_MEDIUM:       return "GL_DEBUG_SEVERITY_MEDIUM";
	case GL_DEBUG_SEVERITY_LOW:          return "GL_DEBUG_SEVERITY_LOW";
	case GL_DEBUG_SEVERITY_NOTIFICATION: return "GL_DEBUG_SEVERITY_NOTIFICATION";
	default:                             return "[unknown OpenGL message severity]";
	}
}

static void GLAPIENTRY gl_message_callback(
	GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLint length,
	const GLchar *message,
	const void *userParam)
{
	fprintf(
		stderr,
		"\nRecieved info:\n"
		"\tOpenGL invoked the message callback.\n"
		"\tMessage source = `%s (0x%x)`\n"
		"\tMessage type = `%s (0x%x)`\n"
		"\tMessage severity = `%s (0x%x)`\n"
		"\tMessage id = `%u`\n"
		"\tMessage = `%.*s`\n"
		"\tResuming...\n",
		gl_message_source_name(source),
		source,
		gl_message_type_name(type),
		type,
		gl_message_severity_name(severity),
		severity,
		id,
		length,
		message
	);
}
#endif  // CMINE_ENABLE_GL_DEBUG

static void setup_gl_error_callback(void)
{
#ifdef CMINE_ENABLE_GL_DEBUG
	if (GLVersion.major < 4) return;
	if (GLVersion.major == 4 && GLVersion.minor < 4) return;
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(gl_message_callback, NULL);
#endif  // CMINE_ENABLE_GL_DEBUG
}

static char *read_entire_file(const char* filename) 
{
	FILE *f = fopen(filename, "rb");
	if (!f) goto err0;
	if (fseek(f, 0, SEEK_END)) goto err1;

	size_t size = ftell(f);
	if (fseek(f, 0, SEEK_SET)) goto err1;

	char *buf = malloc(size + 1);
	if (!buf) goto err1;
	if (!fread(buf, size, 1, f)) goto err2;

	buf[size] = 0;
	fclose(f);
	return buf;

err2:
	free(buf);
err1:
	fclose(f);
err0:
	return NULL;
}

static GLuint load_shader(const char* filename, GLuint shader_type)
{
	GLuint shader = glCreateShader(shader_type);
	if (!shader) 
	{
		fprintf(
			stderr,
			"\nCaught runtime error:\n"
			"\tOpenGL failed to create a shader.\n"
			"\tshader_type = `0x%x`\n",
			shader_type
		);
		goto err0;
	}

	char *source = read_entire_file(filename);
	if (!source)
	{
		fprintf(
			stderr,
			"\nCaught runtime error:\n"
			"\tUnable to locate shader.\n"
			"\tpath = `%s`\n",
			filename
		);
		goto err1;
	}

	const char *const tmp = source;  // ???
	glShaderSource(shader, 1, &tmp, NULL);
	glCompileShader(shader);
	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	free(source);
	if (!success) 
	{
		GLint size = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &size);
		char *log = malloc(size);
		glGetShaderInfoLog(shader, size, NULL, log);
		fprintf(
			stderr,
			"\nCaught runtime error:\n"
			"\tOpenGL failed to compile a shader.\n"
			"\tShader type = `0x%x`\n"
			"\tShader filename = `%s`\n"
			"\tInfo log = `%s`\n",
			shader_type,
			filename,
			log);
		free(log);
		goto err1;
	}

	return shader;
err1:
	glDeleteShader(shader);
err0:
	return 0;
}

static GLuint link_shader_program(GLuint vertex, GLuint fragment)
{
	GLuint prog = glCreateProgram();
	if (!prog) 
	{
		fprintf(
			stderr,
			"\nCaught runtime error:\n"
			"\tOpenGL failed to create a shader program.\n");
		goto err0;
	}

	glAttachShader(prog, vertex);
	glAttachShader(prog, fragment);
	glLinkProgram(prog);
	GLint success;
	glGetProgramiv(prog, GL_LINK_STATUS, &success);
	if (!success) 
	{
		GLint size = 0;
		glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &size);
		char *log = malloc(size);
		const char *print_log;
		if (log) 
		{
			glGetProgramInfoLog(prog, size, NULL, log);
			print_log = log;
		}
		else
		{
			print_log = "[unable to allocate info log]";
		}
		fprintf(
			stderr,
			"\nCaught runtime error:\n"
			"\tOpenGL failed to link a shader program.\n"
			"\tInfo log = `%s`\n",
			print_log);
		free(log);
		goto err1;
	}

	return prog;
err1: 
	glDeleteProgram(prog);
err0:
	return 0;
}

static GLuint load_shader_program(
	const char* vertext_filename, 
	const char* fragment_filename) 
{
	GLuint vsh = load_shader(vertext_filename, GL_VERTEX_SHADER);
	if (!vsh) goto err0;
	GLuint fsh = load_shader(fragment_filename, GL_FRAGMENT_SHADER);
	if (!fsh) goto err1;
	GLuint prog = link_shader_program(vsh, fsh);
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

GLuint load_pixel_texture(const Image *image)
{
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RGB,
		image->width,
		image->height,
		0, 
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		image->pixels
	);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	return texture;
}

static struct 
{
	struct
	{
		GLuint vao;
		GLuint prog;
	} quad;
	GLuint tmp_texture;
	GLuint chunk_shader_prog;
} render;

int render_init(void)
{
	// Initialize glad.
	{
		if (!gladLoadGLLoader(app_context_gl_loader()))
		{
			fprintf(stderr, "Failed to load OpenGL context.\n");
			goto err_glad;
		}
		setup_gl_error_callback();
	}

	// Initialize tmp_texture.
	{
		Color32 tmp_colors[4] = {
			COLOR32_BLUE, COLOR32_GREEN,
			COLOR32_PINK, COLOR32_BLACK,
		};
		const Image image = {
			.width = 2,
			.height = 2,
			.pixels = &tmp_colors,
		};
		render.tmp_texture = load_pixel_texture(&image);
	}

	// Initialize chunk_shader_prog.
	{

		const char* vertex_src = "./resources/chunk_vsh.glsl";
		const char* fragment_src = "./resources/chunk_fsh.glsl";
		render.chunk_shader_prog = load_shader_program(vertex_src, fragment_src);
		if (!render.chunk_shader_prog) goto err_chunk_shader_prog;
	}

	// Initialize quad.
	{
		const char *vertex_src = "./resources/quad_vsh.glsl";
		const char *fragment_src = "./resources/quad_fsh.glsl";
		render.quad.prog = load_shader_program(vertex_src, fragment_src);
		if (!render.quad.prog) goto err_quad_prog;

		const uint32_t indices[] = {
			0, 1, 3,
			1, 2, 3,
		};
		const float vertices[4][5] = {
			// positions         // texture coords
			{  1.0f,  1.0f, 0.0f,   1.0f, 1.0f },   // top right
			{  1.0f, -1.0f, 0.0f,   1.0f, 0.0f },   // bottom right
			{ -1.0f, -1.0f, 0.0f,   0.0f, 0.0f },   // bottom left
			{ -1.0f,  1.0f, 0.0f,   0.0f, 1.0f },   // top left 
		};
		glGenVertexArrays(1, &render.quad.vao);
		GLuint vbo;
		GLuint ebo;
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);

		glBindVertexArray(render.quad.vao);

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
	}
	return 1;
err_all:
	glDeleteVertexArrays(1, &render.quad.vao);
err_quad_vao:
	glDeleteProgram(render.quad.prog);
err_quad_prog:
	glDeleteProgram(render.chunk_shader_prog);
err_chunk_shader_prog:
	glDeleteTextures(1, &render.tmp_texture);
err_tmp_texture:
err_glad:
	return 0;
}

void render_draw_quad(GLuint texture, Mat transform)
{
	glUseProgram(render.quad.prog);

	const char *transform_name = "transform";
	GLuint location = glGetUniformLocation(render.quad.prog, transform_name);
	if (location == -1)
	{
		fprintf(
			stderr,
			"\nCaught runtime error:\n"
			"\nRenderer is unbale to find unform `%s` in quad shader program",
			transform_name);
		return;
	}
	glUniformMatrix4fv(location, 1, GL_FALSE, transform.v);

	glBindTexture(GL_TEXTURE_2D, texture);
	glBindVertexArray(render.quad.vao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

GLuint render_tmp_texture(void)
{
	return render.tmp_texture;
}

GLuint render_chunk_shader_program(void)
{
	return render.chunk_shader_prog;
}
