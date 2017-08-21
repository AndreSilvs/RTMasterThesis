#include "Matrix.h"

#include <utility>

/*********
Matrix2D
*********/

Matrix2D::Matrix2D() {
}

Matrix2D::Matrix2D( float fill ) {
    for ( int i = 0; i < 4; ++i ) {
        values[ i ] = fill;
    }
}

Matrix2D::Matrix2D( float v11, float v12, float v21, float v22 ) {
    values[ 0 ] = v11;
    values[ 1 ] = v12;
    values[ 2 ] = v21;
    values[ 3 ] = v22;
}

Matrix2D::Matrix2D( float * arr ) {
    for ( int i = 0; i < 4; ++i ) {
        values[ i ] = arr[ i ];
    }
}

void Matrix2D::setValues( float v11, float v12, float v21, float v22 ) {
    values[ 0 ] = v11;
    values[ 1 ] = v12;
    values[ 2 ] = v21;
    values[ 3 ] = v22;
}

void Matrix2D::setValue( float value, int row, int column ) {
    values[ row * 2 + column ] = value;
}

float Matrix2D::getValue( int row, int column ) const {
    return values[ row * 2 + column ];
}

void Matrix2D::setIdentity() {
    setValues(
        1, 0,
        0, 1 );
}

float Matrix2D::getDeterminant() const {
    return values[ 0 ] * values[ 3 ] - values[ 1 ] * values[ 2 ];
}

Matrix2D Matrix2D::getTranspose() const {
    Matrix2D mat( *this );
    mat.transpose();
    return mat;
}

void Matrix2D::transpose() {
    std::swap( values[ 1 ], values[ 2 ] );
}

bool Matrix2D::isInvertible() const {
    return getDeterminant() != 0.0f;
}

void Matrix2D::invert() {
    if ( isInvertible() ) {
        std::swap( values[ 0 ], values[ 3 ] );
        values[ 1 ] = -values[ 1 ];
        values[ 2 ] = -values[ 2 ];

        operator*=( getDeterminant() );
    }
}

Matrix2D Matrix2D::getInverse() const {
    Matrix2D matrix( *this );
    matrix.invert();
    return matrix;
}

Vector2D Matrix2D::multiplyTranspose( const Vector2D &vec ) const {
    Vector2D newVector;

    for ( int i = 0; i < 2; ++i ) {
        float value = 0.0f;
        for ( int k = 0; k < 2; ++k ) {
            value += getValue( k, i ) * vec[ k ];
        }
        newVector[ i ] = value;
    }

    return newVector;
}

float *Matrix2D::getMatrixAsArray() {
    return values;
}
const float *Matrix2D::getMatrixAsArray() const {
    return values;
}

float &Matrix2D::operator[]( unsigned int i ) {
    return values[ i ];
}

const float &Matrix2D::operator[]( unsigned int i ) const {
    return values[ i ];
}

Matrix2D &Matrix2D::operator+=( const Matrix2D &rhs ) {
    for ( int i = 0; i < 4; ++i ) {
        values[ i ] += rhs.values[ i ];
    }
    return *this;
}

Matrix2D &Matrix2D::operator-=( const Matrix2D &rhs ) {
    for ( int i = 0; i < 4; ++i ) {
        values[ i ] -= rhs.values[ i ];
    }
    return *this;
}

Matrix2D &Matrix2D::operator*=( const Matrix2D &rhs ) {
    Matrix2D newMatrix;

    for ( int i = 0; i < 2; ++i ) {
        for ( int j = 0; j < 2; ++j ) {
            float value = 0.0f;
            for ( int k = 0; k < 2; ++k ) {
                value += getValue( i, k ) * rhs.getValue( k, j );
            }
            newMatrix.setValue( value, i, j );
        }
    }

    operator=( newMatrix );

    return *this;
}

Matrix2D &Matrix2D::operator*=( const float &rhs ) {
    for ( int i = 0; i < 4; ++i ) {
        values[ i ] *= rhs;
    }
    return *this;
}

Matrix2D &Matrix2D::operator/=( const float &rhs ) {
    for ( int i = 0; i < 4; ++i ) {
        values[ i ] /= rhs;
    }
    return *this;
}


