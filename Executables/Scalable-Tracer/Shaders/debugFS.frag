#version 450 core

uniform sampler2D tex;

uniform float lod;
uniform int tileOrder[9] = { 0, 0, 0,  0, 0, 0,   0, 0, 0 };

in vec2 texCoords;

layout( location=0 ) out vec4 pixelColor;

void main() {
	// Conversion
	//ivec2 nIndices = ivec2( clamp( int(texCoords.x * 3.0), 0, 2 ), clamp( int(texCoords.y * 3.0), 0, 2 ) );
	//int lIndex = tileOrder[ nIndices.x + nIndices.y*3 ];
	//ivec2 tIndices = ivec2( lIndex%3, lIndex/3 );
	//vec2 conversionV = vec2( tIndices-nIndices) / vec2( 3.0, 3.0 );

	// No conversion
	vec2 conversionV = vec2(0,0);

	pixelColor = vec4( texture( tex, texCoords + conversionV, lod ).rgb, 1.0 );
}