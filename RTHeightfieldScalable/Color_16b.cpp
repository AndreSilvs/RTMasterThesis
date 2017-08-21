#include "Color_16b.h"

#include "Bits.h"

Color4C_16b::Color4C_16b() {
}
//Color4C( float red, float green, float blue, float alpha );


Color4C_16b::Color4C_16b( unsigned short r, unsigned short g, unsigned short b ) :
    red( r ), green( g ), blue( b ), alpha( 65536 ) {
}
    

Color4C_16b::Color4C_16b( unsigned short r, unsigned short g, unsigned short b, unsigned short a ) :
    red( r ), green( g ), blue( b ), alpha( a ) {
}

Color4C_16b::Color4C_16b( unsigned short gray ) :
    red( gray ), green( gray ), blue( gray ), alpha( 65536 ) {
}

Color4C_16b::Color4C_16b( unsigned short gray, unsigned short a ) :
    red( gray ), green( gray ), blue( gray ), alpha( a ) {
}

void Color4C_16b::reverseChannels() {
    unsigned short tbl = blue;
    unsigned short tba = alpha;
    alpha = red;
    blue = green;
    green = tbl;
    red = tba;
}

void Color4C_16b::setColor( unsigned short r, unsigned short g, unsigned short b, unsigned short a ) {
    red = r;
    green = g;
    blue = b;
    alpha = a;
}

int Color4C_16b::getRed() const {
    return red;
}
int Color4C_16b::getGreen() const {
    return green;
}
int Color4C_16b::getBlue() const {
    return blue;
}
int Color4C_16b::getAlpha() const {
    return alpha;
}

float Color4C_16b::fRed() const {
    return float( red ) / 65536.0f;
}
float Color4C_16b::fGreen() const {
    return float( green ) / 65536.0f;
}
float Color4C_16b::fBlue() const {
    return float( blue ) / 65536.0f;
}
float Color4C_16b::fAlpha() const {
    return float( alpha ) / 65536.0f;
}

//unsigned int getUIntColor() const;

//Color4C_16b vectorMultiply( Color4C color ) const;

Color4C_16b &Color4C_16b::operator=( const unsigned int &color ) {
    red = (unsigned short)redRGBA( color ) * 255;
    green = (unsigned short)greenRGBA( color ) * 255;
    blue = (unsigned short)blueRGBA( color ) * 255;
    alpha = (unsigned short)alphaRGBA( color ) * 255;
    return *this;
}
unsigned short &Color4C_16b::operator[]( const unsigned int &i ) {
    return reinterpret_cast< unsigned short* >( this )[ i ];
}
const unsigned short &Color4C_16b::operator[]( const unsigned int &i ) const {
    return reinterpret_cast<const unsigned short* >( this )[ i ];
}