Matrix2D operator+( Matrix2D lhs, const Matrix2D &rhs ) {
    return lhs += rhs;
}

Matrix2D operator-( Matrix2D lhs, const Matrix2D &rhs ) {
    return lhs -= rhs;
}

Matrix2D operator*( const Matrix2D &lhs, const Matrix2D &rhs ) {
    Matrix2D newMatrix;

    for ( int i = 0; i < 2; ++i ) {
        for ( int j = 0; j < 2; ++j ) {
            float value = 0.0f;
            for ( int k = 0; k < 2; ++k ) {
                value += lhs.getValue( i, k ) * rhs.getValue( k, j );
            }
            newMatrix.setValue( value, i, j );
        }
    }

    return newMatrix;
}

Vector2D operator*( const Matrix2D &lhs, const Vector2D &rhs ) {
    Vector2D newVector;

    for ( int i = 0; i < 2; ++i ) {
        float value = 0.0f;
        for ( int k = 0; k < 2; ++k ) {
            value += lhs.getValue( i, k ) * rhs[ k ];
        }
        newVector[ i ] = value;
    }

    return newVector;
}

Matrix2D operator*( Matrix2D lhs, const float &rhs ) {
    return lhs *= rhs;
}

Matrix2D operator*( const float &lhs, Matrix2D rhs ) {
    return rhs *= lhs;
}

Matrix2D operator/( Matrix2D lhs, const float &rhs ) {
    return lhs /= rhs;
}

Matrix2D operator-( const Matrix2D &mat ) {
    return Matrix2D{ -mat[ 0 ], -mat[ 1 ],
                     -mat[ 2 ], -mat[ 3 ] };
}



/*********
 Matrix3D
*********/


Matrix3D::Matrix3D() {
}

Matrix3D::Matrix3D( float fill ) {
    for ( int i = 0; i < 4; ++i ) {
        values[ i ] = fill;
    }
}

Matrix3D::Matrix3D( float v11, float v12, float v13,
                    float v21, float v22, float v23,
                    float v31, float v32, float v33 ) {
    values[ 0 ] = v11;
    values[ 1 ] = v12;
    values[ 2 ] = v13;
    values[ 3 ] = v21;
    values[ 4 ] = v22;
    values[ 5 ] = v23;
    values[ 6 ] = v31;
    values[ 7 ] = v32;
    values[ 8 ] = v33;
}

Matrix3D::Matrix3D( float * arr ) {
    for ( int i = 0; i < 9; ++i ) {
        values[ i ] = arr[ i ];
    }
}

void Matrix3D::setValues( float v11, float v12, float v13,
                          float v21, float v22, float v23,
                          float v31, float v32, float v33 ) {
    values[ 0 ] = v11;
    values[ 1 ] = v12;
    values[ 2 ] = v13;
    values[ 3 ] = v21;
    values[ 4 ] = v22;
    values[ 5 ] = v23;
    values[ 6 ] = v31;
    values[ 7 ] = v32;
    values[ 8 ] = v33;
}

void Matrix3D::setValue( float value, int row, int column ) {
    values[ row * 3 + column ] = value;
}

float Matrix3D::getValue( int row, int column ) const {
    return values[ row * 3 + column ];
}

void Matrix3D::setIdentity() {
    setValues(
        1, 0, 0,
        0, 1, 0,
        0, 0, 1 );
}

float Matrix3D::getDeterminant() const {
    return values[ 0 ] * ( values[ 4 ] * values[ 8 ] - values[ 5 ] * values[ 7 ] ) +
           values[ 1 ] * ( values[ 5 ] * values[ 6 ] - values[ 8 ] * values[ 3 ] ) +
           values[ 2 ] * ( values[ 3 ] * values[ 7 ] - values[ 4 ] * values[ 6 ] );
}

Matrix3D Matrix3D::getTranspose() const {
    Matrix3D mat( *this );
    mat.transpose();
    return mat;
}

void Matrix3D::transpose() {
    std::swap( values[ 1 ], values[ 3 ] );
    std::swap( values[ 2 ], values[ 6 ] );
    std::swap( values[ 5 ], values[ 7 ] );
}

