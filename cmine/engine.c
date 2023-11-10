#include "engine.h"

//#define CMINE_IMAGE_EDITOR

#ifdef CMINE_IMAGE_EDITOR

#include "safety.h"
#include "gl_error.h"
#include "sprite.h"
#include "render.h"
#include "transform.h"

static UiSprite sprite;
static Mat4x4f transform;

void on_engine_start(WindowInfo window) {
	setup_gl_error_callback();
	static const UiVertices vertices = { {
			// positions          // texture coords
			{  0.5f,  0.5f, 0.0f,   1.0f, 1.0f },   // top right
			{  0.5f, -0.5f, 0.0f,   1.0f, 0.0f },   // bottom right
			{ -0.5f, -0.5f, 0.0f,   0.0f, 0.0f },   // bottom left
			{ -0.5f,  0.5f, 0.0f,   0.0f, 1.0f },   // top left 
		} };
	transform = mat4xf_perspective((float)window.width / (float)window.height, 3.14f / 1.0f, 0.1f, 100.0f);
	//transform = mat4x4f_ortho(0.0f, 800.0f, 0.0f, 600.0f, -100.0f, 100.0f);
	transform = mat4x4f_matmul(transform, mat4x4f_translate((Vec3f) { 0.0f, 0.0f, -10.0f }));
	//transform = mat4x4f_matmul(transform, mat4x4f_rotate_axis_x(3.14 / 2));
	TRY(sprite.prog = load_shader_program("ui_vsh.glsl", "ui_fsh.glsl"));
	TRY(sprite.vao = create_ui_vao(vertices));
	TRY(sprite.texture = load_pixel_texture("test.bmp"));
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

void on_frame_update(WindowInfo window) {
	transform = mat4x4f_matmul(transform, mat4x4f_rotate_axis_x(0.001));
	transform = mat4x4f_matmul(transform, mat4x4f_rotate_axis_y(0.0005));
	transform = mat4x4f_matmul(transform, mat4x4f_rotate_axis_z(0.0003));

	glViewport(0, 0, window.width, window.height);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(sprite.prog);

	GLuint location;
	TRY((location = glGetUniformLocation(sprite.prog, "transform")) != -1);
	glUniformMatrix4fv(location, 1, GL_FALSE, (const GLfloat*)&transform);

	glBindTexture(GL_TEXTURE_2D, sprite.texture);
	glBindVertexArray(sprite.vao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

#else

#include "safety.h"
#include "gl_error.h"
#include "sprite.h"
#include "render.h"
#include "transform.h"

#include <stdlib.h>

struct Engine {
	UiSprite sprite;
	Mat4x4f transform;
};
typedef struct Engine Engine;

void on_engine_startup(void** user_pointer, Settings settings) {
	setup_gl_error_callback();
	Engine* engine = smalloc(sizeof(Engine));
	*user_pointer = engine;

	const UiVertices vertices = { {
		// positions          // texture coords
		{  0.5f,  0.5f, 0.0f,   1.0f, 1.0f },   // top right
		{  0.5f, -0.5f, 0.0f,   1.0f, 0.0f },   // bottom right
		{ -0.5f, -0.5f, 0.0f,   0.0f, 0.0f },   // bottom left
		{ -0.5f,  0.5f, 0.0f,   0.0f, 1.0f },   // top left 
	} };
	
	TRY(engine->sprite.prog = load_shader_program("ui_vsh.glsl", "ui_fsh.glsl"));
	TRY(engine->sprite.vao = create_ui_vao(vertices));
	TRY(engine->sprite.texture = load_pixel_texture("test.bmp"));
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

void on_engine_update(void** user_pointer, Settings settings) {
	Engine* engine = *user_pointer;
	engine->transform = mat4x4f_matmul(
		mat4x4f_matmul(
			mat4x4f_identity_mat(),
			mat4x4f_look_at_mat(
				vec3f_new(0.0f, 0.0f, 1.0f),
				vec3f_zero(),
				vec3f_up()
			)
		),
		mat4x4f_perspective_mat(
			(float)settings.window.width / (float)settings.window.height,
			to_radians(85.0f),
			0.1f,
			100.0f
		)
	);

	glViewport(0, 0, settings.window.width, settings.window.height);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(engine->sprite.prog);

	GLuint location;
	TRY((location = glGetUniformLocation(engine->sprite.prog, "transform")) != -1);
	glUniformMatrix4fv(location, 1, GL_FALSE, (const GLfloat*)&engine->transform);

	glBindTexture(GL_TEXTURE_2D, engine->sprite.texture);
	glBindVertexArray(engine->sprite.vao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void on_engine_shutdown(void** user_pointer, Settings settings) {
	free(*user_pointer);
	*user_pointer = NULL;
}

#endif
