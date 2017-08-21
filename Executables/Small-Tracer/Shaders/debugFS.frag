#version 450 core

uniform sampler2D tex;

uniform float lod;

in vec2 texCoords;

layout( location=0 ) out vec4 pixelColor;

void main() {
	pixelColor = texture( tex, texCoords, lod );
}