#include "render.h"  // THIS MUST BE INCLUDED FIRST
#include "context.h"
#include "input.h"
#include "chunk.h"
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

	bool should_generate_chunk = true;
	u32 seed = 0;

	GLuint texture = render_tmp_texture();
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	Vec3 pos = v3_new(1, 0, -1.5);

	f32 mouse_speed = 0.001f;
	f32 move_speed = 3.0f;
	Angle h_ang = angle_from_radians(0);
	Angle v_ang = angle_from_radians(0);

	context_hide_cursor();
	while (!context_has_close_flag())
	{
		if (should_generate_chunk)
		{
			chunks_unload(&chunks);
			Perlin* perlin = malloc(sizeof(Perlin));
			perlin_init(perlin, seed);
			Fbm fbm = {
				.octave_count = 1,
				.frequency = 0.2f,
				.intensity = 8,
				.persistance = 1,
				.lacunarity = 1,
			};
			chunks_generate_blocks(&chunks, perlin, fbm);
			chunks_generate_mesh(&chunks);
			free(perlin);
			should_generate_chunk = false;
			seed++;
		}

		Vec3 up = v3_new(0, 0, 1);
		Vec3 dir = v3_new(-angle_cos(h_ang), -angle_sin(h_ang), 0);
		Vec3 left = v3_cross(dir, up);
		
		f32 move = move_speed * (f32)delta_time();
		if (is_key_pressed(key_w)) pos = v3_add(pos, v3_scale(dir, move));
		if (is_key_pressed(key_s)) pos = v3_sub(pos, v3_scale(dir, move));
		if (is_key_pressed(key_a)) pos = v3_sub(pos, v3_scale(left, move));
		if (is_key_pressed(key_d)) pos = v3_add(pos, v3_scale(left, move));
		if (is_key_pressed(key_left_shift)) pos = v3_add(pos, v3_scale(up, move));
		if (is_key_pressed(key_space)) pos = v3_sub(pos, v3_scale(up, move));
		if (is_key_down(key_g)) should_generate_chunk = true;

		if (!context_is_window_focused() || is_key_down(key_esc)) context_show_cursor();
		if (context_is_cursor_hovered() && is_key_down(mouse_key_left)) context_hide_cursor();
		if (context_is_cursor_hidden()) {
			h_ang = angle_normal(angle_add(h_ang, angle_from_radians((f32)mouse_dx()*mouse_speed)));
			v_ang = angle_clamp(
				angle_sub(v_ang, angle_from_radians((f32)mouse_dy()*mouse_speed)),
				angle_from_degrees(-89.9f),
				angle_from_degrees(89.9f)
			);
		}

		i32 width = context_window_width();
		i32 height = context_window_height();
		f32 aspect = (f32)width / (f32)height;

		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		f32 sin = angle_sin(v_ang);
		f32 cos = angle_cos(v_ang);
		Camera cam = {
			.pos = pos, 
			.dir = dir3_from_vec(v3_new(dir.x*cos, dir.y*cos, -sin)),
			.up = dir3_from_vec(up),
		};
		Perspective p = {
			.fov_z_rad = 75.0f * DEG_TO_RAD,
			.aspect = aspect,
			.near = 0.1f,
			.far = 100.0f,
		};
		chunks_draw(&chunks, cam, p);
		context_swap_buffers();
		context_update();
		input_update();
	}

	chunks_deinit(&chunks);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
}

int main(void) {
	if (!context_try_init()) {
		return 1;
	}
	input_init();
	render_init();

	world_run();
	
	context_deinit();
	return 0;
}
