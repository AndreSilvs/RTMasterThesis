#version 450 core

// VERTEX SHADER VARS
uniform vec2 uScreenDims;
uniform vec2 uScreenRatios;
uniform vec3 uU,uV,uN;
// VERTEX SHADER VARS


uniform vec3 uHeightmapDims;
uniform vec3 uRealDims;

uniform sampler2D uNormalMap;

uniform sampler2D uHeightmap;
uniform sampler2D uMaximumMips;
uniform sampler2D uPhotoTex;

uniform int uLastLOD;
uniform vec3 uCameraEye;

uniform vec3 uLightDirection;

uniform float uFarZ;

uniform float uLodZParam = 256.0;

uniform float uRealPatchSize = 1.0;
//const float realPatchASize = 1.0;

layout(binding = 0, offset=0) uniform atomic_uint totalRays;

in vec3 rayDirection;
in vec2 texCoords;

struct HFIntersection {
	vec3 debug;
	vec2 texCoords;
	vec3 normal;
	float height;
	float t;
	int lod;
	int steps;
	bool intersected;
	bool iterated;
};
struct QRootResult {
	int nRoots; float r1,r2;
};

const float cRayEpsilon = 0.0000001;

layout (location=0) out vec4 outColor;

int getIndex( float pos, float n ) {
	return clamp( int( ( pos * n ) / uHeightmapDims.x ), 0, int(n)-1 );
}

bool isPointWithinBox( vec3 point, vec3 minC, vec3 maxC ) {
	return ( minC.x < point.x && point.x < maxC.x ) &&
		( minC.y < point.y && point.y < maxC.y ) &&
		( minC.z < point.z && point.z < maxC.z );
}

float copySign( float m, float s ) {
	return abs( m ) * sign( s );
}

vec3 stepGradient( int steps ) {
	float ratio = float( steps ) / 80.0;

	if ( steps < 80 ) {
		return mix( vec3( 1, 0, 0 ), vec3( 1, 1, 0 ), ratio );
	}
	else if ( steps < 160 ) {
		return mix( vec3( 1, 1, 0 ), vec3( 0, 1, 0 ), ratio - 1.0 );
	}
	else if ( steps < 240 ) {
		return mix( vec3( 0, 1, 0 ), vec3( 0, 1, 1 ), ratio - 2.0 );
	}
	else if ( steps < 320 ) {
		return mix( vec3( 0, 1, 1 ), vec3( 0, 0, 1 ), ratio - 3.0 );
	}
	else if ( steps < 400 ) {
		return mix( vec3( 0, 0, 1 ), vec3( 1, 0, 1 ), ratio - 4.0 );
	}
	return vec3( 1, 0, 1 );
}

vec4 calculateSkyColor( vec3 rayDir ) {
	float skyRatio = clamp( normalize( rayDir ).z, 0.0, 1.0 );
		
	vec3 ldir = normalize( uLightDirection );

	vec3 skyColor = mix( vec3( 0.2, 0.3, 0.7 ), vec3( 0.0, 0.6, 0.8 ), skyRatio );
	vec4 result = vec4( skyColor * abs( 1.0-pow( 1.0-ldir.z, 2.0 )), 1.0 );

	float ldot = dot( -ldir, normalize( rayDirection ) );
	if( ldot > 0.9996 ) {
		result = vec4( mix( result.rgb, vec3( 1.0, 1.0, 1.0 ), pow( (ldot - 0.9996) / 0.0004, 2.0 ) ), 1.0 );
	}
	return result;
}

// ========================================================================
// BILINEAR PATCH INTERSECTION CODE

vec3 interpolatePatch( vec3 p00, vec3 p01, vec3 p10, vec3 p11, vec2 uv ) {
	return (1.0-uv.x) * (1.0-uv.y) * p00 +
		   (1.0-uv.x) * uv.y       * p01 +
		   uv.x       * (1.0-uv.y) * p10 +
		   uv.x       *uv.y        * p11;
}

vec3 tanU( vec3 p00, vec3 p01, vec3 p10, vec3 p11, float v ) {
	return ( 1.0 - v ) * ( p10 - p00 ) + v * ( p11 - p01 );
}
vec3 tanV( vec3 p00, vec3 p01, vec3 p10, vec3 p11, float u ) {
	return ( 1.0 - u ) * ( p01 - p00 ) + u * ( p11 - p10 );
}
vec3 interpolateNormal( vec3 p00, vec3 p01, vec3 p10, vec3 p11, vec2 uv ) {
	return cross( tanU( p00, p01, p10, p11, uv.x ), tanV( p00, p01, p10, p11, uv.y ) );
}

// Calculate the number of roots, and roots, present in a quadratic equation
// in the form "A.t^2 + B.t + C = 0"
QRootResult quadraticRoots( float a, float b, float c, float minT, float maxT ) {
	QRootResult qrr;

	qrr.r1 = qrr.r2 = minT - minT; // make it lower than min

    if ( a == 0.0 ) // then its close to 0
    {
        if ( b != 0.0 ) // not close to 0
        {
            qrr.r1 = -c / b;
            if ( qrr.r1 > minT && qrr.r1 < maxT ) {//its in the interval
                qrr.nRoots = 1;
				return qrr; //1 soln found
			}
            else { //its not in the interval
				qrr.nRoots = 0;
                return qrr;
			}
        }
        else {
			qrr.nRoots = 0;
            return qrr;
		}
    }
    float d = b*b - 4.0 * a * c; //discriminant
    if ( d <= 0.0 ) // single or no root
    {
        if ( d == 0.0 ) // close to 0
        {
            qrr.r1 = -b / a;
            if ( qrr.r1 > minT && qrr.r1 < maxT ) { // its in the interval
				qrr.nRoots = 1;
                return qrr;
			}
            else { //its not in the interval
				qrr.nRoots = 0;
                return qrr;
			}
        }
        else { // no root d must be below 0
			qrr.nRoots = 0;
            return qrr;
		}
    }

    float q = -0.5  * ( b + copySign( sqrt( d ), b ) );
    qrr.r1 = c / q;
    qrr.r2 = q / a;

    if ( ( qrr.r1 > minT && qrr.r1 < maxT )
        && ( qrr.r2 > minT && qrr.r2 < maxT ) ) {
        qrr.nRoots = 2;
		return qrr;
	}
    else if ( qrr.r1 > minT && qrr.r1 < maxT ) {//then one wasn't in interval
        qrr.nRoots = 1;
		return qrr;
	}
    else if ( qrr.r2 > minT && qrr.r2 < maxT ) {  // make it easier, make u[0] be the valid one always
        float dummy;
        dummy = qrr.r1;
        qrr.r1 = qrr.r2;
        qrr.r2 = dummy; // just in case somebody wants to check it
		qrr.nRoots = 1;
        return qrr;
    }
    
	qrr.nRoots = 0;
	return qrr;
}


float getu(float v, float M1, float M2, float J1, float J2, float K1, float K2, float R1, float R2 ) {
	float denom = ( v*( M1 - M2 ) + J1 - J2 );
    float d2 = ( v*M1 + J1 );
    if ( abs( denom ) > abs( d2 ) ) // which denominator is bigger
    {
        return ( v*( K2 - K1 ) + R2 - R1 ) / denom;
    }
    return -( v*K1 + R1 ) / d2;
}

float computet(vec3 dir, vec3 orig, vec3 srfpos) {
	// if x is bigger than y and z
    if ( abs( dir.x ) >= abs( dir.y ) && abs( dir.x ) >= abs( dir.z ) )
        return ( srfpos.x - orig.x ) / dir.x;
    // if y is bigger than x and z
    else if ( abs( dir.y ) >= abs( dir.z ) ) // && fabs(dir.y()) >= fabs(dir.x()))
        return ( srfpos.y - orig.y ) / dir.y;
    // otherwise x isn't bigger than both and y isn't bigger than both
    else  //if(fabs(dir.z()) >= fabs(dir.x()) && fabs(dir.z()) >= fabs(dir.y()))
        return ( srfpos.z - orig.z ) / dir.z;
}

