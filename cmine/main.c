#include "safety.h"
#include "engine.h"
#include "glfw_error.h"

#include "glad.h"
#include <GLFW/glfw3.h>

#include <stdlib.h>

static void update_settings(GLFWwindow* window, Settings* settings) {
	glfwPollEvents();

	// Update window settigns.
	glfwGetFramebufferSize(window, &settings->window.width, &settings->window.height);

	// Update cursor settings.
	glfwGetCursorPos(window, &settings->cursor.x, &settings->cursor.y);
	settings->cursor.is_inside_window = glfwGetWindowAttrib(window, GLFW_HOVERED);

	// Update time settings.
	settings->time.seconds = glfwGetTime();
}

void main(void) {
	setup_glfw_error_callback();
	satexit(glfwTerminate);
	TRY(glfwInit());
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow* window;
	TRY((window = glfwCreateWindow(
		800,
		600,
		"test window", 
		NULL, 
		NULL
	)));
	glfwMakeContextCurrent(window);
	TRY(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress));

	Settings settings;
	void* user_pointer;
	update_settings(window, &settings);
	on_engine_startup(&user_pointer, settings);

	while (!glfwWindowShouldClose(window)) {
		update_settings(window, &settings);
		on_engine_update(&user_pointer, settings);
		glfwSwapBuffers(window);
	}

	on_engine_shutdown(&user_pointer, settings);

	return 0;
}