bool Matrix3D::isInvertible() const {
    return getDeterminant() != 0.0f;
}

void Matrix3D::invert() {
    if ( isInvertible() ) {
        operator=( getInverse() );
    }
}

Matrix3D Matrix3D::getInverse() const {
    Matrix3D matrix( *this );

    float invDet = 1.0f / getDeterminant();
    matrix[ 0 ] = ( values[ 4 ] * values[ 8 ] - values[ 5 ] * values[ 7 ] ) * invDet;
    matrix[ 1 ] = ( values[ 5 ] * values[ 6 ] - values[ 8 ] * values[ 3 ] ) * invDet;
    matrix[ 2 ] = ( values[ 3 ] * values[ 7 ] - values[ 4 ] * values[ 6 ] ) * invDet;
    matrix[ 3 ] = ( values[ 2 ] * values[ 7 ] - values[ 1 ] * values[ 8 ] ) * invDet;
    matrix[ 4 ] = ( values[ 0 ] * values[ 8 ] - values[ 2 ] * values[ 6 ] ) * invDet;
    matrix[ 5 ] = ( values[ 1 ] * values[ 6 ] - values[ 7 ] * values[ 0 ] ) * invDet;
    matrix[ 6 ] = ( values[ 1 ] * values[ 5 ] - values[ 4 ] * values[ 2 ] ) * invDet;
    matrix[ 7 ] = ( values[ 2 ] * values[ 3 ] - values[ 0 ] * values[ 5 ] ) * invDet;
    matrix[ 8 ] = ( values[ 0 ] * values[ 4 ] - values[ 3 ] * values[ 1 ] ) * invDet;

    return matrix;
}

Vector3D Matrix3D::multiplyTranspose( const Vector3D &vec ) const {
    Vector3D newVector;

    for ( int i = 0; i < 3; ++i ) {
        float value = 0.0f;
        for ( int k = 0; k < 3; ++k ) {
            value += getValue( k, i ) * vec[ k ];
        }
        newVector[ i ] = value;
    }

    return newVector;
}

float *Matrix3D::getMatrixAsArray() {
    return values;
}
const float *Matrix3D::getMatrixAsArray() const {
    return values;
}

float &Matrix3D::operator[]( unsigned int i ) {
    return values[ i ];
}

const float &Matrix3D::operator[]( unsigned int i ) const {
    return values[ i ];
}

Matrix3D &Matrix3D::operator+=( const Matrix3D &rhs ) {
    for ( int i = 0; i < 9; ++i ) {
        values[ i ] += rhs.values[ i ];
    }
    return *this;
}

Matrix3D &Matrix3D::operator-=( const Matrix3D &rhs ) {
    for ( int i = 0; i < 9; ++i ) {
        values[ i ] -= rhs.values[ i ];
    }
    return *this;
}

Matrix3D &Matrix3D::operator*=( const Matrix3D &rhs ) {
    Matrix3D newMatrix;

    for ( int i = 0; i < 3; ++i ) {
        for ( int j = 0; j < 3; ++j ) {
            float value = 0.0f;
            for ( int k = 0; k < 3; ++k ) {
                value += getValue( i, k ) * rhs.getValue( k, j );
            }
            newMatrix.setValue( value, i, j );
        }
    }

    operator=( newMatrix );

    return *this;
}

Matrix3D &Matrix3D::operator*=( const float &rhs ) {
    for ( int i = 0; i < 9; ++i ) {
        values[ i ] *= rhs;
    }
    return *this;
}

Matrix3D &Matrix3D::operator/=( const float &rhs ) {
    for ( int i = 0; i < 9; ++i ) {
        values[ i ] /= rhs;
    }
    return *this;
}


Matrix3D operator+( Matrix3D lhs, const Matrix3D &rhs ) {
    return lhs += rhs;
}

Matrix3D operator-( Matrix3D lhs, const Matrix3D &rhs ) {
    return lhs -= rhs;
}

