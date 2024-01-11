#include "main_menu_run.h"

#include "mathf.h"
#include "sprite.h"
#include "safety.h"
#include "render.h"
#include "arena.h"
#include "gl_error.h"
#include "glad.h"
#include "app.h"

struct PerspectiveCamera {
	Vec3f pos;
	Vec3f look;
	float32_t fov_y_radians;
	float32_t near;
	float32_t far;
};
typedef struct PerspectiveCamera PerspectiveCamera;

struct MainMenu {
	float32_t last_frame_time;
	PerspectiveCamera cam;
	UISprite sprite;
};
typedef struct MainMenu MainMenu;

static MainMenu create_main_menu(Input input) {
	UISprite sprite = {0};
	static const UIVertices vertices = {{
		// positions         // texture coords
		{  0.5f,  0.5f, 0.0f,   1.0f, 1.0f },   // top right
		{  0.5f, -0.5f, 0.0f,   1.0f, 0.0f },   // bottom right
		{ -0.5f, -0.5f, 0.0f,   0.0f, 0.0f },   // bottom left
		{ -0.5f,  0.5f, 0.0f,   0.0f, 1.0f },   // top left 
	}};
	
	try(sprite.vao = create_ui_vao(vertices));
	try(sprite.prog = load_shader_program("ui_vsh.glsl", "ui_fsh.glsl"));
	try(sprite.texture = load_pixel_texture("test.bmp"));

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	return (MainMenu) {
		.cam = {
			.pos = (Vec3f){ -0.4f, 0.0f, -0.0f },
			.look = vec3f_forward(),
			.fov_y_radians = to_radians(75.0f),
			.near = 0.1f,
			.far = 100.0f,
		},
		.last_frame_time = input.time.seconds,
		.sprite = sprite
	};
}

static void destroy_main_menu(MainMenu* self) {
	glDeleteProgram(self->sprite.prog);
	glDeleteTextures(1, &self->sprite.texture);
	glDeleteVertexArrays(1, &self->sprite.vao);
}

static void update_main_menu(MainMenu* self, Input input) {
	float32_t time = input.time.seconds;
	float32_t delta = time - self->last_frame_time;

	Vec3f dir = { 0, 0, 0 };
	if (input.keys.is_w_pressed) dir = vec3f_forward();
	if (input.keys.is_s_pressed) dir = vec3f_backward();
	if (input.keys.is_a_pressed) dir = vec3f_right();
	if (input.keys.is_d_pressed) dir = vec3f_left();

	self->cam.pos.x += dir.x * delta;
	self->cam.pos.y += dir.y * delta;
	self->cam.pos.z += dir.z * delta;

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
			mat4x4_translate(self->cam.pos)
		),
		mat4x4_perspective(
			(float32_t)input.window.width / (float32_t)input.window.height,
			self->cam.fov_y_radians,
			self->cam.near,
			self->cam.far
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

void main_menu_run(App* app) {
	MainMenu main_menu = create_main_menu(app_input(app));
	app_update(app);
	while (!app_should_close(app)) {
		update_main_menu(&main_menu, app_input(app));
		app_update(app);
	}
	destroy_main_menu(&main_menu);
}
