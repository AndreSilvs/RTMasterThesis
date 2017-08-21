#include "MatrixCalcs.h"

#include <math.h>
#include <vector>
#include <sstream>
#include <iostream>

#include "MathCalcs.h"

Matrix3D extractMatrix( const Matrix4D &matrix ) {
    return Matrix3D {
        matrix[ 0 ], matrix[ 1 ], matrix[ 2 ],
        matrix[ 4 ], matrix[ 5 ], matrix[ 6 ],
        matrix[ 8 ], matrix[ 9 ], matrix[ 10] 
    };
}
Matrix4D extendMatrix( const Matrix3D &matrix ) {
    return Matrix4D {
        matrix[ 0 ], matrix[ 1 ], matrix[ 2 ], 0,
        matrix[ 3 ], matrix[ 4 ], matrix[ 5 ], 0,
        matrix[ 6 ], matrix[ 7 ], matrix[ 8 ], 0,
        0, 0, 0, 1
    };
}

//Transformacoes
void setTranslationMatrix( Matrix4D &matrix, float x, float y, float z ) {
    matrix.setValues(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        x, y, z, 1 
    );
}
void setTranslationMatrix( Matrix4D &matrix, const Vector3D& v ) {
    matrix.setValues(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        v.x, v.y, v.z, 1
    );
}

void translateMatrix( Matrix4D &matrix, float x, float y, float z ) {
    Matrix4D translationMatrix;
    setTranslationMatrix( translationMatrix, x, y, z );
    matrix = translationMatrix * matrix;
}
void translateMatrix( Matrix4D &matrix, const Vector3D& v ) {
    Matrix4D translationMatrix;
    setTranslationMatrix( translationMatrix, v );
    matrix = translationMatrix * matrix;
}
void translateMatrix( Matrix4D &matrix, const Vector2D& v ) {
    Matrix4D translationMatrix;
    setTranslationMatrix( translationMatrix, Vector3D( v, 0 ) );
    matrix = translationMatrix * matrix;
}

void setScaleMatrix( Matrix4D &matrix, float x, float y, float z ) {
    matrix.setValues(
        x, 0, 0, 0,
        0, y, 0, 0,
        0, 0, z, 0,
        0, 0, 0, 1
    );
}
void setScaleMatrix( Matrix4D &matrix, const Vector3D& v ) {
    matrix.setValues(
        v.x, 0, 0, 0,
        0, v.y, 0, 0,
        0, 0, v.z, 0,
        0, 0, 0, 1
        );
}
void scaleMatrix( Matrix4D &matrix, float x, float y, float z ) {
    Matrix4D scaleMatrix;
    setScaleMatrix( scaleMatrix, x, y, z );
    matrix = scaleMatrix * matrix;
}
void scaleMatrix( Matrix4D &matrix, const Vector3D& v ) {
    Matrix4D scaleMatrix;
    setScaleMatrix( scaleMatrix, v );
    matrix = scaleMatrix * matrix;
}

void setRotationMatrix( Matrix4D &matrix, float angle, float vectorX, float vectorY, float vectorZ ) {
	float radiansAngle = mathCalcs::degreesToRadians( angle );
	Vector3D rotationVector{ vectorX, vectorY, vectorZ };
	
    rotationVector.normalize();
	
	float cosA = cos( radiansAngle );
	float sinA = sin( radiansAngle );

    Vector3D tempVector = ( 1 - cosA ) * rotationVector;

	matrix[ 0 ] = cosA + tempVector[ 0 ] * rotationVector[ 0 ];
	matrix[ 1 ] = tempVector[ 0 ] * rotationVector[ 1 ] + sinA * rotationVector[ 2 ];
	matrix[ 2 ] = tempVector[ 0 ] * rotationVector[ 2 ] - sinA * rotationVector[ 1 ];
	matrix[ 3 ] = 0;

	matrix[ 4 ] = tempVector[ 1 ] * rotationVector[ 0 ] - sinA * rotationVector[ 2 ];
	matrix[ 5 ] = cosA + tempVector[ 1 ] * rotationVector[ 1 ];
	matrix[ 6 ] = tempVector[ 1 ] * rotationVector[ 2 ] + sinA * rotationVector[ 0 ];
	matrix[ 7 ] = 0;

	matrix[ 8 ] = tempVector[ 2 ] * rotationVector[ 0 ] + sinA * rotationVector[ 1 ]; 
	matrix[ 9 ] = tempVector[ 2 ] * rotationVector[ 1 ] - sinA * rotationVector[ 0 ];
	matrix[ 10 ] = cosA + tempVector[ 2 ] * rotationVector[ 2 ];
	matrix[ 11 ] = 0;

	matrix[ 12 ] = 0;
	matrix[ 13 ] = 0;
	matrix[ 14 ] = 0;
	matrix[ 15 ] = 1;
}
void setRotationMatrix( Matrix4D &matrix, float angle, const Vector3D& v ) {
    setRotationMatrix( matrix, angle, v.x, v.y, v.z );
}