// Returns an array with the format (u,v,t)
// Where (u,v) are the intersection coordinates, and t is the ray parameter
// An invalid intersection will have t < 0
vec3 intersectBilinearPatch( vec3 rayOrigin, vec3 rayDirection, vec3 p00, vec3 p01, vec3 p10, vec3 p11 ) {
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Equation of the patch:
    // P(u, v) = (1-u)(1-v)P00 + (1-u)vP01 + u(1-v)P10 + uvP11
    // Equation of the ray:
    // R(t) = r + tq
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    vec3 pos1, pos2; //Vector pos = ro + t*rd;
    int num_solutions; // number of solutions to the quadratic
    float vsol[ 2 ]; // the two roots from quadraticroot
    float t2, u; // the t values of the two roots

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Variables for substitition
    // a = P11 - P10 - P01 + P00
    // b = P10 - P00
    // c = P01 - P00
    // d = P00  (d is shown below in the #ifdef raypatch area)
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~  

    // Find a w.r.t. x, y, z
    vec3 a = p11 - p10 - p01 + p00;

    // Find b w.r.t. x, y, z
    vec3 b = p10 - p00;

    // Find c w.r.t. x, y, z
    vec3 c = p01 - p00;

    // Find d w.r.t. x, y, z - subtracting r just after
    vec3 d = p00 - rayOrigin;

    // Find A1 and A2
    float a1 = a.x * rayDirection.z - a.z * rayDirection.x;
    float a2 = a.y * rayDirection.z - a.z * rayDirection.y;
    
    // Find B1 and B2
    float b1 = b.x * rayDirection.z - b.z * rayDirection.x;
    float b2 = b.y * rayDirection.z - b.z * rayDirection.y;

    // Find C1 and C2
    float c1 = c.x*rayDirection.z - c.z*rayDirection.x;
    float c2 = c.y*rayDirection.z - c.z*rayDirection.y;

    // Find D1 and D2
    float d1 = d.x * rayDirection.z - d.z * rayDirection.x;
    float d2 = d.y * rayDirection.z - d.z * rayDirection.y;

    //Vector dir( qx, qy, qz ), orig( rx, ry, rz );
    float qA = a2 * c1 - a1 * c2;
    float qB = a2 * d1 - a1 * d2 + b2 * c1 - b1 * c2;
    float qC = b2 * d1 - b1 * d2;

    vec3 uvt = vec3( -2, -2, -2 );

	QRootResult result = quadraticRoots( qA, qB, qC, -cRayEpsilon, 1 + cRayEpsilon );
	num_solutions = result.nRoots;


    if ( num_solutions == 0 ) {
		uvt = vec3( -1.0, -1.0, -1.0 );
        return uvt; // no solutions found
    }
    else if ( num_solutions == 1 ) {
        uvt.y = result.r1; //vsol[ 0 ];
        uvt.x = getu( uvt.y, a2, a1, b2, b1, c2, c1, d2, d1 );

        pos1 = interpolatePatch( p00, p01, p10, p11, uvt.xy );
        uvt.z = computet( rayDirection, rayOrigin, pos1 );
        if ( ( uvt.x < (1.0f + cRayEpsilon)) && ( uvt.x > (-cRayEpsilon)) && (uvt.z > 0) ) {//vars okay?
			// uvt == ( getu, result.r1, computet );
            return uvt;
		}
        else {
			uvt = vec3( -1.0, -1.0, -1.0 );
            return uvt; // no other soln - so ret false
		}
    }
    else if ( num_solutions == 2 ) {
        uvt.y = result.r1; //vsol[ 0 ];
        uvt.x = getu( uvt.y, a2, a1, b2, b1, c2, c1, d2, d1 );

        pos1 = interpolatePatch( p00, p01, p10, p11, uvt.xy );
        uvt.z = computet( rayDirection, rayOrigin, pos1 );
        if ( uvt.x < 1.0f + cRayEpsilon && uvt.x > -cRayEpsilon && uvt.z > 0 ) {
            u = getu( result.r2, a2, a1, b2, b1, c2, c1, d2, d1 );
            if ( u < 1 + cRayEpsilon && u > cRayEpsilon ) {
                pos2 = interpolatePatch( p00, p01, p10, p11, vec2( u, result.r2 ) );
                t2 = computet( rayDirection, rayOrigin, pos2 );
                if ( t2 < 0 || uvt.z < t2 ) {// t2 is bad or t1 is better
					// uvt = ( getu, result.r1, computet )
                    return uvt;
				}
                // other wise both t2 > 0 and t2 < t1
				uvt = vec3( result.r2, u, t2 );
                return uvt;
            }
			// uvt = ( getu, result.r1, computet )
            return uvt; // u2 is bad but u1 vars are still okay
        }
        else // doesn't fit in the root - try other one
        {
            uvt.y = result.r2; //vsol[ 1 ];
            uvt.x = getu( result.r2, a2, a1, b2, b1, c2, c1, d2, d1 );
            pos1 = interpolatePatch( p00, p01, p10, p11, uvt.xy );
            uvt.z = computet( rayDirection, rayOrigin, pos1 );
            if ( uvt.x < 1 + cRayEpsilon && uvt.x > -cRayEpsilon &&uvt.z > 0 ) {
				//uvt = ( getu, result.r2, computet )
                return uvt;
			}
            else {
				uvt = vec3( -1.0, -1.0, -1.0 );
			    return uvt;
			}
        }
    }

	uvt = vec3( -1.0, -1.0, -1.0 );
    return uvt;
}

// Calculate maximum LOD based on distance from eye
int lodDistanceFunction( float eyeDistance ) {
	//return int( log2( eyeDistance / uLodZParam ) );
	return int( log2( (eyeDistance+uLodZParam) / uLodZParam ) );
}

/*vec3 intersectBarPatch( ivec2 pIndex, vec3 rayOrigin, vec3 rayDir, float tEnter, float tExit, float heightPlane ) {
	if ( tEnter < tStart ) {}
}*/


vec3 intersectHeightPlane( vec3 rayOrigin, vec3 rayDir, float tEnter, float tExit, float heightValue ) {
	vec3 enterPt = rayOrigin + rayDir*tEnter;
	if ( enterPt.z < heightValue ) {
		return vec3( enterPt.xy / uHeightmapDims.xy, tEnter );
	}
	float realt = ( heightValue - rayOrigin.z ) / rayDir.z;
	vec3 pt = rayOrigin + realt * rayDir;
	return vec3( pt.xy / uHeightmapDims.xy, realt );
}


