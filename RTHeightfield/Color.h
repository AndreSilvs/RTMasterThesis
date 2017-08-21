#pragma once

struct Color4C;
struct Color3C {
    unsigned char blue;
    unsigned char green;
    unsigned char red;

    Color3C();
    //Color3C( float red, float green, float blue );
    Color3C( int red, int green, int blue );
    Color3C( Color4C color );

    void setColor( unsigned char red, unsigned char green, unsigned char blue );

    int getRed() const;
    int getGreen() const;
    int getBlue() const;

    float fRed() const;
    float fGreen() const;
    float fBlue() const;

    unsigned int getUIntColor() const;

    Color3C vectorMultiply( Color3C color ) const;

    unsigned char &operator[]( const unsigned int &i );
    const unsigned char &operator[]( const unsigned int &i ) const;

    Color3C &operator+=( const Color3C &rhs );
    Color3C &operator-=( const Color3C &rhs );
    Color3C &operator*=( const float &rhs );
};
bool operator==( const Color3C& lhs, const Color3C& rhs );
bool operator!=( const Color3C& lhs, const Color3C& rhs );
Color3C operator+( Color3C lhs, const Color3C &rhs );
Color3C operator-( Color3C lhs, const Color3C &rhs );
Color3C operator*( Color3C lhs, const float &rhs );
Color3C operator*( const float &lhs, Color3C rhs );

struct Color4C {
    unsigned char alpha;
    unsigned char blue;
    unsigned char green;
    unsigned char red;

    Color4C();
    //Color4C( float red, float green, float blue, float alpha );

    // Set RGB values and default to 255 alpha
    Color4C( unsigned char red, unsigned char green, unsigned char blue );
    // Set RGBA values
    Color4C( unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha );
    // Fill with grayscale value and default to 255 alpha
    Color4C( unsigned int gray );
    // Fill with grayscale value
    Color4C( unsigned int gray, unsigned alpha );
    // Construct RGB from Color3C and default to 255 alpha
    Color4C( Color3C color );
    // Construct RGB from Color3C
    Color4C( Color3C color, float alpha );

    void setColor( unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha );

    int getRed() const;
    int getGreen() const;
    int getBlue() const;
    int getAlpha() const;

    float fRed() const;
    float fGreen() const;
    float fBlue() const;
    float fAlpha() const;

    unsigned int getUIntColor() const;

    Color4C vectorMultiply( Color4C color ) const;

    Color4C &operator=( const unsigned int &color );
    unsigned char &operator[]( const unsigned int &i );
    const unsigned char &operator[]( const unsigned int &i ) const;

    Color4C &operator+=( const Color4C &rhs );
    Color4C &operator-=( const Color4C &rhs );
    Color4C &operator*=( const float &rhs );
};
bool operator==( const Color4C& lhs, const Color4C& rhs );
bool operator!=( const Color4C& lhs, const Color4C& rhs );
Color4C operator+( Color4C lhs, const Color4C &rhs );
Color4C operator-( Color4C lhs, const Color4C &rhs );
Color4C operator*( Color4C lhs, const float &rhs );
Color4C operator*( const float &lhs, Color4C rhs );


Color3C maxBoth( Color3C c1, Color3C c2 );
Color4C maxBoth( Color4C c1, Color4C c2 );

// Alpha 0.0f = c1, 1.0f = c2
Color3C mix( Color3C c1, Color3C c2, float alpha );