Matrix3D operator*( const Matrix3D &lhs, const Matrix3D &rhs ) {
    Matrix3D newMatrix;

    for ( int i = 0; i < 3; ++i ) {
        for ( int j = 0; j < 3; ++j ) {
            float value = 0.0f;
            for ( int k = 0; k < 3; ++k ) {
                value += lhs.getValue( i, k ) * rhs.getValue( k, j );
            }
            newMatrix.setValue( value, i, j );
        }
    }

    return newMatrix;
}

Vector3D operator*( const Matrix3D &lhs, const Vector3D &rhs ) {
    Vector3D newVector;

    for ( int i = 0; i < 3; ++i ) {
        float value = 0.0f;
        for ( int k = 0; k < 3; ++k ) {
            value += lhs.getValue( i, k ) * rhs[ k ];
        }
        newVector[ i ] = value;
    }

    return newVector;
}

Matrix3D operator*( Matrix3D lhs, const float &rhs ) {
    return lhs *= rhs;
}

Matrix3D operator*( const float &lhs, Matrix3D rhs ) {
    return rhs *= lhs;
}

Matrix3D operator/( Matrix3D lhs, const float &rhs ) {
    return lhs /= rhs;
}

Matrix3D operator-( const Matrix3D &mat ) {
    return Matrix3D{ -mat[ 0 ], -mat[ 1 ], -mat[ 2 ],
                     -mat[ 3 ], -mat[ 4 ], -mat[ 5 ],
                     -mat[ 6 ], -mat[ 7 ], -mat[ 8 ] };
}



/*********
Matrix4D
*********/



Matrix4D::Matrix4D() {
}

Matrix4D::Matrix4D( float fill ) {
    for ( int i = 0; i < 4; ++i ) {
        values[ i ] = fill;
    }
}

Matrix4D::Matrix4D( float v11, float v12, float v13, float v14,
                    float v21, float v22, float v23, float v24,
                    float v31, float v32, float v33, float v34,
                    float v41, float v42, float v43, float v44 ) {
    values[ 0 ] = v11;
    values[ 1 ] = v12;
    values[ 2 ] = v13;
    values[ 3 ] = v14;
    values[ 4 ] = v21;
    values[ 5 ] = v22;
    values[ 6 ] = v23;
    values[ 7 ] = v24;
    values[ 8 ] = v31;
    values[ 9 ] = v32;
    values[ 10 ] = v33;
    values[ 11 ] = v34;
    values[ 12 ] = v41;
    values[ 13 ] = v42;
    values[ 14 ] = v43;
    values[ 15 ] = v44;
}

Matrix4D::Matrix4D( float * arr ) {
    for ( int i = 0; i < 16; ++i ) {
        values[ i ] = arr[ i ];
    }
}

void Matrix4D::setValues( float v11, float v12, float v13, float v14,
    float v21, float v22, float v23, float v24,
    float v31, float v32, float v33, float v34,
    float v41, float v42, float v43, float v44 ) {

    values[ 0 ] = v11;
    values[ 1 ] = v12;
    values[ 2 ] = v13;
    values[ 3 ] = v14;
    values[ 4 ] = v21;
    values[ 5 ] = v22;
    values[ 6 ] = v23;
    values[ 7 ] = v24;
    values[ 8 ] = v31;
    values[ 9 ] = v32;
    values[ 10 ] = v33;
    values[ 11 ] = v34;
    values[ 12 ] = v41;
    values[ 13 ] = v42;
    values[ 14 ] = v43;
    values[ 15 ] = v44;
}

void Matrix4D::setValue( float value, int row, int column ) {
    values[ row * 4 + column ] = value;
}

float Matrix4D::getValue( int row, int column ) const {
    return values[ row * 4 + column ];
}

void Matrix4D::setIdentity() {
    setValues(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1 );
}

Matrix4D Matrix4D::getTranspose() const {
    Matrix4D mat( *this );
    mat.transpose();
    return mat;
}

/*
 0  1  2  3
 4  5  6  7
 8  9 10 11
12 13 14 15

*/

void Matrix4D::transpose() {
    std::swap( values[ 1 ], values[ 4 ] );
    std::swap( values[ 2 ], values[ 8 ] );
    std::swap( values[ 3 ], values[ 12] );
    std::swap( values[ 6 ], values[ 9 ] );
    std::swap( values[ 7 ], values[ 13] );
    std::swap( values[ 11], values[ 14] );
}

