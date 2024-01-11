#pragma once
#include "input.h"

typedef struct App App;
typedef void* (*GLLoaderFunptr)(const char*);

App* app_init(void);
void app_deinit(App* const app);

GLLoaderFunptr app_context_gl_loader(void);
void app_bind_current_context(App* const app);
void app_unbind_current_context(void);
void app_update(App* const app);
bool app_should_close(const App* const app);
Input app_input(const App* const app);
