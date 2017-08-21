#pragma once

#include "Matrix.h"

Matrix3D extractMatrix( const Matrix4D &matrix );
Matrix4D extendMatrix( const Matrix3D &matrix );

//Transformaçoes
void setTranslationMatrix( Matrix4D &matrix, float x, float y, float z );
void setTranslationMatrix( Matrix4D &matrix, const Vector3D& v );

void translateMatrix( Matrix4D &matrix, float x, float y, float z );
void translateMatrix( Matrix4D &matrix, const Vector3D& v );
void translateMatrix( Matrix4D &matrix, const Vector2D& v );

void setScaleMatrix( Matrix4D &matrix, float x, float y, float z );
void setScaleMatrix( Matrix4D &matrix, const Vector3D& v );
void scaleMatrix( Matrix4D &matrix, float x, float y, float z );
void scaleMatrix( Matrix4D &matrix, const Vector3D& v );

void setRotationMatrix( Matrix4D &matrix, float angle, float vectorX, float vectorY, float vectorZ );
void setRotationMatrix( Matrix4D &matrix, float angle, const Vector3D& v );
void setRotationXMatrix( Matrix4D &matrix, float angle );
void setRotationYMatrix( Matrix4D &matrix, float angle );
void setRotationZMatrix( Matrix4D &matrix, float angle );

void rotateMatrix( Matrix4D &matrix, float angle, float vectorX, float vectorY, float vectorZ );
void rotateMatrix( Matrix4D &matrix, float angle, const Vector3D& v );
void rotateXMatrix( Matrix4D &matrix, float angle );
void rotateYMatrix( Matrix4D &matrix, float angle );
void rotateZMatrix( Matrix4D &matrix, float angle );

//Projections
void setOrthoMatrix( Matrix4D &matrix, float left, float right, float bottom, float top, float farz, float nearz );

void setPerspectiveMatrix( Matrix4D &matrix, float fov, float aspect, float nearz, float farz );

void setFrustumMatrix( Matrix4D &matrix, float left, float right, float bottom, float top, float farz, float nearz );

void setLookAtMatrix( Matrix4D &matrix, const Vector3D &eye, const Vector3D &center, const Vector3D &up );

void setLookToMatrixY( Matrix4D &matrix, const Vector3D &eye, const Vector3D &direction );
void setLookToMatrixZ( Matrix4D &matrix, const Vector3D &eye, const Vector3D &direction );

// Other
Matrix3D computeNormalMatrix( const Matrix4D &vmMatrix );

Matrix4D getViewportMatrix( int width, int height, float nearz, float farz );
Matrix4D getViewportMatrix( float left, float right, float bottom, float top, float nearz, float farz );

Matrix4D getShadowMatrix( const Matrix4D& lightProjection, const Matrix4D& lightView, const Matrix4D& model );

Vector2D convertPointWorldToScreen( const Matrix4D &pvMatrix, const Vector3D &point );
