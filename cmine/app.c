#include "app.h"
#include "app_run.h"

#include "safety.h"
#include "engine.h"
#include "glfw_error.h"

#include "main_menu.h"

#include "glad.h"
#include <GLFW/glfw3.h>

#include <stdbool.h>

struct App {
	GLFWwindow* window;
	Input input;
	Scene scene;
	bool should_close;
};

static void update_input(App* app) {
	glfwPollEvents();

	// Update window input.
	glfwGetFramebufferSize(app->window, &app->input.window.width, &app->input.window.height);

	// Update cursor input.
	glfwGetCursorPos(app->window, &app->input.cursor.x, &app->input.cursor.y);
	app->input.cursor.is_inside_window = glfwGetWindowAttrib(app->window, GLFW_HOVERED);

	// Update time input.
	app->input.time.seconds = glfwGetTime();

	// Update key input.
	app->input.keys.is_lmb_pressed = glfwGetMouseButton(app->window, GLFW_MOUSE_BUTTON_LEFT);
	app->input.keys.is_rmb_pressed = glfwGetMouseButton(app->window, GLFW_MOUSE_BUTTON_RIGHT);

	app->input.keys.is_w_pressed = glfwGetKey(app->window, GLFW_KEY_W);
	app->input.keys.is_a_pressed = glfwGetKey(app->window, GLFW_KEY_A);
	app->input.keys.is_s_pressed = glfwGetKey(app->window, GLFW_KEY_S);
	app->input.keys.is_d_pressed = glfwGetKey(app->window, GLFW_KEY_D);
}

void app_update(App* app) {
	glfwSwapBuffers(app->window);
	update_input(app);
	app->should_close = (bool)glfwWindowShouldClose(app->window);
}

bool app_should_close(App* app) {
	return app->should_close;
}

const Input* app_input(App* app) {
	return &app->input;
}

Scene* app_active_scene(App* app) {
	return &app->scene;
}

void app_run(void) {
	setup_glfw_error_callback();
	satexit(glfwTerminate);
	TRY(glfwInit());
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	App app;
	TRY((app.window = glfwCreateWindow(
		800,
		600,
		"test window",
		NULL,
		NULL
	)));
	glfwMakeContextCurrent(app.window);
	TRY(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress));

	app_update(&app);
	if (!app.should_close) {
		app.scene = create_main_menu(&app);

		while (app.scene.self) {
			app.scene.update(&app);
		}
	}

	glfwTerminate();
}
