#pragma once
#include "types.h"
#include <GLFW/glfw3.h>

typedef u8 MouseKey;
enum
{
    mouse_key_left,
    mouse_key_right,
    mouse_key_middle,
    mouse_key_count
};

typedef u8 Key;
enum
{
    // Keybord buttons:

    key_a,
    key_b,
    key_c,
    key_d,
    key_e,
    key_f,
    key_g,
    key_h,
    key_i,
    key_j,
    key_k,
    key_l,
    key_m,
    key_n,
    key_o,
    key_p,
    key_q,
    key_r,
    key_s,
    key_t,
    key_u,
    key_v,
    key_w,
    key_x,
    key_y,
    key_z,
    key_tilde,
    key_space,
    key_esc,
    key_left_shift,
    key_tab,
    key_count
};

typedef void* (*GLLoaderFunPtr)(const char*);

bool context_try_init(void);
void context_update(void);
void context_deinit(void);

GLLoaderFunPtr context_gl_loader(void);
i32 context_window_width(void);
i32 context_window_height(void);
bool context_has_close_flag(void);
void context_set_close_flag(void);
bool context_is_cursor_hovered(void);
bool context_is_cursor_hidden(void);
void context_hide_cursor(void);
void context_show_cursor(void);
bool context_is_window_focused(void);
bool context_is_key_pressed(Key mouse_key);
bool context_is_mouse_key_pressed(MouseKey key);
f64 conetxt_mouse_x(void);
f64 conetxt_mouse_y(void);
f64 context_time(void);
void context_swap_buffers(void);
