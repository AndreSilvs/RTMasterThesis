#pragma once

struct Vector2D {
    float x;
    float y;

    Vector2D();
    Vector2D( float x, float y );
    Vector2D( float fill );

    void setVector( float x, float y );

    Vector2D getNormalized() const;
    Vector2D &normalizeThis();
    void normalize();

    float getLength() const;
    float getSquaredLength() const;

    static float dotProduct( const Vector2D &v1, const Vector2D &v2 );
	
	// 2D Cross product
    static float crossProduct( const Vector2D &v1, const Vector2D &v2 );

    void rotate90CCW();
    void rotate90CW();

    Vector2D &operator+=( const Vector2D &rhs );
    Vector2D &operator-=( const Vector2D &rhs );
    Vector2D &operator*=( const Vector2D &rhs );
    Vector2D &operator*=( const float &rhs );
    Vector2D &operator/=( const Vector2D &rhs );
    Vector2D &operator/=( const float &rhs );

    float &operator[]( const unsigned int &i );
    const float &operator[]( const unsigned int &i ) const;
};
Vector2D operator+( Vector2D lhs, const Vector2D &rhs );
Vector2D operator-( Vector2D lhs, const Vector2D &rhs );
Vector2D operator*( Vector2D lhs, const float &rhs );
Vector2D operator*( const float &lhs, Vector2D rhs );
Vector2D operator*( Vector2D lhs, const Vector2D &rhs );
Vector2D operator/( Vector2D lhs, const Vector2D &rhs );
Vector2D operator/( Vector2D lhs, const float &rhs );
Vector2D operator-( Vector2D vec );
bool operator==( const Vector2D& lhs, const Vector2D& rhs );
bool operator!=( const Vector2D& lhs, const Vector2D& rhs );

struct Vector3D {
    float x;
    float y;
    float z;

    Vector3D();
    Vector3D( float x, float y, float z );
    Vector3D( const Vector2D &v, float z );
    Vector3D( float fill );

    void setVector( float x, float y, float z );

    Vector3D getNormalized() const;
    Vector3D &normalizeThis();
    void normalize();

    float getLength() const;
    float getSquaredLength() const;

    static float dotProduct( const Vector3D &v1, const Vector3D &v2 );
    static Vector3D crossProduct( const Vector3D &v1, const Vector3D &v2 );

    Vector3D &operator+=( const Vector3D &rhs );
    Vector3D &operator-=( const Vector3D &rhs );
    Vector3D &operator*=( const Vector3D &rhs );
    Vector3D &operator*=( const float &rhs );
    Vector3D &operator/=( const Vector3D &rhs );
    Vector3D &operator/=( const float &rhs );

    float &operator[]( const unsigned int &i );
    const float &operator[]( const unsigned int &i ) const;
};
Vector3D operator+( Vector3D lhs, const Vector3D &rhs );
Vector3D operator-( Vector3D lhs, const Vector3D &rhs );
Vector3D operator*( Vector3D lhs, const float &rhs );
Vector3D operator*( const float &lhs, Vector3D rhs );
Vector3D operator*( Vector3D lhs, const Vector3D &rhs );
Vector3D operator/( Vector3D lhs, const Vector3D &rhs );
Vector3D operator/( Vector3D lhs, const float &rhs );
Vector3D operator-( Vector3D vec );
bool operator==( const Vector3D &lhs, const Vector3D &rhs );
bool operator!=( const Vector3D &lhs, const Vector3D &rhs );

struct Vector4D {
    float x;
    float y;
    float z;
    float w;

    Vector4D();
    Vector4D( float x, float y, float z, float w );
    Vector4D( const Vector3D &v, float w );
    Vector4D( float fill );

    void setVector( float x, float y, float z, float w );

    Vector4D getNormalized() const;
    Vector4D &normalizeThis();
    void normalize();

    float getLength() const;
    float getSquaredLength() const;

    static float dotProduct( const Vector4D &v1, const Vector4D &v2 );

    Vector4D &operator+=( const Vector4D &rhs );
    Vector4D &operator-=( const Vector4D &rhs );
    Vector4D &operator*=( const Vector4D &rhs );
    Vector4D &operator*=( const float &rhs );
    Vector4D &operator/=( const Vector4D &rhs );
    Vector4D &operator/=( const float &rhs );

    float &operator[]( const unsigned int &i );
    const float &operator[]( const unsigned int &i ) const;
};
Vector4D operator+( Vector4D lhs, const Vector4D &rhs );
Vector4D operator-( Vector4D lhs, const Vector4D &rhs );
Vector4D operator*( Vector4D lhs, const Vector4D &rhs );
Vector4D operator*( Vector4D lhs, const float &rhs );
Vector4D operator*( const float &lhs, Vector4D rhs );
Vector4D operator/( Vector4D lhs, const Vector4D &rhs );
Vector4D operator/( Vector4D lhs, const float &rhs );
Vector4D operator-( Vector4D vec );
bool operator==( const Vector4D &lhs, const Vector4D &rhs );
bool operator!=( const Vector4D &lhs, const Vector4D &rhs );
