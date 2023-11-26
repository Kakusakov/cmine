#pragma once
#include "fixed_types.h"

typedef struct KeyInput KeyInput;
struct KeyInput {
	bool is_rmb_pressed;
	bool is_lmb_pressed;

	bool is_w_pressed;
	bool is_a_pressed;
	bool is_s_pressed;
	bool is_d_pressed;
};

typedef struct CursorInput CursorInput;
struct CursorInput {
	bool is_inside_window;
	float64_t x;
	float64_t y;
};

typedef struct WindowInput WindowInput;
struct WindowInput {
	int32_t width;
	int32_t height;
};

typedef struct TimeInput TimeInput;
struct TimeInput {
	float64_t seconds;
};

typedef struct Input Input;
struct Input {
	KeyInput keys;
	CursorInput cursor;
	WindowInput window;
	TimeInput time;
};