HFIntersection rayTraceTerrain( vec3 rayOrigin, vec3 rayDirection ) {
	HFIntersection result;
	result.steps = 0;
	result.intersected = false;
	result.iterated = false;

	int maxLOD = 0;

	float txMin = - rayOrigin.x / rayDirection.x;
	float tyMin = - rayOrigin.y / rayDirection.y;
	float tzMin = - rayOrigin.z / rayDirection.z;

	float txMax = ( uRealPatchSize * uHeightmapDims.x - rayOrigin.x ) / rayDirection.x;
	float tyMax = ( uRealPatchSize * uHeightmapDims.y - rayOrigin.y ) / rayDirection.y;
	float tzMax = ( uHeightmapDims.z - rayOrigin.z ) / rayDirection.z;

	if ( txMin > txMax ) { float tmp = txMax; txMax = txMin; txMin = tmp; }
	if ( tyMin > tyMax ) { float tmp = tyMax; tyMax = tyMin; tyMin = tmp; }
	if ( tzMin > tzMax ) { float tmp = tzMax; tzMax = tzMin; tzMin = tmp; }

	if ( ( txMin > tyMax ) || ( tyMin > txMax ) ) {
		return result;
	}
	float tMin, tMax;
	if ( tyMin > txMin ) { tMin = tyMin; } else { tMin = txMin; }
	if ( tyMax < txMax ) { tMax = tyMax; } else { tMax = txMax; }


	if ( ( tMin > tzMax ) || ( tzMin > tMax ) ) {
		return result;
	}
	if ( tzMin > tMin ) { tMin = tzMin; }
	if ( tzMax < tMax ) { tMax = tzMax; }

	// Now we have the ts, calculate the start and end points
	vec3 startPoint;
	if ( isPointWithinBox( rayOrigin, vec3( 0, 0, 0 ), vec3( uRealPatchSize*uHeightmapDims.xy, uHeightmapDims.z ) ) ) {
		startPoint = rayOrigin;
		tMin = 0.0f;
	}
	else {
		if ( tMin < 0 ) { return result; }
		startPoint = rayOrigin + tMin * rayDirection;
	}
	vec3 endPoint = rayOrigin + tMax * rayDirection;

	// Start at the coarsest level of the maximum mipmaps
	int currentLOD = uLastLOD;
	float divLOD = 1.0;

	float dtx = ( txMax - txMin ) / divLOD;
	float dty = ( tyMax - tyMin ) / divLOD;

	ivec2 indices = ivec2( 0, 0 );

	// Steps for traversal
	float tx_next, ty_next;
	int ix_step, iy_step;
	int ix_stop, iy_stop;

	tx_next = txMin + ( rayDirection.x < 0 ? ( divLOD - indices.x ) : ( indices.x + 1 ) ) * dtx;
	ix_step = ( rayDirection.x < 0 ? -1 : 1 );
	ix_stop = ( rayDirection.x < 0 ? -1 : int(divLOD) );

	ty_next = tyMin + ( rayDirection.y < 0 ? ( divLOD - indices.y ) : ( indices.y + 1 ) ) * dty;
	iy_step = ( rayDirection.y < 0 ? -1 : 1 );
	iy_stop = ( rayDirection.y < 0 ? -1 : int(divLOD) );

	// Find BB intersection points
	float t = tMin;
	float tNext = 0.0;
	int axis = 0;

	float patchSizeX = uRealPatchSize * uHeightmapDims.x / pow( 2.0, uLastLOD );
	float patchSizeY = uRealPatchSize * uHeightmapDims.y / pow( 2.0, uLastLOD );

	//float lodDistance = 0.0;

	result.iterated = true;
	while ( true ) {
		if ( t > uFarZ ) { return result; }
		if ( indices.x == ix_stop ) { return result; }
		if ( indices.y == iy_stop ) { return result; }

		// Ascend mipmap levels if we have gone below the maxLOD
		maxLOD = lodDistanceFunction( t );
		while ( currentLOD < maxLOD ) {
			// keep the same intersection t because we don't want to move backwards
			result.steps++;
			currentLOD = currentLOD + 1;
			divLOD = divLOD / 2.0;

			dtx = ( txMax - txMin ) / divLOD;
			dty = ( tyMax - tyMin ) / divLOD;

			if ( currentLOD >= uLastLOD ) {
				return result;
			}

			indices /= 2;

			tx_next = txMin + ( rayDirection.x < 0 ? ( divLOD - indices.x ) : ( indices.x + 1 ) ) * dtx;
			ix_stop = ( rayDirection.x < 0 ? -1 : int(divLOD) );

			ty_next = tyMin + ( rayDirection.y < 0 ? ( divLOD - indices.y ) : ( indices.y + 1 ) ) * dty;
			iy_stop = ( rayDirection.y < 0 ? -1 : int(divLOD) );
		}

		if ( tx_next < ty_next ) {
			tNext = tx_next;
			axis = 0;
		}
		else {
			tNext = ty_next;
			axis = 1;
		}

		// Check if ray goes below mipmap altitude - .x because the height is stored in the first channel (single channel for 16bit)
		float heightSample = texelFetch( uMaximumMips, indices, currentLOD ).x;

		float rayZt = ( rayOrigin.z + t * rayDirection.z );
		float rayZnext = ( rayOrigin.z + tNext * rayDirection.z );

		// If ray falls below mipmap height plane we do either:
		// - Bilinear interpolation if at LOD = 0
		// - Or continue testing at a lower LOD
		if ( rayZt <= ( heightSample * uHeightmapDims.z ) || rayZnext <= ( heightSample * uHeightmapDims.z ) )  {
			if ( currentLOD == 0 ) {
				// Intersect bilinear patch
				vec4 bPatch = texelFetch( uHeightmap, indices, 0 );

				vec3 p00 = vec3( float(indices.x)  * uRealPatchSize, float(indices.y)  *uRealPatchSize, bPatch.r * uHeightmapDims.z );
				vec3 p10 = vec3( float(indices.x+1)* uRealPatchSize, float(indices.y)  *uRealPatchSize, bPatch.g * uHeightmapDims.z );
				vec3 p01 = vec3( float(indices.x)  * uRealPatchSize, float(indices.y+1)*uRealPatchSize, bPatch.b * uHeightmapDims.z );
				vec3 p11 = vec3( float(indices.x+1)* uRealPatchSize, float(indices.y+1)*uRealPatchSize, bPatch.a * uHeightmapDims.z );

				// Check if the ray does not intersect the patch
				vec2 entranceXY = rayOrigin.xy + t * rayDirection.xy;
				vec2 entranceCoords = ( entranceXY - uRealPatchSize * vec2( indices ) ) / uRealPatchSize;
				vec3 entrancePoint = interpolatePatch( p00, p01, p10, p11, entranceCoords );
				if ( rayZt < entrancePoint.z ) {
					// No intersection actually happens because the ray passes below the patch
					// but we need to simulate a correct intersection to fill transition gaps

					result.debug.x = 1.0;
					result.intersected = true;
					result.lod = 0;
					result.height = rayZt;
					result.t = t;
					result.texCoords = entranceXY / ( uRealPatchSize * uHeightmapDims.xy );
					result.normal = normalize( interpolateNormal( p00, p01, p10, p11, entranceCoords ) );
					return result;
				}

				vec3 intersection = intersectBilinearPatch( rayOrigin, rayDirection, p00, p01, p10, p11 );
				if ( intersection.z > (0.0 + cRayEpsilon) ) {
					vec3 itsPoint = interpolatePatch( p00, p01, p10, p11, intersection.xy );
					result.intersected = true;
					result.debug.x = 1.0;
					result.height = itsPoint.z;
					result.texCoords = ( vec2( indices ) + intersection.xy ) / uHeightmapDims.xy;
					//result.texCoords = result.texCoords * ( uHeightmapDims.xy / uRealDims.xy );
					result.normal = normalize( interpolateNormal( p00, p01, p10, p11, intersection.xy ) );
					result.t =  intersection.z;
					result.lod = 0;
					return result;
				}

				// Intersect height plane
				/*vec3 intersection = intersectHeightPlane( rayOrigin, rayDirection, t, tNext, heightSample * uHeightmapDims.z );
				result.intersected = true;
				result.debug.x = 1.0;
				result.height = rayOrigin.z + rayDirection.z * intersection.z;
				result.texCoords = intersection.xy;
				result.t = intersection.z;
				return result;*/
			}
			else if ( currentLOD == maxLOD ) {
				// Intersect bilinear patch - low res heightfields
				vec4 bPatch = texelFetch( uHeightmap, indices, currentLOD );

				float patchLodSizeX = uRealPatchSize * pow( 2, currentLOD );
				float patchLodSizeY = patchLodSizeX;

				vec3 p00 = vec3( float(indices.x)  * patchLodSizeX, float(indices.y)  *patchLodSizeY, bPatch.r * uHeightmapDims.z );
				vec3 p10 = vec3( float(indices.x+1)* patchLodSizeX, float(indices.y)  *patchLodSizeY, bPatch.g * uHeightmapDims.z );
				vec3 p01 = vec3( float(indices.x)  * patchLodSizeX, float(indices.y+1)*patchLodSizeY, bPatch.b * uHeightmapDims.z );
				vec3 p11 = vec3( float(indices.x+1)* patchLodSizeX, float(indices.y+1)*patchLodSizeY, bPatch.a * uHeightmapDims.z );

				// Check if the ray does not intersect the patch
				vec2 entranceXY = rayOrigin.xy + t * rayDirection.xy;
				vec2 entranceCoords = ( entranceXY - vec2( patchLodSizeX, patchLodSizeY ) * vec2( indices ) ) / vec2( patchLodSizeX, patchLodSizeY );
				vec3 entrancePoint = interpolatePatch( p00, p01, p10, p11, entranceCoords );
				if ( rayZt < entrancePoint.z ) {
					// No intersection actually happens because the ray passes below the patch
					// but we need to simulate a correct intersection so as to render lighting correctly
					// This may happen in a LOD transition

					result.debug.x = 1.0;
					result.intersected = true;
					result.lod = currentLOD;
					result.height = rayZt;
					result.t = t;
					result.texCoords = entranceXY / ( uRealPatchSize * uHeightmapDims.xy );
					result.normal = normalize( interpolateNormal( p00, p01, p10, p11, entranceCoords ) );
					return result;
				}

				vec3 intersection = intersectBilinearPatch( rayOrigin, rayDirection, p00, p01, p10, p11 );
				if ( intersection.z > (0.0 + cRayEpsilon) ) {
					vec3 itsPoint = interpolatePatch( p00, p01, p10, p11, intersection.xy );
					result.intersected = true;
					result.debug.x = 1.0;
					result.height = itsPoint.z;
					result.texCoords = ( vec2( indices ) + intersection.xy ) / ( uHeightmapDims.xy / vec2( patchLodSizeX, patchLodSizeY ) ) / uRealPatchSize;
					//result.texCoords = result.texCoords * ( uHeightmapDims.xy / uRealDims.xy );
					result.normal = normalize( interpolateNormal( p00, p01, p10, p11, intersection.xy ) );
					result.t =  intersection.z;
					result.lod = currentLOD;
					return result;
				}
				/*vec3 intersection = intersectHeightPlane( rayOrigin, rayDirection, t, tNext, heightSample * uHeightmapDims.z );
				result.intersected = true;
				result.debug.x = 1.0;
				result.height = rayOrigin.z + rayDirection.z * intersection.z;
				result.texCoords = intersection.xy;
				result.normal = vec3( 0, 0, 1 ); // Correct X,Y!
				result.t = intersection.z;
				result.lod = currentLOD;
				return result;*/
			}
			// Descend
			else {
				result.steps++;
				if ( rayDirection.z < 0 && rayZt > (heightSample*uHeightmapDims.z) ) { t = ((heightSample * uHeightmapDims.z ) - rayOrigin.z ) / rayDirection.z; }

				vec3 intersectionPoint = rayOrigin + t * rayDirection;
				vec2 centerPoint = uRealPatchSize * vec2( float( indices.x ) * ( uHeightmapDims.x / divLOD ) + uHeightmapDims.x / ( 2 * divLOD ),
										                 float( indices.y ) * ( uHeightmapDims.y / divLOD ) + uHeightmapDims.y / ( 2 * divLOD ) );

				currentLOD = currentLOD - 1;

				divLOD = divLOD * 2.0;

				dtx = ( txMax - txMin ) / divLOD;
				dty = ( tyMax - tyMin ) / divLOD;

				// Optimize
				indices *= 2;
				indices.x = indices.x + ( intersectionPoint.x > centerPoint.x ? 1 : 0 );
				indices.y = indices.y + ( intersectionPoint.y > centerPoint.y ? 1 : 0 );

				tx_next = txMin + ( rayDirection.x < 0 ? ( divLOD - indices.x ) : ( indices.x + 1 ) ) * dtx;
				ix_stop = ( rayDirection.x < 0 ? -1 : int(divLOD) );

				ty_next = tyMin + ( rayDirection.y < 0 ? ( divLOD - indices.y ) : ( indices.y + 1 ) ) * dty;
				iy_stop = ( rayDirection.y < 0 ? -1 : int(divLOD) );

				continue;
			}
		}
		else { // Else ascend one level in the mipmap structure
			if ( currentLOD == uLastLOD ) {
				break;
			}
			bool up = false;
			if ( axis == 0 ) { if ( ( indices.x + ix_step )/2 != indices.x/2 ) { up = true; indices.x = ( indices.x + ix_step ) / 2; indices.y /= 2; } }
			if ( axis == 1 ) { if ( ( indices.y + iy_step )/2 != indices.y/2 ) { up = true; indices.y = ( indices.y + iy_step ) / 2; indices.x /= 2; } }

			if ( up ) {
				result.steps++;
				currentLOD = currentLOD + 1;
				divLOD = divLOD / 2.0;

				dtx = ( txMax - txMin ) / divLOD;
				dty = ( tyMax - tyMin ) / divLOD;

				if ( currentLOD >= uLastLOD ) {
					return result;
				}
			
				t = tNext;

				if ( ( indices.x < 0 || indices.x >= divLOD ) || ( indices.y < 0 || indices.y >= divLOD ) ) {
					return result;
				}

				tx_next = txMin + ( rayDirection.x < 0 ? ( divLOD - indices.x ) : ( indices.x + 1 ) ) * dtx;
				ix_stop = ( rayDirection.x < 0 ? -1 : int(divLOD) );

				ty_next = tyMin + ( rayDirection.y < 0 ? ( divLOD - indices.y ) : ( indices.y + 1 ) ) * dty;
				iy_stop = ( rayDirection.y < 0 ? -1 : int(divLOD) );

				continue;
			}
		}
		
		if ( axis == 0 ) {
			tx_next += dtx;
			indices.x += ix_step;
		}
		else {
			ty_next += dty;
			indices.y += iy_step;
		}
		result.steps++;

		t = tNext;
	}

	return result;
}

