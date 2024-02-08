# Beware, for I have no clue what am I doing.

cmake_minimum_required(VERSION 3.5)
project(cmine LANGUAGES C)

include(FetchContent)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
	glfw
	GIT_REPOSITORY https://github.com/glfw/glfw.git
	GIT_TAG        599fb3de348f73134b4e9d2ee5cabc62ca0b84ab
)
FetchContent_MakeAvailable(glfw)
include_directories(${glfw_SOURCE_DIR}/include)

add_subdirectory(src)