void setRotationXMatrix( Matrix4D &matrix, float angle ) {
	float angleRadians = mathCalcs::degreesToRadians( angle );
	float cosA = cos( angleRadians );
	float sinA = sin( angleRadians );

    matrix.setIdentity();

	matrix[ 5 ] = cosA;
	matrix[ 6 ] = sinA;
	matrix[ 9 ] = -sinA;
	matrix[ 10] = cosA;
}

void setRotationYMatrix( Matrix4D &matrix, float angle ) {
	float angleRadians = mathCalcs::degreesToRadians( angle );
	float cosA = cos( angleRadians );
	float sinA = sin( angleRadians );

    matrix.setIdentity();

	matrix[ 0 ] = cosA;
	matrix[ 2 ] = -sinA;
	matrix[ 8 ] = sinA;
	matrix[ 10] = cosA;
}

void setRotationZMatrix( Matrix4D &matrix, float angle ) {
	float angleRadians = mathCalcs::degreesToRadians( angle );
	float cosA = cos( angleRadians );
	float sinA = sin( angleRadians );

    matrix.setIdentity();

	matrix[ 0 ] = cosA;
	matrix[ 1 ] = sinA;
	matrix[ 4 ] = -sinA;
	matrix[ 5 ] = cosA;
}

void rotateMatrix( Matrix4D &matrix, float angle, float vectorX, float vectorY, float vectorZ ) {
    Matrix4D rotationMatrix;
    setRotationMatrix( rotationMatrix, angle, vectorX, vectorY, vectorZ );
    matrix = rotationMatrix * matrix;
}
void rotateMatrix( Matrix4D &matrix, float angle, const Vector3D& v ) {
    Matrix4D rotationMatrix;
    setRotationMatrix( rotationMatrix, angle, v );
    matrix = rotationMatrix * matrix;
}

void rotateXMatrix( Matrix4D &matrix, float angle ) {
    Matrix4D rotationMatrix;
    setRotationXMatrix( rotationMatrix, angle );
    matrix = rotationMatrix * matrix;
}
void rotateYMatrix( Matrix4D &matrix, float angle ) {
    Matrix4D rotationMatrix;
    setRotationYMatrix( rotationMatrix, angle );
    matrix = rotationMatrix * matrix;
}
void rotateZMatrix( Matrix4D &matrix, float angle ) {
    Matrix4D rotationMatrix;
    setRotationZMatrix( rotationMatrix, angle );
    matrix = rotationMatrix * matrix;
}

//Projeccoes
void setOrthoMatrix( Matrix4D &matrix, float left, float right, float bottom, float top, float farz, float nearz ) {

    matrix.setIdentity();

	matrix[0] = 2/(right-left);
	matrix[5] = 2/(top-bottom);
	matrix[10] = -2/(farz-nearz);
	matrix[12] = -(right+left)/(right-left);
	matrix[13] = -(top+bottom)/(top-bottom);
	matrix[14] = -(farz+nearz)/(farz-nearz);
}

void setPerspectiveMatrix( Matrix4D &matrix, float fov, float aspect, float nearz, float farz ) {
    matrix.setIdentity();

	float f = 1.0f / tan( fov * (float)(CALC_PI / 360.0) );
 
    matrix[0] = f / aspect;
    matrix[5] = f;
    matrix[10] = (farz + nearz) / (nearz - farz);
    matrix[14] = (2.0f * farz * nearz) / (nearz - farz);
    matrix[11] = -1.0f;
    matrix[15] = 0.0f;
}