HFIntersection rayTraceTerrain_DoubleLOD( vec3 rayOrigin, vec3 rayDirection ) {
	HFIntersection result;
	result.steps = 0;
	result.intersected = false;
	result.iterated = false;

	int maxLOD_descent = 0;
	int maxLOD_center = 0;

	float txMin = - rayOrigin.x / rayDirection.x;
	float tyMin = - rayOrigin.y / rayDirection.y;
	float tzMin = - rayOrigin.z / rayDirection.z;

	float txMax = ( uRealPatchSize * uHeightmapDims.x - rayOrigin.x ) / rayDirection.x;
	float tyMax = ( uRealPatchSize * uHeightmapDims.y - rayOrigin.y ) / rayDirection.y;
	float tzMax = ( uHeightmapDims.z - rayOrigin.z ) / rayDirection.z;

	if ( txMin > txMax ) { float tmp = txMax; txMax = txMin; txMin = tmp; }
	if ( tyMin > tyMax ) { float tmp = tyMax; tyMax = tyMin; tyMin = tmp; }
	if ( tzMin > tzMax ) { float tmp = tzMax; tzMax = tzMin; tzMin = tmp; }

	if ( ( txMin > tyMax ) || ( tyMin > txMax ) ) {
		return result;
	}
	float tMin, tMax;
	if ( tyMin > txMin ) { tMin = tyMin; } else { tMin = txMin; }
	if ( tyMax < txMax ) { tMax = tyMax; } else { tMax = txMax; }


	if ( ( tMin > tzMax ) || ( tzMin > tMax ) ) {
		return result;
	}
	if ( tzMin > tMin ) { tMin = tzMin; }
	if ( tzMax < tMax ) { tMax = tzMax; }

	// Now we have the ts, calculate the start and end points
	vec3 startPoint;
	if ( isPointWithinBox( rayOrigin, vec3( 0, 0, 0 ), vec3( uRealPatchSize*uHeightmapDims.xy, uHeightmapDims.z ) ) ) {
		startPoint = rayOrigin;
		tMin = 0.0f;
	}
	else {
		if ( tMin < 0 ) { return result; }
		startPoint = rayOrigin + tMin * rayDirection;
	}
	vec3 endPoint = rayOrigin + tMax * rayDirection;

	// Start at the coarsest level of the maximum mipmaps
	int currentLOD = uLastLOD;
	float divLOD = 1.0;

	float dtx = ( txMax - txMin ) / divLOD;
	float dty = ( tyMax - tyMin ) / divLOD;

	ivec2 indices = ivec2( 0, 0 );

	// Steps for traversal
	float tx_next, ty_next;
	int ix_step, iy_step;
	int ix_stop, iy_stop;

	tx_next = txMin + ( rayDirection.x < 0 ? ( divLOD - indices.x ) : ( indices.x + 1 ) ) * dtx;
	ix_step = ( rayDirection.x < 0 ? -1 : 1 );
	ix_stop = ( rayDirection.x < 0 ? -1 : int(divLOD) );

	ty_next = tyMin + ( rayDirection.y < 0 ? ( divLOD - indices.y ) : ( indices.y + 1 ) ) * dty;
	iy_step = ( rayDirection.y < 0 ? -1 : 1 );
	iy_stop = ( rayDirection.y < 0 ? -1 : int(divLOD) );

	// Find BB intersection points
	float t = tMin;
	float tNext = 0.0;
	int axis = 0;

	float patchSizeX = uRealPatchSize * uHeightmapDims.x / pow( 2.0, uLastLOD );
	float patchSizeY = uRealPatchSize * uHeightmapDims.y / pow( 2.0, uLastLOD );

	//float lodDistance = 0.0;

	result.iterated = true;
	while ( true ) {
		if ( t > uFarZ ) { return result; }
		if ( indices.x == ix_stop ) { return result; }
		if ( indices.y == iy_stop ) { return result; }

		// Ascend mipmap levels if we have gone below the maxLOD
		maxLOD_descent = lodDistanceFunction( t );
		while ( currentLOD < maxLOD_descent ) {
			// keep the same intersection t because we don't want to move backwards
			result.steps++;
			currentLOD = currentLOD + 1;
			divLOD = divLOD / 2.0;

			dtx = ( txMax - txMin ) / divLOD;
			dty = ( tyMax - tyMin ) / divLOD;

			if ( currentLOD >= uLastLOD ) {
				return result;
			}

			indices /= 2;

			tx_next = txMin + ( rayDirection.x < 0 ? ( divLOD - indices.x ) : ( indices.x + 1 ) ) * dtx;
			ix_stop = ( rayDirection.x < 0 ? -1 : int(divLOD) );

			ty_next = tyMin + ( rayDirection.y < 0 ? ( divLOD - indices.y ) : ( indices.y + 1 ) ) * dty;
			iy_stop = ( rayDirection.y < 0 ? -1 : int(divLOD) );
		}

		if ( tx_next < ty_next ) {
			tNext = tx_next;
			axis = 0;
		}
		else {
			tNext = ty_next;
			axis = 1;
		}

		// Check if ray goes below mipmap altitude - .x because the height is stored in the first channel (single channel for 16bit)
		float heightSample = texelFetch( uMaximumMips, indices, currentLOD ).x;

		float rayZt = ( rayOrigin.z + t * rayDirection.z );
		float rayZnext = ( rayOrigin.z + tNext * rayDirection.z );

		maxLOD_center = lodDistanceFunction( length( vec3( uRealPatchSize * pow( 2, currentLOD ) * ( vec2( indices ) + vec2( 0.5, 0.5 ) ), heightSample * uHeightmapDims.z ) - rayOrigin ) );

		// If ray falls below mipmap height plane we do either:
		// - Bilinear interpolation if at LOD = 0
		// - Or continue testing at a lower LOD
		if ( rayZt <= ( heightSample * uHeightmapDims.z ) || rayZnext <= ( heightSample * uHeightmapDims.z ) )  {
			// Descend
			if ( currentLOD > maxLOD_descent ) {
				result.steps++;
				if ( rayDirection.z < 0 && rayZt > (heightSample*uHeightmapDims.z) ) { t = ((heightSample * uHeightmapDims.z ) - rayOrigin.z ) / rayDirection.z; }

				vec3 intersectionPoint = rayOrigin + t * rayDirection;
				vec2 centerPoint = uRealPatchSize * vec2( float( indices.x ) * ( uHeightmapDims.x / divLOD ) + uHeightmapDims.x / ( 2 * divLOD ),
										                 float( indices.y ) * ( uHeightmapDims.y / divLOD ) + uHeightmapDims.y / ( 2 * divLOD ) );

				currentLOD = currentLOD - 1;

				divLOD = divLOD * 2.0;

				dtx = ( txMax - txMin ) / divLOD;
				dty = ( tyMax - tyMin ) / divLOD;

				// Optimize
				indices *= 2;
				indices.x = indices.x + ( intersectionPoint.x > centerPoint.x ? 1 : 0 );
				indices.y = indices.y + ( intersectionPoint.y > centerPoint.y ? 1 : 0 );

				tx_next = txMin + ( rayDirection.x < 0 ? ( divLOD - indices.x ) : ( indices.x + 1 ) ) * dtx;
				ix_stop = ( rayDirection.x < 0 ? -1 : int(divLOD) );

				ty_next = tyMin + ( rayDirection.y < 0 ? ( divLOD - indices.y ) : ( indices.y + 1 ) ) * dty;
				iy_stop = ( rayDirection.y < 0 ? -1 : int(divLOD) );

				continue;
			}
			else if ( currentLOD >= maxLOD_center ) {
				// Intersect bilinear patch - low res heightfields
				vec4 bPatch = texelFetch( uHeightmap, indices, currentLOD );

				float patchLodSizeX = uRealPatchSize * pow( 2, currentLOD );
				float patchLodSizeY = patchLodSizeX;

				vec3 p00 = vec3( float(indices.x)  * patchLodSizeX, float(indices.y)  *patchLodSizeY, bPatch.r * uHeightmapDims.z );
				vec3 p10 = vec3( float(indices.x+1)* patchLodSizeX, float(indices.y)  *patchLodSizeY, bPatch.g * uHeightmapDims.z );
				vec3 p01 = vec3( float(indices.x)  * patchLodSizeX, float(indices.y+1)*patchLodSizeY, bPatch.b * uHeightmapDims.z );
				vec3 p11 = vec3( float(indices.x+1)* patchLodSizeX, float(indices.y+1)*patchLodSizeY, bPatch.a * uHeightmapDims.z );

				// Check if the ray does not intersect the patch
				vec2 entranceXY = rayOrigin.xy + t * rayDirection.xy;
				vec2 entranceCoords = ( entranceXY - vec2( patchLodSizeX, patchLodSizeY ) * vec2( indices ) ) / vec2( patchLodSizeX, patchLodSizeY );
				vec3 entrancePoint = interpolatePatch( p00, p01, p10, p11, entranceCoords );
				if ( rayZt < entrancePoint.z ) {
					// No intersection actually happens because the ray passes below the patch
					// but we need to simulate a correct intersection so as to render lighting correctly
					// This may happen in a LOD transition

					result.debug.x = 1.0;
					result.intersected = true;
					result.lod = currentLOD;
					result.height = rayZt;
					result.t = t;
					result.texCoords = entranceXY / ( uRealPatchSize * uHeightmapDims.xy );
					result.normal = normalize( interpolateNormal( p00, p01, p10, p11, entranceCoords ) );
					return result;
				}

				vec3 intersection = intersectBilinearPatch( rayOrigin, rayDirection, p00, p01, p10, p11 );
				if ( intersection.z > (0.0 + cRayEpsilon) ) {
					vec3 itsPoint = interpolatePatch( p00, p01, p10, p11, intersection.xy );
					result.intersected = true;
					result.debug.x = 1.0;
					result.height = itsPoint.z;
					result.texCoords = ( vec2( indices ) + intersection.xy ) / ( uHeightmapDims.xy / vec2( patchLodSizeX, patchLodSizeY ) ) / uRealPatchSize;
					//result.texCoords = result.texCoords * ( uHeightmapDims.xy / uRealDims.xy );
					result.normal = normalize( interpolateNormal( p00, p01, p10, p11, intersection.xy ) );
					result.t =  intersection.z;
					result.lod = currentLOD;
					return result;
				}
				/*vec3 intersection = intersectHeightPlane( rayOrigin, rayDirection, t, tNext, heightSample * uHeightmapDims.z );
				result.intersected = true;
				result.debug.x = 1.0;
				result.height = rayOrigin.z + rayDirection.z * intersection.z;
				result.texCoords = intersection.xy;
				result.normal = vec3( 0, 0, 1 ); // Correct X,Y!
				result.t = intersection.z;
				result.lod = currentLOD;
				return result;*/
			}
		}
		else { // Else ascend one level in the mipmap structure
			if ( currentLOD == uLastLOD ) {
				break;
			}
			bool up = false;
			if ( axis == 0 ) { if ( ( indices.x + ix_step )/2 != indices.x/2 ) { up = true; indices.x = ( indices.x + ix_step ) / 2; indices.y /= 2; } }
			if ( axis == 1 ) { if ( ( indices.y + iy_step )/2 != indices.y/2 ) { up = true; indices.y = ( indices.y + iy_step ) / 2; indices.x /= 2; } }

			if ( up ) {
				result.steps++;
				currentLOD = currentLOD + 1;
				divLOD = divLOD / 2.0;

				dtx = ( txMax - txMin ) / divLOD;
				dty = ( tyMax - tyMin ) / divLOD;

				if ( currentLOD >= uLastLOD ) {
					return result;
				}
			
				t = tNext;

				if ( ( indices.x < 0 || indices.x >= divLOD ) || ( indices.y < 0 || indices.y >= divLOD ) ) {
					return result;
				}

				tx_next = txMin + ( rayDirection.x < 0 ? ( divLOD - indices.x ) : ( indices.x + 1 ) ) * dtx;
				ix_stop = ( rayDirection.x < 0 ? -1 : int(divLOD) );

				ty_next = tyMin + ( rayDirection.y < 0 ? ( divLOD - indices.y ) : ( indices.y + 1 ) ) * dty;
				iy_stop = ( rayDirection.y < 0 ? -1 : int(divLOD) );

				continue;
			}
		}
		
		if ( axis == 0 ) {
			tx_next += dtx;
			indices.x += ix_step;
		}
		else {
			ty_next += dty;
			indices.y += iy_step;
		}
		result.steps++;

		t = tNext;
	}

	return result;
}


