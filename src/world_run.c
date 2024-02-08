#include "world_run.h"
#include "world_run.h"
#include "chunk.h"

void world_run(App* app) {
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
}
