#include "main_menu_run.h"
#include "safety.h"
#include "gl_error.h"
#include "glad.h"

int main(void) {
	App* app = app_init();
	app_bind_current_context(app);
	try(gladLoadGLLoader(app_context_gl_loader()));
	setup_gl_error_callback();

	app_update(app);
	if (!app_should_close(app)) {
		main_menu_run(app);
	}

	app_unbind_current_context();
	app_deinit(app);
	return 0;
}
