#version 330	// GLSL version
in vec3 texCoord;
out vec4 fragColor;
uniform samplerCube cubemap;
 
void main (void) {
    fragColor = texture(cubemap, texCoord);
}