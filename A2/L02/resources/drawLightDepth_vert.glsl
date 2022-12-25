#version 410 core
layout(location = 0) in vec4 aPos;

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

void main() {
    gl_Position = P * V * M * aPos; 
}