//frustum?
void setFrustumMatrix( Matrix4D &matrix, float left, float right, float bottom, float top, float farz, float nearz ) {

	/*float bottom, top, left, right;

	top=nearr*tan((PI/180)*fov/2);
	bottom = -top;
	right = top*aspect;
	left=-right;*/

	/*setIdentityMatrix( matrix );

	matrix[0] = 2 * nearz / ( right - left );
	matrix[5] = 2 * nearz / ( top - bottom );
	matrix[8] = ( right + left ) / ( right - left );
	matrix[9] = ( top + bottom ) / ( top - bottom );
	matrix[10] = -( farz + nearz ) / ( farz - nearz );
	matrix[11] = -1;
	matrix[14] = -2 * farz * nearz / ( farz - nearz );*/
}

void setLookAtMatrix( Matrix4D &matrix, const Vector3D &eye, const Vector3D &center, const Vector3D &up ) {
	Vector3D n, v, u, upV;

    n = center - eye;
    n.normalize();

    upV = up;

    u = Vector3D::crossProduct( n, up );
    u.normalize();

    v = Vector3D::crossProduct( u, n );
    v.normalize();

	matrix[0]  = u[0];
    matrix[4]  = u[1];
    matrix[8]  = u[2];
    matrix[12] = 0.0f;
 
    matrix[1]  = v[0];
    matrix[5]  = v[1];
    matrix[9]  = v[2];
    matrix[13] = 0.0f;
 
    matrix[2]  = -n[0];
    matrix[6]  = -n[1];
    matrix[10] = -n[2];
    matrix[14] =  0.0f;
 
    matrix[3]  = 0.0f;
    matrix[7]  = 0.0f;
    matrix[11] = 0.0f;
    matrix[15] = 1.0f;

	translateMatrix( matrix, -eye.x, -eye.y, -eye.z );
}

void setLookToMatrixY( Matrix4D &matrix, const Vector3D &eye, const Vector3D &direction ) {
	Vector3D n, v, u, up;

    n = direction;
    n.normalize();

    up.setVector( 0, 1, 0 );

    u = Vector3D::crossProduct( n, up );
    u.normalize();

    v = Vector3D::crossProduct( u, n );
    v.normalize();

	matrix[0]  = u[0];
    matrix[4]  = u[1];
    matrix[8]  = u[2];
    matrix[12] = 0.0f;
 
    matrix[1]  = v[0];
    matrix[5]  = v[1];
    matrix[9]  = v[2];
    matrix[13] = 0.0f;
 
    matrix[2]  = -n[0];
    matrix[6]  = -n[1];
    matrix[10] = -n[2];
    matrix[14] =  0.0f;
 
    matrix[3]  = 0.0f;
    matrix[7]  = 0.0f;
    matrix[11] = 0.0f;
    matrix[15] = 1.0f;

    translateMatrix( matrix, -eye.x, -eye.y, -eye.z );
}
void setLookToMatrixZ( Matrix4D &matrix, const Vector3D &eye, const Vector3D &direction ) {
    Vector3D n, v, u, up;

    n = direction;
    n.normalize();

    up.setVector( 0, 0, 1 );

    u = Vector3D::crossProduct( n, up );
    u.normalize();

    v = Vector3D::crossProduct( u, n );
    v.normalize();

    matrix[ 0 ] = u[ 0 ];
    matrix[ 4 ] = u[ 1 ];
    matrix[ 8 ] = u[ 2 ];
    matrix[ 12 ] = 0.0f;

    matrix[ 1 ] = v[ 0 ];
    matrix[ 5 ] = v[ 1 ];
    matrix[ 9 ] = v[ 2 ];
    matrix[ 13 ] = 0.0f;

    matrix[ 2 ] = -n[ 0 ];
    matrix[ 6 ] = -n[ 1 ];
    matrix[ 10 ] = -n[ 2 ];
    matrix[ 14 ] = 0.0f;

    matrix[ 3 ] = 0.0f;
    matrix[ 7 ] = 0.0f;
    matrix[ 11 ] = 0.0f;
    matrix[ 15 ] = 1.0f;

    translateMatrix( matrix, -eye.x, -eye.y, -eye.z );
}

