#pragma once

enum
{
    // Mouse buttons:

    app_key_mouse_left,
    app_key_mouse_right,
    app_key_mouse_middle,

    // Keybord buttons:

    app_key_a,
    app_key_b,
    app_key_c,
    app_key_d,
    app_key_e,
    app_key_f,
    app_key_g,
    app_key_h,
    app_key_i,
    app_key_j,
    app_key_k,
    app_key_l,
    app_key_m,
    app_key_n,
    app_key_o,
    app_key_p,
    app_key_q,
    app_key_r,
    app_key_s,
    app_key_t,
    app_key_u,
    app_key_v,
    app_key_w,
    app_key_x,
    app_key_y,
    app_key_z,
    app_key_tilde,
    app_key_space,
    app_key_esc,
    app_key_left_shift,
    app_key_tab,
    app_key_count
};


int app_init(void);
void app_deinit(void);

void app_update(void);
void app_swap_buffers(void);
int app_should_close(void);

typedef void* (*GLLoaderFunPtr)(const char*);
GLLoaderFunPtr app_context_gl_loader(void);

float app_time(void);
int app_window_width(void);
int app_window_height(void);
int app_is_window_focused(void);
float app_cursor_position_x(void);
float app_cursor_position_y(void);
int app_is_cursor_hovered(void);
int app_is_key_pressed(int key);

int app_is_cursor_hidden(void);
void app_hide_cursor(void);
void app_show_cursor(void);
