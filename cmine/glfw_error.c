#include "glad.h"
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

void setup_glfw_error_callback(void) {
	glfwSetErrorCallback(glfw_error_callback);
}
