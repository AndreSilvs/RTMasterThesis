#pragma once

#include "Vector.h"

struct Matrix2D {
    float values[ 4 ];

    Matrix2D();
    Matrix2D( float fill );
    Matrix2D( float v11, float v12,
              float v21, float v22 );
    Matrix2D( float *arr );

    void setValues( float v11, float v12,
                    float v21, float v22 );

    void setValue( float value, int row, int column );
    float getValue( int row, int column ) const;

    void setIdentity();

    float getDeterminant() const;

    void transpose();
    Matrix2D getTranspose() const;

    bool isInvertible() const;
    void invert();
    Matrix2D getInverse() const;

    Vector2D multiplyTranspose( const Vector2D &vec ) const;

    float *getMatrixAsArray();
    const float *getMatrixAsArray() const;

    float &operator[]( unsigned int i );
    const float &operator[]( unsigned int i ) const;

    Matrix2D &operator+=( const Matrix2D &rhs );
    Matrix2D &operator-=( const Matrix2D &rhs );
    Matrix2D &operator*=( const Matrix2D &rhs );
    
    Matrix2D &operator*=( const float &rhs );
    Matrix2D &operator/=( const float &rhs );
};
Matrix2D operator+( Matrix2D lhs, const Matrix2D &rhs );
Matrix2D operator-( Matrix2D lhs, const Matrix2D &rhs );
Matrix2D operator*( const Matrix2D &lhs, const Matrix2D &rhs );

Vector2D operator*( const Matrix2D &lhs, const Vector2D &rhs );

Matrix2D operator*( Matrix2D lhs, const float &rhs );
Matrix2D operator*( const float &lhs, Matrix2D rhs );
Matrix2D operator/( Matrix2D lhs, const float &rhs );

Matrix2D operator-( const Matrix2D &mat );




struct Matrix3D {
    float values[ 9 ];

    Matrix3D();
    Matrix3D( float fill );
    Matrix3D( float v11, float v12, float v13,
              float v21, float v22, float v23,
              float v31, float v32, float v33 );
    Matrix3D( float *arr );

    void setValues( float v11, float v12, float v13,
                    float v21, float v22, float v23,
                    float v31, float v32, float v33 );

    void setValue( float value, int row, int column );
    float getValue( int row, int column ) const;

    void setIdentity();

    float getDeterminant() const;

    Matrix3D getTranspose() const;
    void transpose();

    bool isInvertible() const;
    Matrix3D getInverse() const;
    void invert();

    Vector3D multiplyTranspose( const Vector3D &vec ) const;

    float *getMatrixAsArray();
    const float *getMatrixAsArray() const;

    float &operator[]( unsigned int i );
    const float &operator[]( unsigned int i ) const;

    Matrix3D &operator+=( const Matrix3D &rhs );
    Matrix3D &operator-=( const Matrix3D &rhs );
    Matrix3D &operator*=( const Matrix3D &rhs );

    Matrix3D &operator*=( const float &rhs );
    Matrix3D &operator/=( const float &rhs );
};
Matrix3D operator+( Matrix3D lhs, const Matrix3D &rhs );
Matrix3D operator-( Matrix3D lhs, const Matrix3D &rhs );
Matrix3D operator*( const Matrix3D &lhs, const Matrix3D &rhs );

Vector3D operator*( const Matrix3D &lhs, const Vector3D &rhs );

Matrix3D operator*( Matrix3D lhs, const float &rhs );
Matrix3D operator*( const float &lhs, Matrix3D rhs );
Matrix3D operator/( Matrix3D lhs, const float &rhs );

Matrix3D operator-( Matrix3D &mat );






struct Matrix4D {
    float values[ 16 ];

    Matrix4D();
    Matrix4D( float fill );
    Matrix4D( float v11, float v12, float v13, float v14,
        float v21, float v22, float v23, float v24,
        float v31, float v32, float v33, float v34,
        float v41, float v42, float v43, float v44 );
    Matrix4D( float *arr );

    void setValues( float v11, float v12, float v13, float v14,
        float v21, float v22, float v23, float v24,
        float v31, float v32, float v33, float v34,
        float v41, float v42, float v43, float v44 );

    void setValue( float value, int row, int column );
    float getValue( int row, int column ) const;

    void setIdentity();

    //float getDeterminant() const;

    Matrix4D getTranspose() const;
    void transpose();

    /*bool isInvertible() const;
    Matrix4D getInverse() const;
    void invert();*/

    Vector4D multiplyTranspose( const Vector4D &vec ) const;

    float *getMatrixAsArray();
    const float *getMatrixAsArray() const;

    float &operator[]( unsigned int i );
    const float &operator[]( unsigned int i ) const;

    Matrix4D &operator+=( const Matrix4D &rhs );
    Matrix4D &operator-=( const Matrix4D &rhs );
    Matrix4D &operator*=( const Matrix4D &rhs );

    Matrix4D &operator*=( const float &rhs );
    Matrix4D &operator/=( const float &rhs );
};
Matrix4D operator+( Matrix4D lhs, const Matrix4D &rhs );
Matrix4D operator-( Matrix4D lhs, const Matrix4D &rhs );
Matrix4D operator*( const Matrix4D &lhs, const Matrix4D &rhs );

Vector4D operator*( const Matrix4D &lhs, const Vector4D &rhs );

Matrix4D operator*( Matrix4D lhs, const float &rhs );
Matrix4D operator*( const float &lhs, Matrix4D rhs );
Matrix4D operator/( Matrix4D lhs, const float &rhs );

Matrix4D operator-( Matrix4D &mat );
