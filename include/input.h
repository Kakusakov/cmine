#pragma once
#include "context.h"

void input_init(void);
void input_update(void);

f64 mouse_x(void);
f64 mouse_y(void);
f64 mouse_dx(void);
f64 mouse_dy(void);

bool is_mouse_pressed(MouseKey key);
bool is_mouse_down(MouseKey key);
bool is_mouse_up(MouseKey key);

bool is_key_pressed(Key key);
bool is_key_down(Key key);
bool is_key_up(Key key);

f64 time(void);
f64 delta_time(void);
