#include "input.h"

void key_buffer_reset(KeyBuffer* buffer)
{
	for (int key = 0; key < app_key_count; key++)
	{
		buffer->is_key_pressed[key] = app_is_key_pressed(key);
		buffer->was_key_pressed[key] = buffer->is_key_pressed[key];
	}
}
void key_buffer_update(KeyBuffer* buffer)
{
	for (int key = 0; key < app_key_count; key++)
	{
		buffer->was_key_pressed[key] = buffer->is_key_pressed[key];
		buffer->is_key_pressed[key] = app_is_key_pressed(key);
	}
}
