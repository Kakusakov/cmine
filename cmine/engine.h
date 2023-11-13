#pragma once
#include <stdbool.h>

struct KeyInput {
	bool is_rmb_pressed;
	bool is_lmb_pressed;

	bool is_w_pressed;
	bool is_a_pressed;
	bool is_s_pressed;
	bool is_d_pressed;
};
typedef struct KeyInput KeyInput;

struct CursorInput {
	bool is_inside_window;
	double x;
	double y;
};
typedef struct CursorInput CursorInput;

struct WindowInput {
	int width;
	int height;
};
typedef struct WindowInput WindowInput;

struct TimeInput {
	double seconds;
};
typedef struct TimeInput TimeInput;

struct Input {
	KeyInput keys;
	CursorInput cursor;
	WindowInput window;
	TimeInput time;
};
typedef struct Input Input;

struct Scene {
	void* self;
	void (*update)(struct App* app);
};
typedef struct Scene Scene;
