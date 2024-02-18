#include "app.h"
#include "render.h"

static void main_menu_run(void) {
	GLuint texture = render_tmp_texture();
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	Vec3f pos = {-0.4f, 0.0f, 0.0f};
	Vec3f look = {0.0f, 0.0f, 1.0f};
	float fov_y_radians = 75.0f * DEG_TO_RAD;
	float near = 0.1f;
	float far = 100.0f;
	float prev_time = app_time();

	app_update();
	while (!app_should_close()) 
	{
		float time = app_time();
		float dt = time - prev_time;

		Vec3f dir = {0};
		if (app_is_key_pressed(app_key_w)) dir.z += 1;
		if (app_is_key_pressed(app_key_s)) dir.z -= 1;
		if (app_is_key_pressed(app_key_a)) dir.x += 1;
		if (app_is_key_pressed(app_key_d)) dir.x -= 1;

		pos.x += dir.x * dt;
		pos.y += dir.y * dt;
		pos.z += dir.z * dt;

		Mat transform = MAT_IDENTITY;
		transform = mat_look_at(
			transform,
			(Vec3f){sinf(time), 0, cosf(time)},
			(Vec3f){0},
			(Vec3f){0, 1, 0}
		);
		transform = mat_translate(
			transform,
			pos
		);
		int width = app_window_width();
		int height = app_window_height();
		transform = mat_perspective(
			transform,
			(float)width / (float)height,
			fov_y_radians,
			near,
			far);

		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT);
		render_draw_quad(texture, transform);
		app_swap_buffers();

		prev_time = time;
		app_update();
	}
}

int main(void) {
	if (!app_init()) return 1;
	if (!render_init()) return 1;

	app_update();
	if (!app_should_close()) main_menu_run();

	app_deinit();
	return 0;
}

/*void world_run(void) {
	Arena* arena = arena_init();
	Chunk* chunk = arena_alloc(arena, 1, Chunk);
	*chunk = chunk_init();
	unsigned seed = 0;
	Perlin* perlin = perlin_from_stdrand(arena, seed);
	FBM fbm = {
		.octave_count = 1,
		.frequency = 0.1,
		.intensity = 0.1,
		.persistance = 1,
		.lacunarity = 1,
	};
	chunk_generate_blocks(chunk, perlin, &fbm, (Vec3i){0});
	chunk_generate_mesh(chunk, (AdjacentChunks){0});

	app_update(app);
	while (!app_should_close(app)) {
		// TODO: Draw chunk...
		app_update(app);
	}
	chunk_deinit(chunk);
	arena_deinit(arena);
}*/
