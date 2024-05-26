#include "app.h"
#include "chunk.h"
#include "input.h"
#include <stdlib.h>

/*static void main_menu_run(void) {
	GLuint texture = render_tmp_texture();
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	Vec3f pos = {1.0f, -0.5f, 0.0f};
	float fov_z_radians = 75.0f * DEG_TO_RAD;
	float near = 0.1f;
	float far = 100.0f;

	app_update();
	float time = app_time();
	while (!app_should_close())
	{
		float dt = app_time() - time;
		if (dt < 0) dt = 0;
		if (dt > 0.1f) dt = 0.1f;
		time += dt;

		if (app_is_key_pressed(app_key_w)) pos.x -= dt;
		if (app_is_key_pressed(app_key_s)) pos.x += dt;
		if (app_is_key_pressed(app_key_d)) pos.y -= dt;
		if (app_is_key_pressed(app_key_a)) pos.y += dt;


		Mat transform = MAT_IDENTITY;
		Vec3f from = {cosf(time), sinf(time), 0};
		Vec3f to = {0};
		Vec3f up = {0, 0, 1};
		transform = mat_look_at(transform, from, to, up);
		transform = mat_translate(transform, pos);
		int width = app_window_width();
		int height = app_window_height();
		float aspect = (float)width / (float)height;
		transform = mat_perspective(transform, aspect, fov_z_radians, near, far);

		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT);
		render_draw_quad(texture, transform);
		app_swap_buffers();

		app_update();
	}
}*/

void world_run(void) {
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	Chunks chunks;
	chunks_init(&chunks, (CPos){0}, 3);

	int should_generate_chunk = 1;
	uint32_t seed = 0;

	GLuint texture = render_tmp_texture();
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	Vec3 pos = p2gl((Vec3){1.0f, 0.0f, -1.5f});
	float fov_z_radians = 75.0f * DEG_TO_RAD;
	float near = 0.1f;
	float far = 100.0f;

	float mouse_speed = 0.001f;
	float move_speed = 3.0f;
	float h_ang = 0;
	float v_ang = 0;

	app_hide_cursor();

	app_update();
	KeyBuffer keys;
	key_buffer_reset(&keys);
	float time = app_time();
	float mouse_x = app_cursor_position_x();
	float mouse_y = app_cursor_position_y();
	while (!app_should_close())
	{
		if (should_generate_chunk)
		{
			chunks_unload(&chunks);
			Perlin* perlin = malloc(sizeof(Perlin));
			perlin_init(perlin, seed);
			Fbm fbm = {
				.octave_count = 1,
				.frequency = 0.2,
				.intensity = 8,
				.persistance = 1,
				.lacunarity = 1,
			};
			chunks_generate_blocks(&chunks, perlin, fbm, (BPos){0});
			chunks_generate_mesh(&chunks, (AdjacentChunks){0});
			free(perlin);
			should_generate_chunk = 0;
			seed++;
		}

		float dt = app_time() - time;
		if (dt < 0) dt = 0;
		if (dt > 0.1f) dt = 0.1f;
		time += dt;
		float mouse_dx = app_cursor_position_x() - mouse_x;
		float mouse_dy = app_cursor_position_y() - mouse_y;
		mouse_x += mouse_dx;
		mouse_y += mouse_dy;

		Vec3 up = p2gl((Vec3){0, 0, 1});
		Vec3 dir = p2gl((Vec3){-cosf(h_ang), -sinf(h_ang), 0});
		Vec3 left = vcross(dir, up);
		
		float move = move_speed * dt;
		if (key_buffer_is_key_pressed(&keys, app_key_w)) pos = vadd(pos, vmul(dir, vsplat(move)));
		if (key_buffer_is_key_pressed(&keys, app_key_s)) pos = vsub(pos, vmul(dir, vsplat(move)));
		if (key_buffer_is_key_pressed(&keys, app_key_a)) pos = vsub(pos, vmul(left, vsplat(move)));
		if (key_buffer_is_key_pressed(&keys, app_key_d)) pos = vadd(pos, vmul(left, vsplat(move)));
		if (key_buffer_is_key_pressed(&keys, app_key_left_shift)) pos = vadd(pos, vmul(up, vsplat(move)));
		if (key_buffer_is_key_pressed(&keys, app_key_space)) pos = vsub(pos, vmul(up, vsplat(move)));
		if (key_buffer_is_key_down(&keys, app_key_g)) should_generate_chunk = 1;

		if (!app_is_window_focused() || key_buffer_is_key_down(&keys, app_key_esc)) app_show_cursor();
		if (app_is_cursor_hovered() && key_buffer_is_key_down(&keys, app_key_mouse_left)) app_hide_cursor();
		if (app_is_cursor_hidden()) 
		{
			h_ang += mouse_dx * mouse_speed;
			v_ang += mouse_dy * mouse_speed;
		}


		int width = app_window_width();
		int height = app_window_height();
		float aspect = (float)width / (float)height;

		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Camera cam = {
			.pos = pos, 
			.dir = dir, 
			.up = up
		};
		Perspective p = {
			.fov_z_rad = fov_z_radians,
			.aspect = aspect,
			.near = near,
			.far = far
		};
		chunks_draw(&chunks, cam, p);
		app_swap_buffers();

		app_update();
		key_buffer_update(&keys);
	}

	chunks_deinit(&chunks);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
}


int main(void) {
	if (!app_init()) return 1;
	if (!render_init()) return 1;

	app_update();
	if (!app_should_close()) world_run();

	app_deinit();
	return 0;
}
