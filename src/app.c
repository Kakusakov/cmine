#include "app.h"
#include "safety.h"
#include <GLFW/glfw3.h>
#include <stdio.h>

static char* glfw_error_code_name(int error_code) {
	switch (error_code) {
	case GLFW_NO_ERROR:
		return "GLFW_NO_ERROR";
	case GLFW_NOT_INITIALIZED:
		return "GLFW_NOT_INITIALIZED";
	case GLFW_NO_CURRENT_CONTEXT:
		return "GLFW_NO_CURRENT_CONTEXT";
	case GLFW_INVALID_ENUM:
		return "GLFW_INVALID_ENUM";
	case GLFW_INVALID_VALUE:
		return "GLFW_INVALID_VALUE";
	case GLFW_OUT_OF_MEMORY:
		return "GLFW_OUT_OF_MEMORY";
	case GLFW_API_UNAVAILABLE:
		return "GLFW_API_UNAVAILABLE";
	case GLFW_VERSION_UNAVAILABLE:
		return "GLFW_VERSION_UNAVAILABLE";
	case GLFW_PLATFORM_ERROR:
		return "GLFW_PLATFORM_ERROR";
	case GLFW_FORMAT_UNAVAILABLE:
		return "GLFW_FORMAT_UNAVAILABLE";
	case GLFW_NO_WINDOW_CONTEXT:
		return "GLFW_NO_WINDOW_CONTEXT";
	default:
		return "[unknown glfw error code]";
	}
}

static void glfw_error_callback(int error_code, const char* description) {
	fprintf(
		stderr,
		"\nCaught runtime error:\n"
		"\tGlfw invoked the error callback.\n"
		"\tError code = `%s (0x%x)`\n"
		"\tDescription = `%s`\n"
		"\tResuming...\n",
		glfw_error_code_name(error_code),
		error_code,
		description
	);
}

static void setup_glfw_error_callback(void) {
	glfwSetErrorCallback(glfw_error_callback);
}

struct App {
	GLFWwindow* window;
	Input input;
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

static void glfw_deinit(void) {
	glfwTerminate();
}

static void glfw_static_init(void) {
	static bool is_initialized = false;
	if (!is_initialized) {
		setup_glfw_error_callback();
		satexit(glfw_deinit);
		try(glfwInit());
		is_initialized = true;
	}
}

App* app_init(void) {
	App* const app = smalloc(sizeof(App));
	glfw_static_init();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	app->window = glfwCreateWindow(
		800,
		600,
		"test window",
		NULL,
		NULL
	);
	if (!app->window) {
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		try((app->window = glfwCreateWindow(
			800,
			600,
			"test window",
			NULL,
			NULL
		)));
	}

	return app;
}
void app_deinit(App* const app) {
	glfwDestroyWindow(app->window);
	sfree(app);
}

GLLoaderFunptr app_context_gl_loader(void) {
	return (GLLoaderFunptr)glfwGetProcAddress;
}
void app_bind_current_context(App* const app) {
	glfwMakeContextCurrent(app->window);
}
void app_unbind_current_context(void) {
	glfwMakeContextCurrent(NULL);
}
void app_update(App* const app) {
	glfwSwapBuffers(app->window);
	update_input(app);
	app->should_close = (bool)glfwWindowShouldClose(app->window);
}
bool app_should_close(const App* const app) {
	return app->should_close;
}
Input app_input(const App* const app) {
	return app->input;
}
