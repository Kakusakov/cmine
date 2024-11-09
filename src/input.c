#include "input.h"

typedef struct Keyboard Keyboard;
struct Keyboard {
	bool kys[key_count];
};

static inline void keyboard_update(Keyboard* self) {
	for (Key key = 0; key < key_count; key++) {
		self->kys[key] = context_is_key_pressed(key);
	}
}

typedef struct Mouse Mouse;
struct Mouse {
	bool kys[mouse_key_count];
	f64 x;
	f64 y;
};

static inline void mouse_update(Mouse* self) {
	for (MouseKey mouse_key = 0; mouse_key < mouse_key_count; mouse_key++) {
		self->kys[mouse_key] = context_is_mouse_key_pressed(mouse_key);
	}
	self->x = conetxt_mouse_x();
	self->y = conetxt_mouse_y();
}

typedef struct Time Time;
struct Time {
	f64 seconds;
};

static inline void time_update(Time* self) {
	self->seconds = context_time();
}

typedef struct Input Input;
struct Input {
	Time time_prev;
	Time time;
	Mouse mouse_prev;
	Mouse mouse;
	Keyboard keyboard_prev;
	Keyboard keyboard;
};

static Input input;

void input_init(void) {
	keyboard_update(&input.keyboard);
	mouse_update(&input.mouse);
	time_update(&input.time);
	input.keyboard_prev = input.keyboard;
	input.mouse_prev = input.mouse;
	input.time_prev = input.time;
}
void input_update() {
	input.keyboard_prev = input.keyboard;
	input.mouse_prev = input.mouse;
	input.time_prev = input.time;
	keyboard_update(&input.keyboard);
	mouse_update(&input.mouse);
	time_update(&input.time);
}
f64 mouse_x(void) {
	return input.mouse.x;
}
f64 mouse_y(void) {
	return input.mouse.y;
}
f64 mouse_dx(void) {
	return input.mouse.x - input.mouse_prev.x;
}
f64 mouse_dy(void) {
	return input.mouse.y - input.mouse_prev.y;
}
bool is_mouse_pressed(MouseKey key) {
	return input.mouse.kys[key];
}
bool is_mouse_down(MouseKey key) {
	return input.mouse.kys[key] && !input.mouse_prev.kys[key];
}
bool is_mouse_up(MouseKey key) {
	return !input.mouse.kys[key] && input.mouse_prev.kys[key];
}
bool is_key_pressed(Key key) {
	return input.keyboard.kys[key];
}
bool is_key_down(Key key) {
	return input.keyboard.kys[key] && !input.keyboard_prev.kys[key];
}
bool is_key_up(Key key) {
	return !input.keyboard.kys[key] && input.keyboard_prev.kys[key];
}
f64 time(void) {
	return input.time.seconds;
}
f64 delta_time(void) {
	f64 delta = input.time.seconds - input.time_prev.seconds;
	if (delta > 0.2) {
		delta = 0.2;
	}
	return delta;
}

