#version 330 core

in vec2 texCoords;

uniform sampler2D texture0;
uniform vec4 colorMultiplier;

layout(location = 0) out vec4 outColor;

void main() {
    vec4 color = colorMultiplier * texture( texture0, texCoords );
	if ( color.a == 0.0 ) {
		discard;
	}
	outColor = color;
}