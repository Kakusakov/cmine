#pragma once
#include <stdbool.h>

struct ButtonSettings {
	bool is_rmb_pressed;
	bool is_lmb_pressed;
};
typedef struct ButtonSettings ButtonSettings;

struct CursorSettings {
	bool is_inside_window;
	double x;
	double y;
};
typedef struct CursorSettings CursorSettings;

struct WindowSettings {
	int width;
	int height;
};
typedef struct WindowSettings WindowSettings;

struct TimeSettings {
	double seconds;
};
typedef struct TimeSettings TimeSettings;

struct Settings {
	//ButtonSettings button;
	CursorSettings cursor;
	WindowSettings window;
	TimeSettings time;
};
typedef struct Settings Settings;

void on_engine_startup(void** user_pointer, const Settings* settings);
void on_engine_update(void** user_pointer, const Settings* settings);
void on_engine_shutdown(void** user_pointer, const Settings* settings);