HFIntersection rayTraceTerrain_CustomLOD( vec3 rayOrigin, vec3 rayDirection, vec3 lodCenter ) {
	HFIntersection result;
	result.steps = 0;
	result.intersected = false;
	result.iterated = false;

	int maxLOD = 0;

	float txMin = - rayOrigin.x / rayDirection.x;
	float tyMin = - rayOrigin.y / rayDirection.y;
	float tzMin = - rayOrigin.z / rayDirection.z;

	float txMax = ( uRealPatchSize * uHeightmapDims.x - rayOrigin.x ) / rayDirection.x;
	float tyMax = ( uRealPatchSize * uHeightmapDims.y - rayOrigin.y ) / rayDirection.y;
	float tzMax = ( uHeightmapDims.z - rayOrigin.z ) / rayDirection.z;

	if ( txMin > txMax ) { float tmp = txMax; txMax = txMin; txMin = tmp; }
	if ( tyMin > tyMax ) { float tmp = tyMax; tyMax = tyMin; tyMin = tmp; }
	if ( tzMin > tzMax ) { float tmp = tzMax; tzMax = tzMin; tzMin = tmp; }

	if ( ( txMin > tyMax ) || ( tyMin > txMax ) ) {
		return result;
	}
	float tMin, tMax;
	if ( tyMin > txMin ) { tMin = tyMin; } else { tMin = txMin; }
	if ( tyMax < txMax ) { tMax = tyMax; } else { tMax = txMax; }


	if ( ( tMin > tzMax ) || ( tzMin > tMax ) ) {
		return result;
	}
	if ( tzMin > tMin ) { tMin = tzMin; }
	if ( tzMax < tMax ) { tMax = tzMax; }

	// Now we have the ts, calculate the start and end points
	vec3 startPoint;
	if ( isPointWithinBox( rayOrigin, vec3( 0, 0, 0 ), vec3( uRealPatchSize*uHeightmapDims.xy, uHeightmapDims.z ) ) ) {
		startPoint = rayOrigin;
		tMin = 0.0f;
	}
	else {
		if ( tMin < 0 ) { return result; }
		startPoint = rayOrigin + tMin * rayDirection;
	}
	vec3 endPoint = rayOrigin + tMax * rayDirection;

	// Start at the coarsest level of the maximum mipmaps
	int currentLOD = uLastLOD;
	float divLOD = 1.0;

	float dtx = ( txMax - txMin ) / divLOD;
	float dty = ( tyMax - tyMin ) / divLOD;

	ivec2 indices = ivec2( 0, 0 );

	// Steps for traversal
	float tx_next, ty_next;
	int ix_step, iy_step;
	int ix_stop, iy_stop;

	tx_next = txMin + ( rayDirection.x < 0 ? ( divLOD - indices.x ) : ( indices.x + 1 ) ) * dtx;
	ix_step = ( rayDirection.x < 0 ? -1 : 1 );
	ix_stop = ( rayDirection.x < 0 ? -1 : int(divLOD) );

	ty_next = tyMin + ( rayDirection.y < 0 ? ( divLOD - indices.y ) : ( indices.y + 1 ) ) * dty;
	iy_step = ( rayDirection.y < 0 ? -1 : 1 );
	iy_stop = ( rayDirection.y < 0 ? -1 : int(divLOD) );

	// Find BB intersection points
	float t = tMin;
	float tNext = 0.0;
	int axis = 0;

	float patchSizeX = uRealPatchSize * uHeightmapDims.x / pow( 2.0, uLastLOD );
	float patchSizeY = uRealPatchSize * uHeightmapDims.y / pow( 2.0, uLastLOD );

	//float lodDistance = 0.0;

	result.iterated = true;
	while ( true ) {
		if ( t > uFarZ ) { return result; }
		if ( indices.x == ix_stop ) { return result; }
		if ( indices.y == iy_stop ) { return result; }

		// Ascend mipmap levels if we have gone below the maxLOD
		maxLOD = lodDistanceFunction( length( ( rayOrigin + t * rayDirection ) - lodCenter ) );
		while ( currentLOD < maxLOD ) {
			// keep the same intersection t because we don't want to move backwards
			result.steps++;
			currentLOD = currentLOD + 1;
			divLOD = divLOD / 2.0;

			dtx = ( txMax - txMin ) / divLOD;
			dty = ( tyMax - tyMin ) / divLOD;

			if ( currentLOD >= uLastLOD ) {
				return result;
			}

			indices /= 2;

			tx_next = txMin + ( rayDirection.x < 0 ? ( divLOD - indices.x ) : ( indices.x + 1 ) ) * dtx;
			ix_stop = ( rayDirection.x < 0 ? -1 : int(divLOD) );

			ty_next = tyMin + ( rayDirection.y < 0 ? ( divLOD - indices.y ) : ( indices.y + 1 ) ) * dty;
			iy_stop = ( rayDirection.y < 0 ? -1 : int(divLOD) );
		}

		if ( tx_next < ty_next ) {
			tNext = tx_next;
			axis = 0;
		}
		else {
			tNext = ty_next;
			axis = 1;
		}

		// Check if ray goes below mipmap altitude - .x because the height is stored in the first channel (single channel for 16bit)
		float heightSample = texelFetch( uMaximumMips, indices, currentLOD ).x;

		float rayZt = ( rayOrigin.z + t * rayDirection.z );
		float rayZnext = ( rayOrigin.z + tNext * rayDirection.z );

		// If ray falls below mipmap height plane we do either:
		// - Bilinear interpolation if at LOD = 0
		// - Or continue testing at a lower LOD
		if ( rayZt <= ( heightSample * uHeightmapDims.z ) || rayZnext <= ( heightSample * uHeightmapDims.z ) )  {
			if ( currentLOD == 0 ) {
				// Intersect bilinear patch
				vec4 bPatch = texelFetch( uHeightmap, indices, 0 );

				vec3 p00 = vec3( float(indices.x)  * uRealPatchSize, float(indices.y)  *uRealPatchSize, bPatch.r * uHeightmapDims.z );
				vec3 p10 = vec3( float(indices.x+1)* uRealPatchSize, float(indices.y)  *uRealPatchSize, bPatch.g * uHeightmapDims.z );
				vec3 p01 = vec3( float(indices.x)  * uRealPatchSize, float(indices.y+1)*uRealPatchSize, bPatch.b * uHeightmapDims.z );
				vec3 p11 = vec3( float(indices.x+1)* uRealPatchSize, float(indices.y+1)*uRealPatchSize, bPatch.a * uHeightmapDims.z );

				// Check if the ray does not intersect the patch
				vec2 entranceXY = rayOrigin.xy + t * rayDirection.xy;
				vec2 entranceCoords = ( entranceXY - uRealPatchSize * vec2( indices ) ) / uRealPatchSize;
				vec3 entrancePoint = interpolatePatch( p00, p01, p10, p11, entranceCoords );
				if ( rayZt < entrancePoint.z ) {
					// No intersection actually happens because the ray passes below the patch
					// but we need to simulate a correct intersection to fill transition gaps

					result.debug.x = 1.0;
					result.intersected = true;
					result.lod = 0;
					result.height = rayZt;
					result.t = t;
					result.texCoords = entranceXY / ( uRealPatchSize * uHeightmapDims.xy );
					result.normal = normalize( interpolateNormal( p00, p01, p10, p11, entranceCoords ) );
					return result;
				}

				vec3 intersection = intersectBilinearPatch( rayOrigin, rayDirection, p00, p01, p10, p11 );
				if ( intersection.z > (0.0 + cRayEpsilon) ) {
					vec3 itsPoint = interpolatePatch( p00, p01, p10, p11, intersection.xy );
					result.intersected = true;
					result.debug.x = 1.0;
					result.height = itsPoint.z;
					result.texCoords = ( vec2( indices ) + intersection.xy ) / uHeightmapDims.xy;
					//result.texCoords = result.texCoords * ( uHeightmapDims.xy / uRealDims.xy );
					result.normal = normalize( interpolateNormal( p00, p01, p10, p11, intersection.xy ) );
					result.t =  intersection.z;
					result.lod = 0;
					return result;
				}

				// Intersect height plane
				/*vec3 intersection = intersectHeightPlane( rayOrigin, rayDirection, t, tNext, heightSample * uHeightmapDims.z );
				result.intersected = true;
				result.debug.x = 1.0;
				result.height = rayOrigin.z + rayDirection.z * intersection.z;
				result.texCoords = intersection.xy;
				result.t = intersection.z;
				return result;*/
			}
			else if ( currentLOD == maxLOD ) {
				// Intersect bilinear patch - low res heightfields
				vec4 bPatch = texelFetch( uHeightmap, indices, currentLOD );

				float patchLodSizeX = uRealPatchSize * pow( 2, currentLOD );
				float patchLodSizeY = patchLodSizeX;

				vec3 p00 = vec3( float(indices.x)  * patchLodSizeX, float(indices.y)  *patchLodSizeY, bPatch.r * uHeightmapDims.z );
				vec3 p10 = vec3( float(indices.x+1)* patchLodSizeX, float(indices.y)  *patchLodSizeY, bPatch.g * uHeightmapDims.z );
				vec3 p01 = vec3( float(indices.x)  * patchLodSizeX, float(indices.y+1)*patchLodSizeY, bPatch.b * uHeightmapDims.z );
				vec3 p11 = vec3( float(indices.x+1)* patchLodSizeX, float(indices.y+1)*patchLodSizeY, bPatch.a * uHeightmapDims.z );

				// Check if the ray does not intersect the patch
				vec2 entranceXY = rayOrigin.xy + t * rayDirection.xy;
				vec2 entranceCoords = ( entranceXY - vec2( patchLodSizeX, patchLodSizeY ) * vec2( indices ) ) / vec2( patchLodSizeX, patchLodSizeY );
				vec3 entrancePoint = interpolatePatch( p00, p01, p10, p11, entranceCoords );
				if ( rayZt < entrancePoint.z ) {
					// No intersection actually happens because the ray passes below the patch
					// but we need to simulate a correct intersection so as to render lighting correctly
					// This may happen in a LOD transition

					result.debug.x = 1.0;
					result.intersected = true;
					result.lod = currentLOD;
					result.height = rayZt;
					result.t = t;
					result.texCoords = entranceXY / ( uRealPatchSize * uHeightmapDims.xy );
					result.normal = normalize( interpolateNormal( p00, p01, p10, p11, entranceCoords ) );
					return result;
				}

				vec3 intersection = intersectBilinearPatch( rayOrigin, rayDirection, p00, p01, p10, p11 );
				if ( intersection.z > (0.0 + cRayEpsilon) ) {
					vec3 itsPoint = interpolatePatch( p00, p01, p10, p11, intersection.xy );
					result.intersected = true;
					result.debug.x = 1.0;
					result.height = itsPoint.z;
					result.texCoords = ( vec2( indices ) + intersection.xy ) / ( uHeightmapDims.xy / vec2( patchLodSizeX, patchLodSizeY ) ) / uRealPatchSize;
					//result.texCoords = result.texCoords * ( uHeightmapDims.xy / uRealDims.xy );
					result.normal = normalize( interpolateNormal( p00, p01, p10, p11, intersection.xy ) );
					result.t =  intersection.z;
					result.lod = currentLOD;
					return result;
				}
				/*vec3 intersection = intersectHeightPlane( rayOrigin, rayDirection, t, tNext, heightSample * uHeightmapDims.z );
				result.intersected = true;
				result.debug.x = 1.0;
				result.height = rayOrigin.z + rayDirection.z * intersection.z;
				result.texCoords = intersection.xy;
				result.normal = vec3( 0, 0, 1 ); // Correct X,Y!
				result.t = intersection.z;
				result.lod = currentLOD;
				return result;*/
			}
			// Descend
			else {
				result.steps++;
				if ( rayDirection.z < 0 && rayZt > (heightSample*uHeightmapDims.z) ) { t = ((heightSample * uHeightmapDims.z ) - rayOrigin.z ) / rayDirection.z; }

				vec3 intersectionPoint = rayOrigin + t * rayDirection;
				vec2 centerPoint = uRealPatchSize * vec2( float( indices.x ) * ( uHeightmapDims.x / divLOD ) + uHeightmapDims.x / ( 2 * divLOD ),
										                 float( indices.y ) * ( uHeightmapDims.y / divLOD ) + uHeightmapDims.y / ( 2 * divLOD ) );

				currentLOD = currentLOD - 1;

				divLOD = divLOD * 2.0;

				dtx = ( txMax - txMin ) / divLOD;
				dty = ( tyMax - tyMin ) / divLOD;

				// Optimize
				indices *= 2;
				indices.x = indices.x + ( intersectionPoint.x > centerPoint.x ? 1 : 0 );
				indices.y = indices.y + ( intersectionPoint.y > centerPoint.y ? 1 : 0 );

				tx_next = txMin + ( rayDirection.x < 0 ? ( divLOD - indices.x ) : ( indices.x + 1 ) ) * dtx;
				ix_stop = ( rayDirection.x < 0 ? -1 : int(divLOD) );

				ty_next = tyMin + ( rayDirection.y < 0 ? ( divLOD - indices.y ) : ( indices.y + 1 ) ) * dty;
				iy_stop = ( rayDirection.y < 0 ? -1 : int(divLOD) );

				continue;
			}
		}
		else { // Else ascend one level in the mipmap structure
			if ( currentLOD == uLastLOD ) {
				break;
			}
			bool up = false;
			if ( axis == 0 ) { if ( ( indices.x + ix_step )/2 != indices.x/2 ) { up = true; indices.x = ( indices.x + ix_step ) / 2; indices.y /= 2; } }
			if ( axis == 1 ) { if ( ( indices.y + iy_step )/2 != indices.y/2 ) { up = true; indices.y = ( indices.y + iy_step ) / 2; indices.x /= 2; } }

			if ( up ) {
				result.steps++;
				currentLOD = currentLOD + 1;
				divLOD = divLOD / 2.0;

				dtx = ( txMax - txMin ) / divLOD;
				dty = ( tyMax - tyMin ) / divLOD;

				if ( currentLOD >= uLastLOD ) {
					return result;
				}
			
				t = tNext;

				if ( ( indices.x < 0 || indices.x >= divLOD ) || ( indices.y < 0 || indices.y >= divLOD ) ) {
					return result;
				}

				tx_next = txMin + ( rayDirection.x < 0 ? ( divLOD - indices.x ) : ( indices.x + 1 ) ) * dtx;
				ix_stop = ( rayDirection.x < 0 ? -1 : int(divLOD) );

				ty_next = tyMin + ( rayDirection.y < 0 ? ( divLOD - indices.y ) : ( indices.y + 1 ) ) * dty;
				iy_stop = ( rayDirection.y < 0 ? -1 : int(divLOD) );

				continue;
			}
		}
		
		if ( axis == 0 ) {
			tx_next += dtx;
			indices.x += ix_step;
		}
		else {
			ty_next += dty;
			indices.y += iy_step;
		}
		result.steps++;

		t = tNext;
	}

	return result;
}

