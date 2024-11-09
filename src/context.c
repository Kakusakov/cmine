#include "context.h"
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

typedef struct Context Context;
struct Context {
	GLFWwindow* window;
};

static Context context;

static const char* glfw_error_code_name(int error_code) {
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
static void glfw_error_callback(int error_code, const char* description) {
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
bool context_try_init(void) {
	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW.\n");
		return false;
	}

	// Will try to load with OpenGL 4.4 environment, but
	// resorts to using OpenGL 3.3 otherwise.
	for (int version = 4; version >= 3; version--)
	{
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, version);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, version);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		context.window = glfwCreateWindow(
			800,
			600,
			"test window",
			NULL,
			NULL
		);
		if (!context.window) continue;
		glfwMakeContextCurrent(context.window);

		return true;
	}
	fprintf(stderr, "Failed to create a window capable of at least OpenGL 3.3.\n");
	glfwTerminate();
	return false;
}
void context_deinit(void) {
	glfwMakeContextCurrent(NULL);
	glfwDestroyWindow(context.window);
	glfwTerminate();
}
GLLoaderFunPtr context_gl_loader(void) {
	return (GLLoaderFunPtr)glfwGetProcAddress;
}
void context_swap_buffers(void) {
	glfwSwapBuffers(context.window);
}
void context_update(void) {
	glfwPollEvents();
}
bool context_has_close_flag(void) {
	return glfwWindowShouldClose(context.window);
}
void context_set_close_flag(void) {
	glfwSetWindowShouldClose(context.window, GLFW_TRUE);
}
i32 context_window_width(void) {
	int w;
	glfwGetWindowSize(context.window, &w, NULL);
	return (i32)w;
}
i32 context_window_height(void) {
	int h;
	glfwGetWindowSize(context.window, NULL, &h);
	return (i32)h;
}
bool context_is_window_focused(void) {
	return glfwGetWindowAttrib(context.window, GLFW_FOCUSED);
}
bool context_is_cursor_hovered(void) {
	return glfwGetWindowAttrib(context.window, GLFW_HOVERED);
}
bool context_is_cursor_hidden(void) {
	return glfwGetInputMode(context.window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED;
}
void context_hide_cursor(void) {
	// TODO: use enum like CURSOR_VISIBILITY...
	glfwSetInputMode(context.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetInputMode(context.window, GLFW_RAW_MOUSE_MOTION, glfwRawMouseMotionSupported());
}
void context_show_cursor(void) {
	// TODO: use enum like CURSOR_VISIBILITY...
	glfwSetInputMode(context.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	glfwSetInputMode(context.window, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
}
bool context_is_key_pressed(Key key) {
	int glfw_key;
	switch (key) {
	case key_a: glfw_key = GLFW_KEY_A; break;
	case key_b: glfw_key = GLFW_KEY_B; break;
	case key_c: glfw_key = GLFW_KEY_C; break;
	case key_d: glfw_key = GLFW_KEY_D; break;
	case key_e: glfw_key = GLFW_KEY_E; break;
	case key_f: glfw_key = GLFW_KEY_F; break;
	case key_g: glfw_key = GLFW_KEY_G; break;
	case key_h: glfw_key = GLFW_KEY_H; break;
	case key_i: glfw_key = GLFW_KEY_I; break;
	case key_j: glfw_key = GLFW_KEY_J; break;
	case key_k: glfw_key = GLFW_KEY_K; break;
	case key_l: glfw_key = GLFW_KEY_L; break;
	case key_m: glfw_key = GLFW_KEY_M; break;
	case key_n: glfw_key = GLFW_KEY_N; break;
	case key_o: glfw_key = GLFW_KEY_O; break;
	case key_p: glfw_key = GLFW_KEY_P; break;
	case key_q: glfw_key = GLFW_KEY_Q; break;
	case key_r: glfw_key = GLFW_KEY_R; break;
	case key_s: glfw_key = GLFW_KEY_S; break;
	case key_t: glfw_key = GLFW_KEY_T; break;
	case key_u: glfw_key = GLFW_KEY_U; break;
	case key_v: glfw_key = GLFW_KEY_V; break;
	case key_w: glfw_key = GLFW_KEY_W; break;
	case key_x: glfw_key = GLFW_KEY_X; break;
	case key_y: glfw_key = GLFW_KEY_Y; break;
	case key_z: glfw_key = GLFW_KEY_Z; break;
	case key_tilde: glfw_key = GLFW_KEY_GRAVE_ACCENT; break;
	case key_esc:   glfw_key = GLFW_KEY_ESCAPE; break;
	case key_space: glfw_key = GLFW_KEY_SPACE; break;
	case key_left_shift: glfw_key = GLFW_KEY_LEFT_SHIFT; break;
	case key_tab: glfw_key = GLFW_KEY_TAB; break;
	default: abort();
	};
	return glfwGetKey(context.window, glfw_key) == GLFW_PRESS;

}
bool context_is_mouse_key_pressed(MouseKey mouse_key) {
	int glfw_mouse_button;
	switch (mouse_key)
	{
	case mouse_key_left:   glfw_mouse_button = GLFW_MOUSE_BUTTON_LEFT;   break;
	case mouse_key_right:  glfw_mouse_button = GLFW_MOUSE_BUTTON_RIGHT;  break;
	case mouse_key_middle: glfw_mouse_button = GLFW_MOUSE_BUTTON_MIDDLE; break;
	default: abort();
	}
	return glfwGetMouseButton(context.window, glfw_mouse_button) == GLFW_PRESS;
}
f64 conetxt_mouse_x(void) {
	double x;
	glfwGetCursorPos(context.window, &x, NULL);
	return (f64)x;
}
f64 conetxt_mouse_y(void) {
	double y;
	glfwGetCursorPos(context.window, NULL, &y);
	return (f64)y;
}
f64 context_time(void) {
	return (f64)glfwGetTime();
}