Matrix3D computeNormalMatrix( const Matrix4D &mvMatrix ) {
    //return extractMatrix( mvMatrix ).getInverse();

    Matrix3D tempMatrix = extractMatrix( mvMatrix );

    float det, invDet;

    det = tempMatrix[ 0 ] * ( tempMatrix[ 4 ] * tempMatrix[ 8 ] - tempMatrix[ 5 ] * tempMatrix[ 7 ] ) +
        tempMatrix[ 1 ] * ( tempMatrix[ 5 ] * tempMatrix[ 6 ] - tempMatrix[ 8 ] * tempMatrix[ 3 ] ) +
        tempMatrix[ 2 ] * ( tempMatrix[ 3 ] * tempMatrix[ 7 ] - tempMatrix[ 4 ] * tempMatrix[ 6 ] );

    invDet = 1.0f / det;

    Matrix3D normalMatrix;
    normalMatrix[ 0 ] = ( tempMatrix[ 4 ] * tempMatrix[ 8 ] - tempMatrix[ 5 ] * tempMatrix[ 7 ] ) * invDet;
    normalMatrix[ 1 ] = ( tempMatrix[ 5 ] * tempMatrix[ 6 ] - tempMatrix[ 8 ] * tempMatrix[ 3 ] ) * invDet;
    normalMatrix[ 2 ] = ( tempMatrix[ 3 ] * tempMatrix[ 7 ] - tempMatrix[ 4 ] * tempMatrix[ 6 ] ) * invDet;
    normalMatrix[ 3 ] = ( tempMatrix[ 2 ] * tempMatrix[ 7 ] - tempMatrix[ 1 ] * tempMatrix[ 8 ] ) * invDet;
    normalMatrix[ 4 ] = ( tempMatrix[ 0 ] * tempMatrix[ 8 ] - tempMatrix[ 2 ] * tempMatrix[ 6 ] ) * invDet;
    normalMatrix[ 5 ] = ( tempMatrix[ 1 ] * tempMatrix[ 6 ] - tempMatrix[ 7 ] * tempMatrix[ 0 ] ) * invDet;
    normalMatrix[ 6 ] = ( tempMatrix[ 1 ] * tempMatrix[ 5 ] - tempMatrix[ 4 ] * tempMatrix[ 2 ] ) * invDet;
    normalMatrix[ 7 ] = ( tempMatrix[ 2 ] * tempMatrix[ 3 ] - tempMatrix[ 0 ] * tempMatrix[ 5 ] ) * invDet;
    normalMatrix[ 8 ] = ( tempMatrix[ 0 ] * tempMatrix[ 4 ] - tempMatrix[ 3 ] * tempMatrix[ 1 ] ) * invDet;
    
    return normalMatrix;
}

Matrix4D getViewportMatrix( int width, int height, float nearz, float farz ) {
    return Matrix4D{
        ( width ) / 2.0f, 0, 0, ( width ) / 2.0f,
        0, height / 2.0f, 0, height / 2.0f,
        0, 0, ( farz - nearz ) / 2.0f, ( farz + nearz ) / 2.0f,
        0, 0, 0, 1
    };
}
Matrix4D getViewportMatrix( float left, float right, float bottom, float top, float nearz, float farz ) {
    return Matrix4D{
        ( right - left ) / 2.0f, 0, 0, ( right + left ) / 2.0f,
        0, ( top - bottom ) / 2.0f, 0, ( top + bottom ) / 2.0f,
        0, 0, ( farz - nearz ) / 2.0f, ( farz + nearz ) / 2.0f,
        0, 0, 0, 1
    };
}

Matrix4D getShadowMatrix( const Matrix4D& lightProjection, const Matrix4D& lightView, const Matrix4D& model ) {
    Matrix4D bias;
    setTranslationMatrix( bias, 0.5f, 0.5f, 0.5f );
    scaleMatrix( bias, 0.5f, 0.5f, 0.5f );

    return model * lightView * lightProjection * bias;
}

Vector2D convertPointWorldToScreen( const Matrix4D &pvMatrix, const Vector3D &point ) {
	Vector4D pointH{ point, 1.0f };

    Vector4D result = pvMatrix.multiplyTranspose( pointH );

    result /= result.w;

    return Vector2D{ result.x, result.y };
}