HFIntersection rayTraceTerrain_DCustomLOD( vec3 rayOrigin, vec3 rayDirection, vec3 lodCenter ) {
	HFIntersection result;
	result.steps = 0;
	result.intersected = false;
	result.iterated = false;

	int maxLOD_descent = 0;
	int maxLOD_center = 0;

	float txMin = - rayOrigin.x / rayDirection.x;
	float tyMin = - rayOrigin.y / rayDirection.y;
	float tzMin = - rayOrigin.z / rayDirection.z;

	float txMax = ( uRealPatchSize * uHeightmapDims.x - rayOrigin.x ) / rayDirection.x;
	float tyMax = ( uRealPatchSize * uHeightmapDims.y - rayOrigin.y ) / rayDirection.y;
	float tzMax = ( uHeightmapDims.z - rayOrigin.z ) / rayDirection.z;

	if ( txMin > txMax ) { float tmp = txMax; txMax = txMin; txMin = tmp; }
	if ( tyMin > tyMax ) { float tmp = tyMax; tyMax = tyMin; tyMin = tmp; }
	if ( tzMin > tzMax ) { float tmp = tzMax; tzMax = tzMin; tzMin = tmp; }

	if ( ( txMin > tyMax ) || ( tyMin > txMax ) ) {
		return result;
	}
	float tMin, tMax;
	if ( tyMin > txMin ) { tMin = tyMin; } else { tMin = txMin; }
	if ( tyMax < txMax ) { tMax = tyMax; } else { tMax = txMax; }


	if ( ( tMin > tzMax ) || ( tzMin > tMax ) ) {
		return result;
	}
	if ( tzMin > tMin ) { tMin = tzMin; }
	if ( tzMax < tMax ) { tMax = tzMax; }

	// Now we have the ts, calculate the start and end points
	vec3 startPoint;
	if ( isPointWithinBox( rayOrigin, vec3( 0, 0, 0 ), vec3( uRealPatchSize*uHeightmapDims.xy, uHeightmapDims.z ) ) ) {
		startPoint = rayOrigin;
		tMin = 0.0f;
	}
	else {
		if ( tMin < 0 ) { return result; }
		startPoint = rayOrigin + tMin * rayDirection;
	}
	vec3 endPoint = rayOrigin + tMax * rayDirection;

	// Start at the coarsest level of the maximum mipmaps
	int currentLOD = uLastLOD;
	float divLOD = 1.0;

	float dtx = ( txMax - txMin ) / divLOD;
	float dty = ( tyMax - tyMin ) / divLOD;

	ivec2 indices = ivec2( 0, 0 );

	// Steps for traversal
	float tx_next, ty_next;
	int ix_step, iy_step;
	int ix_stop, iy_stop;

	tx_next = txMin + ( rayDirection.x < 0 ? ( divLOD - indices.x ) : ( indices.x + 1 ) ) * dtx;
	ix_step = ( rayDirection.x < 0 ? -1 : 1 );
	ix_stop = ( rayDirection.x < 0 ? -1 : int(divLOD) );

	ty_next = tyMin + ( rayDirection.y < 0 ? ( divLOD - indices.y ) : ( indices.y + 1 ) ) * dty;
	iy_step = ( rayDirection.y < 0 ? -1 : 1 );
	iy_stop = ( rayDirection.y < 0 ? -1 : int(divLOD) );

	// Find BB intersection points
	float t = tMin;
	float tNext = 0.0;
	int axis = 0;

	float patchSizeX = uRealPatchSize * uHeightmapDims.x / pow( 2.0, uLastLOD );
	float patchSizeY = uRealPatchSize * uHeightmapDims.y / pow( 2.0, uLastLOD );

	//float lodDistance = 0.0;

	result.iterated = true;
	while ( true ) {
		if ( t > uFarZ ) { return result; }
		if ( indices.x == ix_stop ) { return result; }
		if ( indices.y == iy_stop ) { return result; }

		// Ascend mipmap levels if we have gone below the maxLOD
		maxLOD_descent = lodDistanceFunction( length( ( rayOrigin + t * rayDirection ) - lodCenter ) );
		while ( currentLOD < maxLOD_descent ) {
			// keep the same intersection t because we don't want to move backwards
			result.steps++;
			currentLOD = currentLOD + 1;
			divLOD = divLOD / 2.0;

			dtx = ( txMax - txMin ) / divLOD;
			dty = ( tyMax - tyMin ) / divLOD;

			if ( currentLOD >= uLastLOD ) {
				return result;
			}

			indices /= 2;

			tx_next = txMin + ( rayDirection.x < 0 ? ( divLOD - indices.x ) : ( indices.x + 1 ) ) * dtx;
			ix_stop = ( rayDirection.x < 0 ? -1 : int(divLOD) );

			ty_next = tyMin + ( rayDirection.y < 0 ? ( divLOD - indices.y ) : ( indices.y + 1 ) ) * dty;
			iy_stop = ( rayDirection.y < 0 ? -1 : int(divLOD) );
		}

		if ( tx_next < ty_next ) {
			tNext = tx_next;
			axis = 0;
		}
		else {
			tNext = ty_next;
			axis = 1;
		}

		// Check if ray goes below mipmap altitude - .x because the height is stored in the first channel (single channel for 16bit)
		float heightSample = texelFetch( uMaximumMips, indices, currentLOD ).x;

		float rayZt = ( rayOrigin.z + t * rayDirection.z );
		float rayZnext = ( rayOrigin.z + tNext * rayDirection.z );

		maxLOD_center = lodDistanceFunction( length( vec3( uRealPatchSize * pow( 2, currentLOD ) * ( vec2( indices ) + vec2( 0.5, 0.5 ) ), heightSample * uHeightmapDims.z ) - rayOrigin ) );

		// If ray falls below mipmap height plane we do either:
		// - Bilinear interpolation if at LOD = 0
		// - Or continue testing at a lower LOD
		if ( rayZt <= ( heightSample * uHeightmapDims.z ) || rayZnext <= ( heightSample * uHeightmapDims.z ) )  {
			// Descend
			if ( currentLOD > maxLOD_descent ) {
				result.steps++;
				if ( rayDirection.z < 0 && rayZt > (heightSample*uHeightmapDims.z) ) { t = ((heightSample * uHeightmapDims.z ) - rayOrigin.z ) / rayDirection.z; }

				vec3 intersectionPoint = rayOrigin + t * rayDirection;
				vec2 centerPoint = uRealPatchSize * vec2( float( indices.x ) * ( uHeightmapDims.x / divLOD ) + uHeightmapDims.x / ( 2 * divLOD ),
										                 float( indices.y ) * ( uHeightmapDims.y / divLOD ) + uHeightmapDims.y / ( 2 * divLOD ) );

				currentLOD = currentLOD - 1;

				divLOD = divLOD * 2.0;

				dtx = ( txMax - txMin ) / divLOD;
				dty = ( tyMax - tyMin ) / divLOD;

				// Optimize
				indices *= 2;
				indices.x = indices.x + ( intersectionPoint.x > centerPoint.x ? 1 : 0 );
				indices.y = indices.y + ( intersectionPoint.y > centerPoint.y ? 1 : 0 );

				tx_next = txMin + ( rayDirection.x < 0 ? ( divLOD - indices.x ) : ( indices.x + 1 ) ) * dtx;
				ix_stop = ( rayDirection.x < 0 ? -1 : int(divLOD) );

				ty_next = tyMin + ( rayDirection.y < 0 ? ( divLOD - indices.y ) : ( indices.y + 1 ) ) * dty;
				iy_stop = ( rayDirection.y < 0 ? -1 : int(divLOD) );

				continue;
			}
			else if ( currentLOD >= maxLOD_center ) {
				// Intersect bilinear patch - low res heightfields
				vec4 bPatch = texelFetch( uHeightmap, indices, currentLOD );

				float patchLodSizeX = uRealPatchSize * pow( 2, currentLOD );
				float patchLodSizeY = patchLodSizeX;

				vec3 p00 = vec3( float(indices.x)  * patchLodSizeX, float(indices.y)  *patchLodSizeY, bPatch.r * uHeightmapDims.z );
				vec3 p10 = vec3( float(indices.x+1)* patchLodSizeX, float(indices.y)  *patchLodSizeY, bPatch.g * uHeightmapDims.z );
				vec3 p01 = vec3( float(indices.x)  * patchLodSizeX, float(indices.y+1)*patchLodSizeY, bPatch.b * uHeightmapDims.z );
				vec3 p11 = vec3( float(indices.x+1)* patchLodSizeX, float(indices.y+1)*patchLodSizeY, bPatch.a * uHeightmapDims.z );

				// Check if the ray does not intersect the patch
				vec2 entranceXY = rayOrigin.xy + t * rayDirection.xy;
				vec2 entranceCoords = ( entranceXY - vec2( patchLodSizeX, patchLodSizeY ) * vec2( indices ) ) / vec2( patchLodSizeX, patchLodSizeY );
				vec3 entrancePoint = interpolatePatch( p00, p01, p10, p11, entranceCoords );
				if ( rayZt < entrancePoint.z ) {
					// No intersection actually happens because the ray passes below the patch
					// but we need to simulate a correct intersection so as to render lighting correctly
					// This may happen in a LOD transition

					result.debug.x = 1.0;
					result.intersected = true;
					result.lod = currentLOD;
					result.height = rayZt;
					result.t = t;
					result.texCoords = entranceXY / ( uRealPatchSize * uHeightmapDims.xy );
					result.normal = normalize( interpolateNormal( p00, p01, p10, p11, entranceCoords ) );
					return result;
				}

				vec3 intersection = intersectBilinearPatch( rayOrigin, rayDirection, p00, p01, p10, p11 );
				if ( intersection.z > (0.0 + cRayEpsilon) ) {
					vec3 itsPoint = interpolatePatch( p00, p01, p10, p11, intersection.xy );
					result.intersected = true;
					result.debug.x = 1.0;
					result.height = itsPoint.z;
					result.texCoords = ( vec2( indices ) + intersection.xy ) / ( uHeightmapDims.xy / vec2( patchLodSizeX, patchLodSizeY ) ) / uRealPatchSize;
					//result.texCoords = result.texCoords * ( uHeightmapDims.xy / uRealDims.xy );
					result.normal = normalize( interpolateNormal( p00, p01, p10, p11, intersection.xy ) );
					result.t =  intersection.z;
					result.lod = currentLOD;
					return result;
				}
				/*vec3 intersection = intersectHeightPlane( rayOrigin, rayDirection, t, tNext, heightSample * uHeightmapDims.z );
				result.intersected = true;
				result.debug.x = 1.0;
				result.height = rayOrigin.z + rayDirection.z * intersection.z;
				result.texCoords = intersection.xy;
				result.normal = vec3( 0, 0, 1 ); // Correct X,Y!
				result.t = intersection.z;
				result.lod = currentLOD;
				return result;*/
			}
		}
		else { // Else ascend one level in the mipmap structure
			if ( currentLOD == uLastLOD ) {
				break;
			}
			bool up = false;
			if ( axis == 0 ) { if ( ( indices.x + ix_step )/2 != indices.x/2 ) { up = true; indices.x = ( indices.x + ix_step ) / 2; indices.y /= 2; } }
			if ( axis == 1 ) { if ( ( indices.y + iy_step )/2 != indices.y/2 ) { up = true; indices.y = ( indices.y + iy_step ) / 2; indices.x /= 2; } }

			if ( up ) {
				result.steps++;
				currentLOD = currentLOD + 1;
				divLOD = divLOD / 2.0;

				dtx = ( txMax - txMin ) / divLOD;
				dty = ( tyMax - tyMin ) / divLOD;

				if ( currentLOD >= uLastLOD ) {
					return result;
				}
			
				t = tNext;

				if ( ( indices.x < 0 || indices.x >= divLOD ) || ( indices.y < 0 || indices.y >= divLOD ) ) {
					return result;
				}

				tx_next = txMin + ( rayDirection.x < 0 ? ( divLOD - indices.x ) : ( indices.x + 1 ) ) * dtx;
				ix_stop = ( rayDirection.x < 0 ? -1 : int(divLOD) );

				ty_next = tyMin + ( rayDirection.y < 0 ? ( divLOD - indices.y ) : ( indices.y + 1 ) ) * dty;
				iy_stop = ( rayDirection.y < 0 ? -1 : int(divLOD) );

				continue;
			}
		}
		
		if ( axis == 0 ) {
			tx_next += dtx;
			indices.x += ix_step;
		}
		else {
			ty_next += dty;
			indices.y += iy_step;
		}
		result.steps++;

		t = tNext;
	}

	return result;
}


