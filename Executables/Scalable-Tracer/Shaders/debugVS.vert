#version 450 core

layout( location=0 ) in vec2 inVertex;

out vec2 texCoords;

void main() {
	gl_Position = vec4( inVertex.x, inVertex.y, 0, 1 );
	texCoords = inVertex / 2.0 + vec2( 0.5, 0.5 );
}