Vector4D Matrix4D::multiplyTranspose( const Vector4D &vec ) const {
    Vector4D newVector;

    for ( int i = 0; i < 4; ++i ) {
        float value = 0.0f;
        for ( int k = 0; k < 4; ++k ) {
            value += getValue( k, i ) * vec[ k ];
        }
        newVector[ i ] = value;
    }

    return newVector;
}

float *Matrix4D::getMatrixAsArray() {
    return values;
}

const float *Matrix4D::getMatrixAsArray() const {
    return values;
}

float &Matrix4D::operator[]( unsigned int i ) {
    return values[ i ];
}

const float &Matrix4D::operator[]( unsigned int i ) const {
    return values[ i ];
}

Matrix4D &Matrix4D::operator+=( const Matrix4D &rhs ) {
    for ( int i = 0; i < 16; ++i ) {
        values[ i ] += rhs.values[ i ];
    }
    return *this;
}

Matrix4D &Matrix4D::operator-=( const Matrix4D &rhs ) {
    for ( int i = 0; i < 16; ++i ) {
        values[ i ] -= rhs.values[ i ];
    }
    return *this;
}

Matrix4D &Matrix4D::operator*=( const Matrix4D &rhs ) {
    Matrix4D newMatrix;

    for ( int i = 0; i < 4; ++i ) {
        for ( int j = 0; j < 4; ++j ) {
            float value = 0.0f;
            for ( int k = 0; k < 4; ++k ) {
                value += getValue( i, k ) * rhs.getValue( k, j );
            }
            newMatrix.setValue( value, i, j );
        }
    }

    operator=( newMatrix );

    return *this;
}

Matrix4D &Matrix4D::operator*=( const float &rhs ) {
    for ( int i = 0; i < 16; ++i ) {
        values[ i ] *= rhs;
    }
    return *this;
}

Matrix4D &Matrix4D::operator/=( const float &rhs ) {
    for ( int i = 0; i < 16; ++i ) {
        values[ i ] /= rhs;
    }
    return *this;
}


Matrix4D operator+( Matrix4D lhs, const Matrix4D &rhs ) {
    return lhs += rhs;
}

Matrix4D operator-( Matrix4D lhs, const Matrix4D &rhs ) {
    return lhs -= rhs;
}

Matrix4D operator*( const Matrix4D &lhs, const Matrix4D &rhs ) {
    Matrix4D newMatrix;

    for ( int i = 0; i < 4; ++i ) {
        for ( int j = 0; j < 4; ++j ) {
            float value = 0.0f;
            for ( int k = 0; k < 4; ++k ) {
                value += lhs.getValue( i, k ) * rhs.getValue( k, j );
            }
            newMatrix.setValue( value, i, j );
        }
    }

    return newMatrix;
}

Vector4D operator*( const Matrix4D &lhs, const Vector4D &rhs ) {
    Vector4D newVector;

    for ( int i = 0; i < 4; ++i ) {
        float value = 0.0f;
        for ( int k = 0; k < 4; ++k ) {
            value += lhs.getValue( i, k ) * rhs[ k ];
        }
        newVector[ i ] = value;
    }

    return newVector;
}

Matrix4D operator*( Matrix4D lhs, const float &rhs ) {
    return lhs *= rhs;
}

Matrix4D operator*( const float &lhs, Matrix4D rhs ) {
    return rhs *= lhs;
}

Matrix4D operator/( Matrix4D lhs, const float &rhs ) {
    return lhs /= rhs;
}

Matrix4D operator-( const Matrix4D &mat ) {
    return Matrix4D{ -mat[ 0 ], -mat[ 1 ], -mat[ 2 ], -mat[ 3 ],
                     -mat[ 4 ], -mat[ 5 ], -mat[ 6 ], -mat[ 7 ],
                     -mat[ 8 ], -mat[ 9 ], -mat[ 10], -mat[ 11],
                     -mat[ 12], -mat[ 13], -mat[ 14], -mat[ 15] };
}