//=============================================================================================
//=============================================================================================
// SHADING ROUTINES

vec3 sampleColor( HFIntersection hfInt ) {
	return texture( uPhotoTex, hfInt.texCoords * ( uHeightmapDims.xy / uRealDims.xy ) ).rgb;
}
vec4 sampleGrid( HFIntersection hfInt ) {
	vec2 uvUnorm = ( hfInt.texCoords * ( uHeightmapDims.xy / pow( 2.0, hfInt.lod ) ) );
	uvUnorm.x = uvUnorm.x - float( int( uvUnorm.x ) );
	uvUnorm.y = uvUnorm.y - float( int( uvUnorm.y ) );

	if ( ( uvUnorm.x < 0.1 || uvUnorm.x > 0.9 ) || ( uvUnorm.y < 0.1 || uvUnorm.y > 0.9 ) ) {
		return vec4( 0, 0, 0, 1 );
	}
	return vec4( 1, 1, 1, 1 );
}

subroutine vec4 ShadingType( HFIntersection hfInt );
subroutine uniform ShadingType ShadingFunc;

subroutine (ShadingType)
vec4 depthShading( HFIntersection hfInt ) {
	float depth = clamp( hfInt.t, 0.0, uFarZ ) / uFarZ;
	depth = 1.0 - depth;
	depth *= depth * depth;
	return vec4( depth, depth, depth, 1.0 );
}
subroutine (ShadingType)
vec4 photoShading( HFIntersection hfInt ) {
	// No Lighting
	return vec4( texture( uPhotoTex, hfInt.texCoords * ( uHeightmapDims.xy / uRealDims.xy ) ).rgb, 1 );

	// Basic Lighting
	/*vec3 colorAtPoint = sampleColor( hfInt );

	vec3 lightDirection = normalize( uLightDirection );

	vec3 normal = texture( uNormalMap, hfInt.texCoords ).rgb;
	normal = normalize( 2.0 * ( normal - vec3( 0.5, 0.5, 0.5 ) ) );

	float inc = max( dot( -lightDirection, normal ), 0.2 );
	return vec4( colorAtPoint*inc, 1 );*/
}
subroutine (ShadingType)
vec4 heightShading( HFIntersection hfInt ) {
	return vec4( hfInt.height /uHeightmapDims.z, hfInt.height /uHeightmapDims.z, hfInt.height/uHeightmapDims.z, 1.0 );
}
subroutine (ShadingType)
vec4 gridShading( HFIntersection hfInt ) {
	vec2 uvUnorm = ( hfInt.texCoords * ( uHeightmapDims.xy / pow( 2.0, hfInt.lod ) ) );
	uvUnorm.x = uvUnorm.x - float( int( uvUnorm.x ) );
	uvUnorm.y = uvUnorm.y - float( int( uvUnorm.y ) );

	if ( ( uvUnorm.x < 0.1 || uvUnorm.x > 0.9 ) || ( uvUnorm.y < 0.1 || uvUnorm.y > 0.9 ) ) {
		return vec4( 0, 0, 0, 1 );
	}
	// else

	return photoShading( hfInt );
	//return heightShading( hfInt );
}
subroutine (ShadingType)
vec4 lodShading( HFIntersection hfInt ) {
	vec4 gridColor = gridShading( hfInt );
	
	vec4 lodColor;
	if ( hfInt.lod == 0 ) {
		lodColor = vec4( 0, 1, 0, 1 );
	}
	else if ( hfInt.lod == 1 ) {
		lodColor = vec4( 1, 1, 0, 1 );
	}
	else if ( hfInt.lod == 2 ) {
		lodColor = vec4( 1, 0, 0, 1 );
	}
	else if ( hfInt.lod == 3 ) {
		lodColor = vec4( 1, 0, 1, 1 );
	}
	else {
		lodColor = vec4( 0, 0.25, 1, 1 );
	}
	//return lodColor;
	//return lodColor * gridColor;
	vec4 photoColor = photoShading( hfInt );
	return ( 0.5 * photoColor + 0.5 * lodColor );
}

