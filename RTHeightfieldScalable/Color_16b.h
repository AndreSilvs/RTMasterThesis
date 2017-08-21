#pragma once

struct Color4C_16b {
    unsigned short alpha;
    unsigned short blue;
    unsigned short green;
    unsigned short red;

    Color4C_16b();
    //Color4C( float red, float green, float blue, float alpha );

    // Set RGB values and default to 255 alpha
    Color4C_16b( unsigned short red, unsigned short green, unsigned short blue );
    // Set RGBA values
    Color4C_16b( unsigned short red, unsigned short green, unsigned short blue, unsigned short alpha );
    // Fill with grayscale value and default to 255 alpha
    Color4C_16b( unsigned short gray );
    // Fill with grayscale value
    Color4C_16b( unsigned short gray, unsigned short alpha );
    // Construct RGB from Color3C and default to 255 alpha
    //Color4C_16b( Color3C color );
    // Construct RGB from Color3C
    //Color4C( Color3C color, float alpha );

    void Color4C_16b::reverseChannels();

    void setColor( unsigned short red, unsigned short green, unsigned short blue, unsigned short alpha );

    int getRed() const;
    int getGreen() const;
    int getBlue() const;
    int getAlpha() const;

    float fRed() const;
    float fGreen() const;
    float fBlue() const;
    float fAlpha() const;

    //unsigned int getUIntColor() const;

    //Color4C_16b vectorMultiply( Color4C color ) const;

    Color4C_16b &operator=( const unsigned int &color );
    unsigned short &operator[]( const unsigned int &i );
    const unsigned short &operator[]( const unsigned int &i ) const;

    //Color4C &operator+=( const Color4C &rhs );
    //Color4C &operator-=( const Color4C &rhs );
    //Color4C &operator*=( const float &rhs );
};
/*bool operator==( const Color4C& lhs, const Color4C& rhs );
bool operator!=( const Color4C& lhs, const Color4C& rhs );
Color4C operator+( Color4C lhs, const Color4C &rhs );
Color4C operator-( Color4C lhs, const Color4C &rhs );
Color4C operator*( Color4C lhs, const float &rhs );
Color4C operator*( const float &lhs, Color4C rhs );*/