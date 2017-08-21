#include "Vector.h"

#include <cmath>

/*********
Vector2D
*********/

Vector2D::Vector2D() :
    x( 0 ), y( 0 ) {
}

Vector2D::Vector2D( float x1, float y1 ) :
    x( x1 ), y( y1 ) {
}

Vector2D::Vector2D( float fill ) :
    x( fill ), y( fill ) {
}

void Vector2D::setVector( float x1, float y1 ) {
    x = x1;
    y = y1;
}


Vector2D Vector2D::getNormalized() const {
    Vector2D vec = *this;
    vec.normalize();
    return vec;
}
Vector2D &Vector2D::normalizeThis() {
    normalize();
    return *this;
}
void Vector2D::normalize() {
    float length = getLength();
    if ( length > 0.0f ) {
        x /= length;
        y /= length;
    }
}

float Vector2D::getLength() const {
    return sqrt( x * x + y * y );
}
float Vector2D::getSquaredLength() const {
    return x * x + y * y;
}

float Vector2D::dotProduct( const Vector2D &v1, const Vector2D &v2 ) {
    return v1.x * v2.x + v1.y * v2.y;
}

float Vector2D::crossProduct( const Vector2D &v1, const Vector2D &v2 ) {
    return v1.x * v2.y - v1.y * v2.x;
}

void Vector2D::rotate90CCW() {
    float temp = x;
    x = -y;
    y = temp;
}
void Vector2D::rotate90CW() {
    float temp = x;
    x = y;
    y = -temp;
}

Vector2D &Vector2D::operator+=( const Vector2D &rhs ) {
    x += rhs.x;
    y += rhs.y;
    return *this;
}
Vector2D &Vector2D::operator-=( const Vector2D &rhs ) {
    x -= rhs.x;
    y -= rhs.y;
    return *this;
}
Vector2D &Vector2D::operator*=( const Vector2D& rhs ) {
    x *= rhs.x;
    y *= rhs.y;
    return *this;
}
Vector2D &Vector2D::operator*=( const float &rhs ) {
    x *= rhs;
    y *= rhs;
    return *this;
}
Vector2D &Vector2D::operator/=( const Vector2D& rhs ) {
    x /= rhs.x;
    y /= rhs.y;
    return *this;
}
Vector2D &Vector2D::operator/=( const float &rhs ) {
    x /= rhs;
    y /= rhs;
    return *this;
}

float &Vector2D::operator[]( const unsigned int &i ) {
    return reinterpret_cast< float* >( this )[ i ];
}

const float &Vector2D::operator[]( const unsigned int &i ) const {
    return reinterpret_cast< const float* >( this )[ i ];
}

Vector2D operator+( Vector2D lhs, const Vector2D &rhs ) {
    return lhs += rhs;
}
Vector2D operator-( Vector2D lhs, const Vector2D &rhs ) {
    return lhs -= rhs;
}
Vector2D operator*( Vector2D lhs, const Vector2D& rhs ) {
    return lhs *= rhs;
}
Vector2D operator*( Vector2D lhs, const float &rhs ) {
    return lhs *= rhs;
}
Vector2D operator*( const float &lhs, Vector2D rhs ) {
    return rhs *= lhs;
}
Vector2D operator/( Vector2D lhs, const Vector2D& rhs ) {
    return lhs /= rhs;
}
Vector2D operator/( Vector2D lhs, const float &rhs ) {
    return lhs /= rhs;
}
Vector2D operator-( Vector2D vec ) {
    vec.x = -vec.x;
    vec.y = -vec.y;
    return vec;
}

