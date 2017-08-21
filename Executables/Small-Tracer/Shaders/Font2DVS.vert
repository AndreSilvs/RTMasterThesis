#version 330 core

in vec2 inVertex;
in vec2 inTexture;

out vec2 texCoords;

uniform mat4 pvmMatrix;

void main() {
	texCoords = inTexture;
    gl_Position = pvmMatrix * vec4( inVertex.x, inVertex.y, 0, 1 );
}