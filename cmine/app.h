#pragma once
#include "engine.h"

typedef struct App App;

void app_update(App* app);

bool app_should_close(App* app);
const Input* app_input(App* app);
Scene* app_active_scene(App* app);
