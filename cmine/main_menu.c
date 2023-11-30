#include "main_menu.h"

#include "vec.h"
#include "mat.h"
#include "sprite.h"
#include "safety.h"
#include "sprite.h"
#include "render.h"
#include "arena.h"
#include "gl_error.h"
#include "glad.h"

struct perspective_settings {
	float32_t fov_y_radians;
	float32_t near;
	float32_t far;
};
typedef struct perspective_settings perspective_settings;

struct view {
	vec3f pos;
	vec3f look;
};
typedef struct view view;

struct perspective_camera {
	perspective_settings settings;
	view view;
};
typedef struct perspective_camera perspective_camera;

struct main_menu {
	float32_t last_frame_time;
	perspective_camera cam;
	ui_sprite sprite;
};
typedef struct main_menu main_menu;

static main_menu* create_main_menu(Arena* arena, Input input) {
	main_menu* self = arena_alloc(arena, 1, main_menu);
	self->last_frame_time = (float32_t)input.time.seconds;

	const ui_vertices vertices = { {
			// positions          // texture coords
			{  0.5f,  0.5f, 0.0f,   1.0f, 1.0f },   // top right
			{  0.5f, -0.5f, 0.0f,   1.0f, 0.0f },   // bottom right
			{ -0.5f, -0.5f, 0.0f,   0.0f, 0.0f },   // bottom left
			{ -0.5f,  0.5f, 0.0f,   0.0f, 1.0f },   // top left 
		} };
	self->cam.view.pos = (vec3f){ -0.4f, 0.0f, -0.0f };
	self->cam.view.look = vec3f_forward();
	self->cam.settings.far = 100.0f;
	self->cam.settings.near = 0.1f;
	self->cam.settings.fov_y_radians = to_radians(75.0f);

	try(self->sprite.prog = load_shader_program("ui_vsh.glsl", "ui_fsh.glsl"));
	try(self->sprite.vao = create_ui_vao(vertices));
	try(self->sprite.texture = load_pixel_texture("test.bmp"));
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	return self;
}

static void destroy_main_menu(main_menu* self) {
	glDeleteProgram(self->sprite.prog);
	glDeleteTextures(1, &self->sprite.texture);
	glDeleteVertexArrays(1, &self->sprite.vao);
}

static void update_main_menu(main_menu* self, Input input) {
	float32_t time = (float32_t)input.time.seconds;
	float32_t delta = time - self->last_frame_time;

	vec3f dir = { 0, 0, 0 };
	if (input.keys.is_w_pressed) {
		dir = vec3f_forward();
	}
	if (input.keys.is_s_pressed) {
		dir = vec3f_backward();
	}
	if (input.keys.is_a_pressed) {
		dir = vec3f_right();
	}
	if (input.keys.is_d_pressed) {
		dir = vec3f_left();
	}
	self->cam.view.pos.x += dir.x * delta;
	self->cam.view.pos.y += dir.y * delta;
	self->cam.view.pos.z += dir.z * delta;

	mat4x4 transform = mat4x4_matmul(
		mat4x4_matmul(
			mat4x4_matmul(
				mat4x4_identity(),
				mat4x4_look_at(
					(vec3f) { sinf(time), 0.0f, cosf(time) },
					(vec3f) { 0, 0, 0 },
					vec3f_up()
				)
			),
			mat4x4_translate(self->cam.view.pos)
		),
		mat4x4_perspective(
			(float32_t)input.window.width / (float32_t)input.window.height,
			self->cam.settings.fov_y_radians,
			self->cam.settings.near,
			self->cam.settings.far
		)
	);

	glViewport(0, 0, input.window.width, input.window.height);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(self->sprite.prog);

	gl_handle location;
	try((location = glGetUniformLocation(self->sprite.prog, "transform")) != -1);
	glUniformMatrix4fv(location, 1, GL_FALSE, (const float32_t*)&transform);

	glBindTexture(GL_TEXTURE_2D, self->sprite.texture);
	glBindVertexArray(self->sprite.vao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	self->last_frame_time = (float)input.time.seconds;
}

void main_menu_run(void) {
	Arena* arena = arena_init();
	App* app = app_init(arena);
	app_bind_current_context(app);
	try(gladLoadGLLoader(app_get_gl_loader(app)));
	setup_gl_error_callback();

	app_update(app);
	if (!app_should_close(app)) {
		main_menu* main_menu = create_main_menu(arena, app_input(app));
		app_update(app);
		while (!app_should_close(app)) {
			update_main_menu(main_menu, app_input(app));
			app_update(app);
		}
		destroy_main_menu(main_menu);
	}

	app_unbind_current_context();
	app_deinit(app);
	arena_deinit(arena);
}
