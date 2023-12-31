#include "main_menu.h"

#include "mathf.h"
#include "sprite.h"
#include "safety.h"
#include "sprite.h"
#include "render.h"
#include "arena.h"
#include "gl_error.h"
#include "glad.h"

struct PerspectiveSettings {
	float32_t fov_y_radians;
	float32_t near;
	float32_t far;
};
typedef struct PerspectiveSettings PerspectiveSettings;

struct View {
	Vec3f pos;
	Vec3f look;
};
typedef struct View View;

struct PerspectiveCamera {
	PerspectiveSettings settings;
	View View;
};
typedef struct PerspectiveCamera PerspectiveCamera;

struct MainMenu {
	float32_t last_frame_time;
	PerspectiveCamera cam;
	UISprite sprite;
};
typedef struct MainMenu MainMenu;

static MainMenu* create_main_menu(Arena* arena, Input input) {
	MainMenu* self = arena_alloc(arena, 1, MainMenu);
	self->last_frame_time = (float32_t)input.time.seconds;

	const UIVertices vertices = { {
			// positions          // texture coords
			{  0.5f,  0.5f, 0.0f,   1.0f, 1.0f },   // top right
			{  0.5f, -0.5f, 0.0f,   1.0f, 0.0f },   // bottom right
			{ -0.5f, -0.5f, 0.0f,   0.0f, 0.0f },   // bottom left
			{ -0.5f,  0.5f, 0.0f,   0.0f, 1.0f },   // top left 
		} };
	self->cam.View.pos = (Vec3f){ -0.4f, 0.0f, -0.0f };
	self->cam.View.look = vec3f_forward();
	self->cam.settings.far = 100.0f;
	self->cam.settings.near = 0.1f;
	self->cam.settings.fov_y_radians = to_radians(75.0f);

	try(self->sprite.prog = load_shader_program("ui_vsh.glsl", "ui_fsh.glsl"));
	try(self->sprite.vao = create_ui_vao(vertices));
	try(self->sprite.texture = load_pixel_texture("test.bmp"));
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	return self;
}

static void destroy_main_menu(MainMenu* self) {
	glDeleteProgram(self->sprite.prog);
	glDeleteTextures(1, &self->sprite.texture);
	glDeleteVertexArrays(1, &self->sprite.vao);
}

static void update_main_menu(MainMenu* self, Input input) {
	float32_t time = (float32_t)input.time.seconds;
	float32_t delta = time - self->last_frame_time;

	Vec3f dir = { 0, 0, 0 };
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
	self->cam.View.pos.x += dir.x * delta;
	self->cam.View.pos.y += dir.y * delta;
	self->cam.View.pos.z += dir.z * delta;

	Mat4x4 transform = mat4x4_matmul(
		mat4x4_matmul(
			mat4x4_matmul(
				mat4x4_identity(),
				mat4x4_look_at(
					(Vec3f) { sinf(time), 0.0f, cosf(time) },
					(Vec3f) { 0, 0, 0 },
					vec3f_up()
				)
			),
			mat4x4_translate(self->cam.View.pos)
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
		MainMenu* MainMenu = create_main_menu(arena, app_input(app));
		app_update(app);
		while (!app_should_close(app)) {
			update_main_menu(MainMenu, app_input(app));
			app_update(app);
		}
		destroy_main_menu(MainMenu);
	}

	app_unbind_current_context();
	app_deinit(app);
	arena_deinit(arena);
}