bool operator==( const Vector2D& lhs, const Vector2D& rhs ) {
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

bool operator!=( const Vector2D& lhs, const Vector2D& rhs ) {
    return !operator==( lhs, rhs );
}


/*********
Vector3D
*********/


Vector3D::Vector3D() :
    x( 0 ), y( 0 ), z( 0 ) {
}

Vector3D::Vector3D( float x1, float y1, float z1 ) :
    x( x1 ), y( y1 ), z( z1 ) {
}

Vector3D::Vector3D( const Vector2D &vec, float z1 ) :
    x( vec.x ), y( vec.y ), z( z1 ) {
}
Vector3D::Vector3D( float fill ) :
    x( fill ), y( fill ), z( fill ) {
}

void Vector3D::setVector( float x1, float y1, float z1 ) {
    x = x1;
    y = y1;
    z = z1;
}

Vector3D Vector3D::getNormalized() const {
    Vector3D vec = *this;
    vec.normalize();
    return vec;
}
Vector3D &Vector3D::normalizeThis() {
    normalize();
    return *this;
}
void Vector3D::normalize() {
    float length = getLength();
    if ( length > 0.0f ) {
        x /= length;
        y /= length;
        z /= length;
    }
}

float Vector3D::getLength() const {
    return sqrt( x * x + y * y + z * z );
}
float Vector3D::getSquaredLength() const {
    return x * x + y * y + z * z;
}

float Vector3D::dotProduct( const Vector3D &v1, const Vector3D &v2 ) {
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

Vector3D Vector3D::crossProduct( const Vector3D &v1, const Vector3D &v2 ) {
    return Vector3D( v1.y * v2.z - v2.y * v1.z,
        v1.z * v2.x - v2.z * v1.x,
        v1.x * v2.y - v2.x * v1.y );
}

Vector3D &Vector3D::operator+=( const Vector3D &rhs ) {
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
    return *this;
}
Vector3D &Vector3D::operator-=( const Vector3D &rhs ) {
    x -= rhs.x;
    y -= rhs.y;
    z -= rhs.z;
    return *this;
}
Vector3D &Vector3D::operator*=( const Vector3D &rhs ) {
    x *= rhs.x;
    y *= rhs.y;
    z *= rhs.z;
    return *this;
}
Vector3D &Vector3D::operator*=( const float &rhs ) {
    x *= rhs;
    y *= rhs;
    z *= rhs;
    return *this;
}
Vector3D &Vector3D::operator/=( const Vector3D &rhs ) {
    x /= rhs.x;
    y /= rhs.y;
    z /= rhs.z;
    return *this;
}
Vector3D &Vector3D::operator/=( const float &rhs ) {
    x /= rhs;
    y /= rhs;
    z /= rhs;
    return *this;
}

float &Vector3D::operator[]( const unsigned int &i ) {
    return reinterpret_cast< float* >( this )[ i ];
}

const float &Vector3D::operator[]( const unsigned int &i ) const {
    return reinterpret_cast< const float* >( this )[ i ];
}

Vector3D operator+( Vector3D lhs, const Vector3D &rhs ) {
    return lhs += rhs;
}
Vector3D operator-( Vector3D lhs, const Vector3D &rhs ) {
    return lhs -= rhs;
}
Vector3D operator*( Vector3D lhs, const Vector3D& rhs ) {
    return lhs *= rhs;
}
Vector3D operator*( Vector3D lhs, const float &rhs ) {
    return lhs *= rhs;
}
Vector3D operator*( const float &lhs, Vector3D rhs ) {
    return rhs *= lhs;
}
Vector3D operator/( Vector3D lhs, const Vector3D& rhs ) {
    return lhs /= rhs;
}
Vector3D operator/( Vector3D lhs, const float &rhs ) {
    return lhs /= rhs;
}
Vector3D operator-( Vector3D vec ) {
    vec.x = -vec.x;
    vec.y = -vec.y;
    vec.z = -vec.z;
    return vec;
}

bool operator==( const Vector3D& lhs, const Vector3D& rhs ) {
    return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
}

bool operator!=( const Vector3D& lhs, const Vector3D& rhs ) {
    return !operator==( lhs, rhs );
}


/*********
Vector4D
*********/


Vector4D::Vector4D() :
    x( 0 ), y( 0 ), z( 0 ) {
}

Vector4D::Vector4D( float x1, float y1, float z1, float w1 ) :
    x( x1 ), y( y1 ), z( z1 ), w( w1 ) {
}

Vector4D::Vector4D( const Vector3D &vec, float w1 ) :
    x( vec.x ), y( vec.y ), z( vec.z ), w( w1 ) {
}

Vector4D::Vector4D( float fill ) :
    x( fill ), y( fill ), z( fill ), w( fill ) {
}


void Vector4D::setVector( float x1, float y1, float z1, float w1 ) {
    x = x1;
    y = y1;
    z = z1;
    w = w1;
}

Vector4D Vector4D::getNormalized() const {
    Vector4D vec = *this;
    vec.normalize();
    return vec;
}
Vector4D &Vector4D::normalizeThis() {
    normalize();
    return *this;
}
void Vector4D::normalize() {
    float length = getLength();
    if ( length > 0.0f ) {
        x /= length;
        y /= length;
        z /= length;
        w /= length;
    }
}

float Vector4D::getLength() const {
    return sqrt( x * x + y * y + z * z + w * w );
}
float Vector4D::getSquaredLength() const {
    return x * x + y * y + z * z + w * w;
}

float Vector4D::dotProduct( const Vector4D &v1, const Vector4D &v2 ) {
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w;
}

Vector4D &Vector4D::operator+=( const Vector4D &rhs ) {
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
    w += rhs.w;
    return *this;
}
Vector4D &Vector4D::operator-=( const Vector4D &rhs ) {
    x -= rhs.x;
    y -= rhs.y;
    z -= rhs.z;
    w -= rhs.w;
    return *this;
}
Vector4D &Vector4D::operator*=( const Vector4D &rhs ) {
    x *= rhs.x;
    y *= rhs.y;
    z *= rhs.z;
    w *= rhs.w;
    return *this;
}
Vector4D &Vector4D::operator*=( const float &rhs ) {
    x *= rhs;
    y *= rhs;
    z *= rhs;
    w *= rhs;
    return *this;
}
Vector4D &Vector4D::operator/=( const Vector4D &rhs ) {
    x /= rhs.x;
    y /= rhs.y;
    z /= rhs.z;
    w /= rhs.w;
    return *this;
}
Vector4D &Vector4D::operator/=( const float &rhs ) {
    x /= rhs;
    y /= rhs;
    z /= rhs;
    w /= rhs;
    return *this;
}

float &Vector4D::operator[]( const unsigned int &i ) {
    return reinterpret_cast< float* >( this )[ i ];
}

const float &Vector4D::operator[]( const unsigned int &i ) const {
    return reinterpret_cast< const float* >( this )[ i ];
}

Vector4D operator+( Vector4D lhs, const Vector4D &rhs ) {
    return lhs += rhs;
}
Vector4D operator-( Vector4D lhs, const Vector4D &rhs ) {
    return lhs -= rhs;
}
Vector4D operator*( Vector4D lhs, const Vector4D &rhs ) {
    return lhs *= rhs;
}
Vector4D operator*( Vector4D lhs, const float &rhs ) {
    return lhs *= rhs;
}
Vector4D operator*( const float &lhs, Vector4D rhs ) {
    return rhs *= lhs;
}
Vector4D operator/( Vector4D lhs, const Vector4D &rhs ) {
    return lhs /= rhs;
}
Vector4D operator/( Vector4D lhs, const float &rhs ) {
    return lhs /= rhs;
}
Vector4D operator-( Vector4D vec ) {
    vec.x = -vec.x;
    vec.y = -vec.y;
    vec.z = -vec.z;
    vec.w = -vec.w;
    return vec;
}

bool operator==( const Vector4D& lhs, const Vector4D& rhs ) {
    return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z && lhs.w == rhs.w;
}

bool operator!=( const Vector4D& lhs, const Vector4D& rhs ) {
    return !operator==( lhs, rhs );
}