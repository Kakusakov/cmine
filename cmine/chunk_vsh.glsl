#version 330 core
layout (location = 0) in uint aPack;

out uint pack;

void main() {
    pack = aPack;
    gl_Position = vec4(1.0);
}
