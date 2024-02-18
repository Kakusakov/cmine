#pragma once

// Initializes the app. 
// Should be ran before any other function.
// If app is already initialized calling this will do nothing.
// Returns zero if initializatoin fails.
int app_init(void);
// Deinitializes the app. 
// To initialize app again call app_init() again.
// If app is not initialized calling this will do nothing.
void app_deinit(void);

// Returns whether the close flag on the window is set.
int app_should_close(void);
// Handles the events recieved by the app.
void app_update(void);
// Swaps the rendering buffers of the window.
void app_swap_buffers(void);

typedef void *(*GLLoaderFunPtr)(const char *);
// Returns the pointer to fuction that can query OpenGL fuction by name.
GLLoaderFunPtr app_context_gl_loader(void);

enum 
{
    app_mouse_button_left,
    app_mouse_button_right,
    app_mouse_button_middle,
    app_mouse_button_count
};
enum
{
    app_key_w,
    app_key_a,
    app_key_s,
    app_key_d,
    app_key_count
};
// Returns the time in seconds since the start of an app.
float app_time(void);
// Returns the current width of the window in pixels.
int app_window_width(void);
// Returns the current height of the window in pixels.
int app_window_height(void);
// Returns whether a certain mouse buttion is pressed.
int app_is_mouse_pressed(int button);
// Returns whether a certain key is pressed.
int app_is_key_pressed(int key);
// Returns the x postion of a cursor relative to the top-left widnow corner.
float app_cursor_position_x(void);
// Returns the x postion of a cursor relative to the top-left widnow corner.
float app_cursor_position_y(void);
// Returns wheter the cursor is hovering over window.
int app_is_cursor_hovered(void);

// Makes the cursor invisible.
void app_hide_cursor(void);
// Makes the cursor visible (default).
void app_show_cursor(void);