subroutine (ShadingType)
vec4 lightingShading( HFIntersection hfInt ) {
	vec3 colorAtPoint = sampleColor( hfInt );

	vec3 intersectionPoint = uCameraEye + normalize( rayDirection ) * hfInt.t;

	vec3 lightDirection = normalize( uLightDirection );
	//vec3 lightDirection = normalize( vec3( -1.0, -0.2, -0.6 ) );

	vec3 normal = texture( uNormalMap, hfInt.texCoords ).rgb;
	normal = normalize( 2.0 * ( normal - vec3( 0.5, 0.5, 0.5 ) ) );

	// Use intersection normal to set the origin of the ray away from the surface
	// Use the normal map normal to calculate the lighting

	// Normal Rendering
	//HFIntersection shadowFeeler = rayTraceTerrain_DCustomLOD( intersectionPoint + 0.01 * hfInt.normal, -lightDirection, uCameraEye );
	
	// Frozen Rendering
	//HFIntersection shadowFeeler = rayTraceTerrain_DCustomLOD( intersectionPoint + 0.01 * hfInt.normal, -lightDirection, vec3( 2068, 672, 145 )  );


	//HFIntersection shadowFeeler = rayTraceTerrain_CustomLOD( intersectionPoint + 0.08 * pow( float(hfInt.lod+1),2.0) * hfInt.normal, -lightDirection, uCameraEye );
	HFIntersection shadowFeeler = rayTraceTerrain_CustomLOD( intersectionPoint + 0.01 * hfInt.normal, -lightDirection, uCameraEye );
	//HFIntersection shadowFeeler = rayTraceTerrain( intersectionPoint + 0.02 * pow( float(hfInt.lod+1),2.0) * hfInt.normal, -lightDirection );
	//HFIntersection shadowFeeler = rayTraceTerrain( intersectionPoint + 0.01 * (hfInt.lod+1) * hfInt.normal, -lightDirection );
	//HFIntersection shadowFeeler = rayTraceTerrain( intersectionPoint + 0.02 * hfInt.normal, -lightDirection );

	// DEBUG NUMBER OF RAYS TRACED
	if ( shadowFeeler.iterated) { atomicCounterIncrement( totalRays ); }

	if ( shadowFeeler.intersected ) {
		return vec4( colorAtPoint * 0.2, 1.0 );
	}
	else {
		// * pow( (200.0-float(shadowFeeler.steps))/200.0, 2.0 )
		float inc = max( dot( -lightDirection, normal ), 0.2 );
		return vec4( colorAtPoint*inc, 1 );
	}
	return vec4( colorAtPoint, 1.0 );
}
subroutine (ShadingType)
vec4 lightingGridShading( HFIntersection hfInt ) {
	return lightingShading( hfInt ) * sampleGrid( hfInt );
	//return ( hfInt.iterated ? vec4( 1, 0, 0, 1 ) : vec4( 0, 0, 0, 1 ) );
}
subroutine (ShadingType)
vec4 normalShading( HFIntersection hfInt ) {
	return vec4( normalize( texture( uNormalMap, hfInt.texCoords ).rgb ), 1.0 );
	//return vec4( ( hfInt.normal + vec3( 1.0, 1.0, 1.0 ) ) / 2.0, 1 );
}

subroutine (ShadingType)
vec4 stepHeatmapShading( HFIntersection hfInt ) {
	return vec4( stepGradient( hfInt.steps ), 1.0 );
	/*float steps = float( hfInt.steps );
	if ( steps < 128 ) {
		return mix( vec4( 0.3, 0, 0, 1 ), vec4( 1, 1, 0, 1 ), steps/128 );
	}
	if ( steps < 256 ) {
		//return mix( vec4( 1, 1, 0, 1 ), vec4( 1, 0, 1, 1 ), (steps-128)/128 );
		return vec4( 1, 0, 1, 1 );
	}
	return vec4( 1, 0, 1, 1 );*/
}

subroutine (ShadingType)
vec4 customShading( HFIntersection hfInt ) {
	//return mix( vec4( 1, 0, 0, 1 ), vec4( 1, 1, 0, 1 ), hfInt.height / uHeightmapDims.z );

	vec3 intersectionPoint = uCameraEye + normalize( rayDirection ) * hfInt.t;
	vec3 lightDirection = normalize( uLightDirection );
	//HFIntersection shadowTest = rayTraceTerrain_DCustomLOD( intersectionPoint + 0.01 * hfInt.normal, -lightDirection, uCameraEye );
	//HFIntersection shadowTest = rayTraceTerrain( intersectionPoint + 0.08 * pow( float(hfInt.lod+1),2.0) * hfInt.normal, -lightDirection );
	HFIntersection shadowTest = rayTraceTerrain_CustomLOD( intersectionPoint + 0.08 * pow( float(hfInt.lod+1),2.0) * hfInt.normal, -lightDirection, uCameraEye );

	// DEBUG NUMBER OF RAYS TRACED
	if ( shadowTest.iterated) { atomicCounterIncrement( totalRays ); }

	return vec4( stepGradient( hfInt.steps + shadowTest.steps ), 1.0 );
	/*float totalSteps = float( hfInt.steps + shadowTest.steps );
		if ( totalSteps < 128 ) {
		return mix( vec4( 0.3, 0, 0, 1 ), vec4( 1, 1, 0, 1 ), totalSteps/128 );
	}
	if ( totalSteps < 256 ) {
		return mix( vec4( 1, 1, 0, 1 ), vec4( 1, 0, 1, 1 ), (totalSteps-128)/128 );
		//return vec4( 1, 0, 1, 1 );
	}
	return vec4( 1, 0, 1, 1 );*/
}

//=============================================================================================
//=============================================================================================


//color subroutine: number of steps
//color subroutine:

vec3 calculateRayDirection( vec2 screenPos ) {
	vec3 cRay;
	cRay.x = ( 2 * (screenPos.x * ( uScreenDims.x - 1.0 ) + 0.5 )/ uScreenDims.x - 1.0 ) * uScreenRatios.x; // U
	cRay.y = ( 2 * (screenPos.y * ( uScreenDims.y - 1.0 ) + 0.5 )/ uScreenDims.y - 1.0 ) * uScreenRatios.y; // V
	cRay.z = 1.0f;

    vec3 dir = cRay.x * uU + cRay.y * uV + cRay.z * uN;

    return normalize( dir );
}

void main() {
	/*vec3 fragRayDirection = calculateRayDirection( texCoords );
	HFIntersection hfInt = rayTraceTerrain( uCameraEye, fragRayDirection );
	// DEBUG NUMBER OF RAYS TRAVERSED
	if ( hfInt.iterated ) { atomicCounterIncrement( totalRays ); }
	if ( !hfInt.intersected ) {
		outColor = calculateSkyColor( rayDirection );
	}
	else {
		outColor = ShadingFunc( hfInt );
	}*/
	
	// Normal Rendering
	HFIntersection hfInt = rayTraceTerrain_DoubleLOD( uCameraEye, normalize( rayDirection ) );

	// Frozen LOD
	//HFIntersection hfInt = rayTraceTerrain_DCustomLOD( uCameraEye, normalize( rayDirection ), vec3( 2068, 672, 145 ) );

	// Old function
	//HFIntersection hfInt = rayTraceTerrain( uCameraEye, normalize( rayDirection ) );
	//HFIntersection hfInt = rayTraceTerrain_CustomLOD( uCameraEye, normalize( rayDirection ), vec3( 2068, 672, 145 ) );
	
	// DEBUG NUMBER OF RAYS TRAVERSED
	if ( hfInt.iterated ) { atomicCounterIncrement( totalRays ); }

	if ( !hfInt.intersected ) {
		outColor = calculateSkyColor( rayDirection );
	}
	else {
		outColor = ShadingFunc( hfInt );
	}
}