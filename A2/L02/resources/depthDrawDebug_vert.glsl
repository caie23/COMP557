#version 410 core
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

in vec4 aPos;  
in vec2 aTex;

out vec2 texCoordForFP;

void main() {	
   gl_Position = P * V * M * aPos;
   texCoordForFP = aTex;
}