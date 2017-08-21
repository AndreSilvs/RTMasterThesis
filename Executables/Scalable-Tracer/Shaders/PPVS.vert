#version 330 core

in vec2 inVertex;
in vec2 inTexture;

out vec2 texCoords;

void main() {
	texCoords = inTexture;
    gl_Position = vec4( inVertex.x, inVertex.y, 0, 1 );
}