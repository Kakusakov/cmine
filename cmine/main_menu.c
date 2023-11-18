#include "main_menu.h"

#include "mat_math.h"
#include "sprite.h"
#include "safety.h"
#include "sprite.h"
#include "render.h"

#include <stdlib.h>

struct PerspectiveSettings {
	float fov_y_radians;
	float near;
	float far;
};
typedef struct PerspectiveSettings PerspectiveSettings;

struct View {
	Vec3f position;
	Vec3f look_dir;
};
typedef struct View View;

struct PerspectiveCamera {
	PerspectiveSettings settings;
	View view;
};
typedef struct PerspectiveCamera PerspectiveCamera;

struct MainMenu {
	float last_frame_time;
	PerspectiveCamera cam;
	UiSprite sprite;
};
typedef struct MainMenu MainMenu;

void destroy_main_menu(MainMenu* self) {
	glDeleteProgram(self->sprite.prog);
	glDeleteTextures(1, &self->sprite.texture);
	glDeleteVertexArrays(1, &self->sprite.vao);
}

void update_main_menu(App* app) {
	app_update(app);
	if (app_should_close(app)) {
		destroy_main_menu(app_active_scene(app)->self);
		app_active_scene(app)->self = NULL;
		return;
	}
	Input input = *app_input(app);
	MainMenu* self = app_active_scene(app)->self;
	float time = (float)input.time.seconds;
	float delta = time - self->last_frame_time;

	Vec3f pos = self->cam.view.position;
	if (input.keys.is_w_pressed) {
		pos = vec3f_vecadd(pos, vec3f_mul(vec3f_forward(), delta));
	}
	if (input.keys.is_s_pressed) {
		pos = vec3f_vecsub(pos, vec3f_mul(vec3f_forward(), delta));
	}
	if (input.keys.is_a_pressed) {
		pos = vec3f_vecadd(pos, vec3f_mul(vec3f_right(), delta));
	}
	if (input.keys.is_d_pressed) {
		pos = vec3f_vecsub(pos, vec3f_mul(vec3f_right(), delta));
	}
	self->cam.view.position = pos;

	Mat4x4f transform = mat4x4f_matmul(
		mat4x4f_matmul(
			mat4x4f_matmul(
				mat4x4f_identity_mat(),
				mat4x4f_look_at_mat(
					vec3f_new(sinf(time), 0.0f, cosf(time)),
					vec3f_zero(),
					vec3f_up()
				)
			),
			mat4x4f_translate_mat(self->cam.view.position)
		),
		mat4x4f_perspective_mat(
			(float)input.window.width / (float)input.window.height,
			self->cam.settings.fov_y_radians,
			self->cam.settings.near,
			self->cam.settings.far
		)
	);

	glViewport(0, 0, input.window.width, input.window.height);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(self->sprite.prog);

	GLuint location;
	TRY((location = glGetUniformLocation(self->sprite.prog, "transform")) != -1);
	glUniformMatrix4fv(location, 1, GL_FALSE, (const GLfloat*)&transform);

	glBindTexture(GL_TEXTURE_2D, self->sprite.texture);
	glBindVertexArray(self->sprite.vao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	self->last_frame_time = (float)input.time.seconds;
}

Scene create_main_menu(App* app) {
	MainMenu* self = smalloc(sizeof(MainMenu));
	Input input = *app_input(app);
	self->last_frame_time = (float)input.time.seconds;

	const UiVertices vertices = { {
			// positions          // texture coords
			{  0.5f,  0.5f, 0.0f,   1.0f, 1.0f },   // top right
			{  0.5f, -0.5f, 0.0f,   1.0f, 0.0f },   // bottom right
			{ -0.5f, -0.5f, 0.0f,   0.0f, 0.0f },   // bottom left
			{ -0.5f,  0.5f, 0.0f,   0.0f, 1.0f },   // top left 
		} };
	self->cam.view.position = vec3f_new(-0.4f, 0.0f, -0.0f);
	self->cam.view.look_dir = vec3f_forward();
	self->cam.settings.far = 100.0f;
	self->cam.settings.near = 0.1f;
	self->cam.settings.fov_y_radians = to_radians(75.0f);

	TRY(self->sprite.prog = load_shader_program("ui_vsh.glsl", "ui_fsh.glsl"));
	TRY(self->sprite.vao = create_ui_vao(vertices));
	TRY(self->sprite.texture = load_pixel_texture("test.bmp"));
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	return (Scene) {
		.self = self,
		.update = update_main_menu,
	};
}
