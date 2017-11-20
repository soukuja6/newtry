#version 330	// GLSL version

// Per-fragment color coming from the vertex shader
in vec4 fcolor;

// Per-frgament output color
out vec4 FragColor;

void main() { 
	// Set the output color according to the input
    FragColor = fcolor;
}