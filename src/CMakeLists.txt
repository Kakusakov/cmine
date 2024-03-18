set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)
set(OpenGL_GL_PREFERENCE GLVND)

include_directories("${cmine_SOURCE_DIR}/include")
file(GLOB cmine_ALL_HEADERS "${cmine_SOURCE_DIR}/include/*.h")
file(GLOB cmine_ALL_SOURCES "*.c")

add_executable(cmine ${cmine_ALL_HEADERS} ${cmine_ALL_SOURCES})
target_link_libraries(cmine glfw)
find_package(OpenGL REQUIRED)
target_link_libraries(cmine OpenGL::GL)
