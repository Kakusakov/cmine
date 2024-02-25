#pragma once
#include "app.h"
#include "stdbool.h"

typedef struct KeyBuffer KeyBuffer;
struct KeyBuffer
{
	bool is_key_pressed[app_key_count];
	bool was_key_pressed[app_key_count];
};

static inline int key_buffer_is_key_pressed(KeyBuffer *buffer, int key)
{
	return buffer->is_key_pressed[key];
}
static inline int key_buffer_is_key_up(KeyBuffer *buffer, int key)
{
	return buffer->is_key_pressed[key] & !buffer->was_key_pressed[key];
}
static inline int key_buffer_is_key_down(KeyBuffer *buffer, int key)
{
	return !buffer->is_key_pressed[key] & buffer->was_key_pressed[key];
}
void key_buffer_reset(KeyBuffer *buffer);
void key_buffer_update(KeyBuffer *buffer);
