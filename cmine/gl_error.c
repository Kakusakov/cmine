#include "gl_error.h"
#include "glad.h"

#include <stdio.h>

static char* gl_message_source_name(GLenum source) {
	switch (source) {
	case GL_DEBUG_SOURCE_API:
		return "GL_DEBUG_SOURCE_API";
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
		return "GL_DEBUG_SOURCE_WINDOW_SYSTEM";
	case GL_DEBUG_SOURCE_SHADER_COMPILER:
		return "GL_DEBUG_SOURCE_SHADER_COMPILER";
	case GL_DEBUG_SOURCE_THIRD_PARTY:
		return "GL_DEBUG_SOURCE_THIRD_PARTY";
	case GL_DEBUG_SOURCE_APPLICATION:
		return "GL_DEBUG_SOURCE_APPLICATION";
	case GL_DEBUG_SOURCE_OTHER:
		return "GL_DEBUG_SOURCE_OTHER";
	default:
		return "[unknown OpenGL message source]";
	}
}

static char* gl_message_type_name(GLenum type) {
	switch (type) {
	case GL_DEBUG_TYPE_ERROR:
		return "GL_DEBUG_TYPE_ERROR";
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		return "GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR";
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		return "GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR";
	case GL_DEBUG_TYPE_PORTABILITY:
		return "GL_DEBUG_TYPE_PORTABILITY";
	case GL_DEBUG_TYPE_PERFORMANCE:
		return "GL_DEBUG_TYPE_PERFORMANCE";
	case GL_DEBUG_TYPE_MARKER:
		return "GL_DEBUG_TYPE_MARKER";
	case GL_DEBUG_TYPE_PUSH_GROUP:
		return "GL_DEBUG_TYPE_PUSH_GROUP";
	case GL_DEBUG_TYPE_POP_GROUP:
		return "GL_DEBUG_TYPE_POP_GROUP";
	case GL_DEBUG_TYPE_OTHER:
		return "GL_DEBUG_TYPE_OTHER";
	default:
		return "[unknown OpenGL message type]";
	}
}

static char* gl_message_severity_name(GLenum severity) {
	switch (severity) {
	case GL_DEBUG_SEVERITY_HIGH:
		return "GL_DEBUG_SEVERITY_HIGH";
	case GL_DEBUG_SEVERITY_MEDIUM:
		return "GL_DEBUG_SEVERITY_MEDIUM";
	case GL_DEBUG_SEVERITY_LOW:
		return "GL_DEBUG_SEVERITY_LOW";
	case GL_DEBUG_SEVERITY_NOTIFICATION:
		return "GL_DEBUG_SEVERITY_NOTIFICATION";
	default:
		return "[unknown OpenGL message severity]";
	}
}

static void GLAPIENTRY gl_message_callback(
	GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam
) {
	fprintf(
		stderr,
		"\nRecieved info:\n"
		"\tOpenGL invoked the message callback.\n"
		"\tMessage source = `%s (0x%x)`\n"
		"\tMessage type = `%s (0x%x)`\n"
		"\tMessage severity = `%s (0x%x)`\n"
		"\tMessage id = `%u`\n"
		"\tMessage = `%.*s`\n"
		"\tResuming...\n",
		gl_message_source_name(source),
		source,
		gl_message_type_name(type),
		type,
		gl_message_severity_name(severity),
		severity,
		id,
		length,
		message
	);
}

void setup_gl_error_callback(void) {
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(gl_message_callback, NULL);
}
