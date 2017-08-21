#version 450 core

layout( location=0 ) in vec2 inVertexPosition;

// Make a uniform block?
uniform vec2 uScreenDims;
uniform vec2 uScreenRatios;

//uniform vec3 uCameraEye; // Maybe needed for ray traversal

uniform vec3 uU,uV,uN;

out vec3 rayDirection;
out vec2 texCoords;

vec3 calculateRayDirection( vec2 screenPos ) {
	//vec3 cRay;
	//cRay.x = screenPos.x * uScreenRatios.x - uScreenRatios.x / 2.0; // U
	//cRay.y = screenPos.y * uScreenRatios.y - uScreenRatios.y / 2.0; // V
	
	//float imgAR = uScreenDims.x / uScreenDims.y;
	//vec3 cRay;
	//cRay.x = ( 2 * screenPos.x -1 ) * tan( 60.0 / 2.0 * (3.141526 /180.0) ) * imgAR;
	//cRay.y = ( 2 * screenPos.y -1 ) * tan( 60.0 / 2.0 * (3.141526 /180.0) );
    //cRay.z = 1.0f; // N
	
	
	/*vec3 cRay;
	cRay.x = ( 2 * screenPos.x - 1.0 ) * uScreenRatios.x; // U
	cRay.y = ( 2 * screenPos.y - 1.0 ) * uScreenRatios.y; // V
	cRay.z = 1.0f;*/
	
	vec3 cRay;
	cRay.x = ( 2 * ( screenPos.x * ( uScreenDims.x - 1.0 ) + 0.5 )/ uScreenDims.x - 1.0 ) * uScreenRatios.x; // U
	cRay.y = ( 2 * ( screenPos.y * ( uScreenDims.y - 1.0 ) + 0.5 )/ uScreenDims.y - 1.0 ) * uScreenRatios.y; // V
	cRay.z = 1.0f;

    vec3 dir = cRay.x * uU + cRay.y * uV + cRay.z * uN;

    return normalize( dir );
}

void main() {
	gl_Position = vec4( inVertexPosition.x, inVertexPosition.y, 0, 1 );
	texCoords = inVertexPosition / 2 + vec2( 0.5, 0.5 );
	rayDirection = calculateRayDirection( texCoords );
}