#version 330	// GLSL version
layout (location = 0) in vec3 Position;
out vec3 texCoord;
uniform mat4 matrix;
 
void main() {
    gl_Position = matrix * vec4(Position, 1.0);
    texCoord = Position;
}