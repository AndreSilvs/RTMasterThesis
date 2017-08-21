#version 330 core

in vec2 texCoords;

uniform sampler2D texture0;

layout(location = 0) out vec4 outColor;

uniform bool doQuantization = false;
uniform int quantization = 8;

vec4 quantize( vec4 color, int quantization ) {
	return vec4( float( int( color.x * quantization ) ) / float( quantization ),
				 float( int( color.y * quantization ) ) / float( quantization ),
				 float( int( color.z * quantization ) ) / float( quantization ),
				 color.a );
}

void main() {
	vec4 color = texture( texture0, texCoords );
	if ( doQuantization ) {
		outColor = quantize( color, quantization );
	}
	else {
		outColor = color;
	}
}