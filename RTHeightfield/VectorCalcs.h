#pragma once

#include "Vector.h"

/* These functions use a right-handed reference frame, where Y is the front/back axis
   X the right/left axis and Z the up/down axis.
   Unless specified otherwise, horizontal angles correspond to the vectors

                         0   degrees = +Y
       90  degrees = -X        -+-         270 degrees = +X
                        180  degrees = -Y
  ---------------------------------------------------------------
   A vertical angles corresponds to the vectors

              90 degrees                      Z+
                        45 degrees
                  -+-             0 degrees   Z=0
                       -45 degrees
             -90 degrees                      Z-
*/

// Takes angle in degrees and returns the direction vector: 0 deg = up/forward, 90 deg = left
Vector2D createDirectionVector( float orientation );

/* Create vector given two angles:
    - horizontalAngle - [0,360]
    - verticalAngle [-90,90]
*/
Vector3D createDirectionVectorYup( float horizontalAngle, float verticalAngle );
Vector3D createDirectionVectorZup( float horizontalAngle, float verticalAngle );

/*float horizontalAngleFromVector( Vector3D dir );
float verticalAngleFromVector( Vector3D dir );*/

Vector2D reflectVector( Vector2D direction, Vector2D normal );
Vector3D reflectVector( Vector3D direction, Vector3D normal );

bool refract( const Vector3D& normal, const Vector3D& incident, float n1, float n2, Vector3D& refractedOut );

template < class V, class F >
void applyFuncToVector( Vector3D& v, const F& f ) {
    v.x = f( v.x );
    v.y = f( v.y );
    v.z = f( v.z );
}

// Calculates the normalized direction from the center to the surface of a sphere from angles in radians
// hAngle = phi = [0,2pi] | vAngle = theta [-pi/2,pi/2]
Vector3D getSphericalDirection( float hAngle, float vAngle );
// Calculates the normalized direction from the apex through the inside of an inverted cone
// Angles in radians.
// hAngle = phi = [0, 2pi] | openAngle = theta [-pi/2,pi/2]
Vector3D getConeVerticalDirection( float hAngle, float openAngle );
// Returns a vector that lies in the cone hull
Vector3D getConeHullDirection( float hAngle, float openAngle );

// Rotate around Y axis counter clockwise
Vector3D rotate90_Yccw( Vector3D v );
// Rotate around Y axis clockwise
Vector3D rotate90_Ycw( Vector3D v );

// Rotate around X axis counter clockwise
Vector3D rotate90_Xccw( Vector3D v );
// Rotate around X axis clockwise
Vector3D rotate90_Xcw( Vector3D v );