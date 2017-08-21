#include "VectorCalcs.h"

#include <cmath>

#include "MathCalcs.h"

Vector2D createDirectionVector( float orientation ) {
    orientation = mathCalcs::degreesToRadians( orientation );
    return Vector2D( -sin( orientation ), cos( orientation ) );
}

Vector3D createDirectionVectorYup( float horizontalAngle, float verticalAngle ) {
    float sinV = sin( mathCalcs::degreesToRadians( verticalAngle ) );
    float cosV = cos( mathCalcs::degreesToRadians( verticalAngle ) );
    float sinH = sin( mathCalcs::degreesToRadians( horizontalAngle ) );
    float cosH = cos( mathCalcs::degreesToRadians( horizontalAngle ) );

    return Vector3D( cosV * sinH, sinV, cosV * cosH );
}

Vector3D createDirectionVectorZup( float horizontalAngle, float verticalAngle ) {
    float sinV = sin( mathCalcs::degreesToRadians( verticalAngle ) );
    float cosV = cos( mathCalcs::degreesToRadians( verticalAngle ) );
    float sinH = sin( mathCalcs::degreesToRadians( -horizontalAngle ) );
    float cosH = cos( mathCalcs::degreesToRadians( -horizontalAngle ) );

    return Vector3D( cosV * sinH, cosV * cosH, sinV );
}

Vector2D reflectVector( Vector2D direction, Vector2D normal ) {
    // reflected = Incident - 2*(Normal.Incident)*Normal;
    return direction - 2 * Vector2D::dotProduct( normal, direction ) * normal;
}

Vector3D reflectVector( Vector3D direction, Vector3D normal ) {
    // reflected = Incident - 2*(Normal.Incident)*Normal;
    return direction - 2 * Vector3D::dotProduct( normal, direction ) * normal;
}

bool refract( const Vector3D& normal, const Vector3D& incident, float n1, float n2, Vector3D& refractedOut ) {
    float n1n2 = n1 / n2;

    float dot = Vector3D::dotProduct( incident, normal );

    float sin2 = n1n2 * n1n2 * ( 1 - dot * dot );

    if ( sin2 <= 1 ) {

        refractedOut = ( incident * n1n2 );

        float aux = n1n2 * dot + sqrt( 1 - sin2 );

        Vector3D auxV = aux * normal;

        refractedOut = refractedOut - auxV;
        refractedOut.normalize();

        return true;
    }

    return false;
    /*float n1n2 = n1 / n2;
    float dotIN = Vector3D::dotProduct( incident, normal );

    float sin2 = n1n2 * n1n2 * ( 1 - dotIN*dotIN );

    if ( sin2 <= 1 ) {
    refractedOut = n1n2 * incident;

    float aux = n1n2 * dotIN + sqrt( 1 - sin2 );
    Vector3D auxVec = aux * normal;
    // n1n2 * incident - ( n1n2 * dot IN + sqrt( 1 - sin2 ) ) * normal
    ( refractedOut -= auxVec ).normalize();
    return true;
    }

    return false;*/
}


Vector3D getSphericalDirection( float hAngle, float vAngle ) {
    Vector3D v;
    v.x = sin( hAngle ) * cos( vAngle );
    v.y = sin( hAngle ) * sin( vAngle );
    v.z = cos( hAngle );
    return v;
}

Vector3D getConeVerticalDirection( float hAngle, float openAngle ) {
    Vector3D v;
    v.x = sin( hAngle ) * sin( openAngle );
    v.y = cos( hAngle ) * sin( openAngle );
    v.z = cos( openAngle );
    return v;
}
Vector3D getConeHullDirection( float hAngle, float openAngle ) {
    Vector3D v;
    v.x = sin( hAngle ) * sin( openAngle / 2.0f );
    v.y = cos( hAngle ) * sin( openAngle / 2.0f );
    v.z = cos( openAngle / 2.0f );
    return v;
}

Vector3D rotate90_Yccw( Vector3D v ) {
    return Vector3D( -v.z, v.y, v.x );
}
Vector3D rotate90_Ycw( Vector3D v ) {
    return Vector3D( v.z, v.y, -v.x );
}

Vector3D rotate90_Xccw( Vector3D v ) {
    return Vector3D( v.x, -v.z, v.y );
}
Vector3D rotate90_Xcw( Vector3D v ) {
    return Vector3D( v.x, v.z, -v.y );
}