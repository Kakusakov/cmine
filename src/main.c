#include "app.h"
#include "render.h"

static void main_menu_run(void) {
	GLuint texture = render_tmp_texture();
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	Vec3f pos = {0.0f, -0.4f, 0.0f};
	float fov_z_radians = 75.0f * DEG_TO_RAD;
	float near = 0.1f;
	float far = 100.0f;
	float prev_time = app_time();

	app_update();
	while (!app_should_close())
	{
		float time = app_time();
		float dt = time - prev_time;

		Vec3f dir = {0};
		if (app_is_key_pressed(app_key_w)) dir.x += 1;
		if (app_is_key_pressed(app_key_s)) dir.x -= 1;
		if (app_is_key_pressed(app_key_a)) dir.y += 1;
		if (app_is_key_pressed(app_key_d)) dir.y -= 1;

		pos.x += dir.x * dt;
		pos.y += dir.y * dt;
		pos.z += dir.z * dt;

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
