#version 330 core
layout (location = 0) in vec3 a_pos;
layout (location = 0) in vec2 a_uv;

out vec2 uv;

void main() 
{
    gl_Position = vec4(a_pos, 1.0);
    uv = a_uv;
}