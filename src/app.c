#include "app.h"
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stddef.h>

static GLFWwindow* window;

static const char *glfw_error_code_name(int error_code) 
{
	switch (error_code) {
	case GLFW_NO_ERROR:            return "GLFW_NO_ERROR";
	case GLFW_NOT_INITIALIZED:     return "GLFW_NOT_INITIALIZED";
	case GLFW_NO_CURRENT_CONTEXT:  return "GLFW_NO_CURRENT_CONTEXT";
	case GLFW_INVALID_ENUM:        return "GLFW_INVALID_ENUM";
	case GLFW_INVALID_VALUE:       return "GLFW_INVALID_VALUE";
	case GLFW_OUT_OF_MEMORY:       return "GLFW_OUT_OF_MEMORY";
	case GLFW_API_UNAVAILABLE:     return "GLFW_API_UNAVAILABLE";
	case GLFW_VERSION_UNAVAILABLE: return "GLFW_VERSION_UNAVAILABLE";
	case GLFW_PLATFORM_ERROR:      return "GLFW_PLATFORM_ERROR";
	case GLFW_FORMAT_UNAVAILABLE:  return "GLFW_FORMAT_UNAVAILABLE";
	case GLFW_NO_WINDOW_CONTEXT:   return "GLFW_NO_WINDOW_CONTEXT";
	default:                       return "[unknown glfw error code]";
	}
}
static void glfw_error_callback(int error_code, const char* description) 
{
	fprintf(
		stderr,
		"\nCaught runtime error:\n"
		"\tGlfw invoked the error callback.\n"
		"\tError code = `%s (0x%x)`\n"
		"\tDescription = `%s`\n",
		glfw_error_code_name(error_code),
		error_code,
		description);
	// TODO: should this exit?
}
int app_init(void)
{
	if (window) return 1;
	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit()) 
	{
		fprintf(stderr, "Failed to initialize GLFW.\n");
		return 0;
	}

	// Will try to load with OpenGL 4.4 environment, but
	// resorts to using OpenGL 3.3 otherwise.
	for (int version = 4; version >= 3; version--)
	{
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, version);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, version);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		window = glfwCreateWindow(
			800,
			600,
			"test window",
			NULL,
			NULL
		);
		if (!window) continue;
		glfwMakeContextCurrent(window);
		return 1;
	}
	fprintf(stderr, "Failed to create a window capable of at least OpenGL 3.3.\n");
	glfwTerminate();
	return 0;
}
void app_deinit(void)
{
	if (!window) return;
	glfwMakeContextCurrent(NULL);
	glfwDestroyWindow(window);
	window = NULL;
	glfwTerminate();
}

void app_swap_buffers(void)
{
	assert(window);
	glfwSwapBuffers(window);
}
void app_update(void)
{
	assert(window);
	glfwPollEvents();
}
int app_should_close(void)
{
	assert(window);
	return glfwWindowShouldClose(window);
}

GLLoaderFunPtr app_context_gl_loader(void)
{
	assert(window);
	return (GLLoaderFunPtr)glfwGetProcAddress;
}

float app_time(void)
{
	assert(window);
	return (float)glfwGetTime();
}
int app_window_width(void)
{
    int width;
    glfwGetWindowSize(window, &width, NULL);
    return width;
}
int app_window_height(void)
{
    int height;
    glfwGetWindowSize(window, NULL, &height);
    return height;
}
int app_is_mouse_pressed(int button)
{
	assert(window);
	int glfw_button;
	switch(button)
	{
		case app_mouse_button_left:   glfw_button = GLFW_MOUSE_BUTTON_LEFT;   break;
		case app_mouse_button_right:  glfw_button = GLFW_MOUSE_BUTTON_RIGHT;  break;
		case app_mouse_button_middle: glfw_button = GLFW_MOUSE_BUTTON_MIDDLE; break;
		default: return 0;
	}
	return glfwGetMouseButton(window, glfw_button);
}
int app_is_key_pressed(int key)
{
	assert(window);
	int glfw_key;
	switch(key)
	{
		case app_key_w: glfw_key = GLFW_KEY_W; break;
		case app_key_a: glfw_key = GLFW_KEY_A; break;
		case app_key_s: glfw_key = GLFW_KEY_S; break;
		case app_key_d: glfw_key = GLFW_KEY_D; break;
		default: return 0;
	}
	return glfwGetKey(window, glfw_key) == GLFW_PRESS;
}
float app_cursor_position_x(void)
{
	assert(window);
	double x;
	glfwGetCursorPos(window, &x, NULL);
	return (float)x;
}
float app_cursor_position_y(void)
{
	assert(window);
	double y;
	glfwGetCursorPos(window, NULL, &y);
	return (float)y;
}
int app_is_cursor_hovered(void)
{
	assert(window);
	return glfwGetWindowAttrib(window, GLFW_HOVERED);
}

void app_hide_cursor(void)
{
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
}
void app_show_cursor(void)
{
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}
