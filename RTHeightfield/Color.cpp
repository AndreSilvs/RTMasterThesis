#include "Color.h"

#include "Bits.h"

/********
Color3C
********/

Color3C::Color3C() {
}

/*Color3C::Color3C( float r, float g, float b ) :
red( (int)( r*255.f ) ), green( (int)( g*255.f ) ), blue( (int)( b*255.f ) )
{
}*/

Color3C::Color3C( int r, int g, int b ) :
    red( (unsigned char)r ), green( (unsigned char)g ), blue( (unsigned char)b ) {
}
Color3C::Color3C( Color4C color ) :
    red( color.red ), green( color.green ), blue( color.blue ) {
}

void Color3C::setColor( unsigned char r, unsigned char g, unsigned char b ) {
    red = r;
    green = g;
    blue = b;
}

int Color3C::getRed() const {
    return red;
}

int Color3C::getGreen() const {
    return green;
}

int Color3C::getBlue() const {
    return blue;
}

float Color3C::fRed() const {
    return (float)red / 255.f;
}

float Color3C::fGreen() const {
    return (float)green / 255.f;
}

float Color3C::fBlue() const {
    return (float)blue / 255.f;
}

unsigned int Color3C::getUIntColor() const {
    return RGBAtoUint( red, green, blue, 0xFF );
}

Color3C Color3C::vectorMultiply( Color3C color ) const {
    return Color3C{
        (unsigned char)( (float)red * ( (float)color.red / 255.f ) ),
        (unsigned char)( (float)green * ( (float)color.green / 255.f ) ),
        (unsigned char)( (float)blue * ( (float)color.blue / 255.f ) )
    };
}

unsigned char &Color3C::operator[]( const unsigned int &i ) {
    return reinterpret_cast< unsigned char* >( this )[ i ];
}

const unsigned char &Color3C::operator[]( const unsigned int &i ) const {
    return reinterpret_cast< const unsigned char* >( this )[ i ];
}

Color3C &Color3C::operator+=( const Color3C &rhs ) {
    red += rhs.red;
    green += rhs.green;
    blue += rhs.blue;
    return *this;
}
Color3C &Color3C::operator-=( const Color3C &rhs ) {
    red -= rhs.red;
    green -= rhs.green;
    blue -= rhs.blue;
    return *this;
}
Color3C &Color3C::operator*=( const float &rhs ) {
    red = (unsigned char)( (float)red * rhs );
    green = (unsigned char)( (float)green * rhs );
    blue = (unsigned char)( (float)blue * rhs );
    return *this;
}

bool operator==( const Color3C& lhs, const Color3C& rhs ) {
    return lhs.red == rhs.red && lhs.green == rhs.green &&
        lhs.blue == rhs.blue;
}
bool operator!=( const Color3C& lhs, const Color3C& rhs ) {
    return !operator==( lhs, rhs );
}

Color3C operator+( Color3C lhs, const Color3C &rhs ) {
    return lhs += rhs;
}
Color3C operator-( Color3C lhs, const Color3C &rhs ) {
    return lhs -= rhs;
}
Color3C operator*( Color3C lhs, const float &rhs ) {
    return lhs *= rhs;
}
Color3C operator*( const float &lhs, Color3C rhs ) {
    return rhs *= lhs;
}


/********
Color4C
********/



Color4C::Color4C() {
}

/*Color4C::Color4C( float r, float g, float b, float a ) :
red( (int)( r*255.f ) ), green( (int)( g*255.f ) ), blue( (int)( b*255.f ) ), alpha( (int)( a*255.f ) )
{
}*/

Color4C::Color4C( unsigned char r, unsigned char g, unsigned char b ) :
    red( r ), green( g ), blue( b ), alpha( 255 ) {
}

Color4C::Color4C( unsigned char r, unsigned char g, unsigned char b, unsigned char a ) :
    red( r ), green( g ), blue( b ), alpha( a ) {
}

Color4C::Color4C( unsigned int gray ) :
    red( (unsigned char)redRGBA( gray ) ), green( (unsigned char)greenRGBA( gray ) ), blue( (unsigned char)blueRGBA( gray ) ), alpha( (unsigned char)alphaRGBA( 255 ) ) {
}
Color4C::Color4C( unsigned int gray, unsigned alpha ) :
    red( (unsigned char)redRGBA( gray ) ), green( (unsigned char)greenRGBA( gray ) ), blue( (unsigned char)blueRGBA( gray ) ), alpha( (unsigned char)alphaRGBA( alpha ) ) {

}
Color4C::Color4C( Color3C col ) :
    red( col.red ), green( col.green ), blue( col.blue ), alpha( 255 ) {
}
Color4C::Color4C( Color3C col, float alpha ) :
    red( col.red ), green( col.green ), blue( col.blue ), alpha( (unsigned char)alpha ) {
}

void Color4C::setColor( unsigned char r, unsigned char g, unsigned char b, unsigned char a ) {
    red = r;
    green = g;
    blue = b;
    alpha = a;
}

int Color4C::getRed() const {
    return red;
}

int Color4C::getGreen() const {
    return green;
}

int Color4C::getBlue() const {
    return blue;
}

int Color4C::getAlpha() const {
    return alpha;
}

float Color4C::fRed() const {
    return (float)red / 255.f;
}

float Color4C::fGreen() const {
    return (float)green / 255.f;
}

float Color4C::fBlue() const {
    return (float)blue / 255.f;
}

float Color4C::fAlpha() const {
    return (float)alpha / 255.f;
}

unsigned int Color4C::getUIntColor() const {
    return RGBAtoUint( red, green, blue, alpha );
}

Color4C Color4C::vectorMultiply( Color4C color ) const {
    return Color4C{
        (unsigned char)( (float)red * ( (float)color.red / 255.f ) ),
        (unsigned char)( (float)green * ( (float)color.green / 255.f ) ),
        (unsigned char)( (float)blue * ( (float)color.blue / 255.f ) ),
        (unsigned char)( (float)alpha * ( (float)color.alpha / 255.f ) )
    };
}

Color4C &Color4C::operator=( const unsigned int &color ) {
    red = (unsigned char)redRGBA( color );
    green = (unsigned char)greenRGBA( color );
    blue = (unsigned char)blueRGBA( color );
    alpha = (unsigned char)alphaRGBA( color );
    return *this;
}

unsigned char &Color4C::operator[]( const unsigned int &i ) {
    return reinterpret_cast< unsigned char* >( this )[ i ];
}

const unsigned char &Color4C::operator[]( const unsigned int &i ) const {
    return reinterpret_cast< const unsigned char* >( this )[ i ];
}

Color4C &Color4C::operator+=( const Color4C &rhs ) {
    red += rhs.red;
    green += rhs.green;
    blue += rhs.blue;
    alpha += rhs.alpha;
    return *this;
}
Color4C &Color4C::operator-=( const Color4C &rhs ) {
    red -= rhs.red;
    green -= rhs.green;
    blue -= rhs.blue;
    alpha -= rhs.alpha;
    return *this;
}
Color4C &Color4C::operator*=( const float &rhs ) {
    red = (unsigned char)( (float)red * rhs );
    green = (unsigned char)( (float)green * rhs );
    blue = (unsigned char)( (float)blue * rhs );
    alpha = (unsigned char)( (float)alpha * rhs );
    return *this;
}

bool operator==( const Color4C& lhs, const Color4C& rhs ) {
    return lhs.red == rhs.red && lhs.green == rhs.green &&
        lhs.blue == rhs.blue && lhs.alpha == rhs.alpha;
}
bool operator!=( const Color4C& lhs, const Color4C& rhs ) {
    return !operator==( lhs, rhs );
}

Color4C operator+( Color4C lhs, const Color4C &rhs ) {
    return lhs += rhs;
}
Color4C operator-( Color4C lhs, const Color4C &rhs ) {
    return lhs -= rhs;
}
Color4C operator*( Color4C lhs, const float &rhs ) {
    return lhs *= rhs;
}
Color4C operator*( const float &lhs, Color4C rhs ) {
    return rhs *= lhs;
}

Color3C maxBoth( Color3C c1, Color3C c2 ) {
    return Color3C( c1.red > c2.red ? c1.red : c2.red,
        c1.green > c2.green ? c1.green : c2.green,
        c1.blue > c2.blue ? c1.blue : c2.blue );
}
Color4C maxBoth( Color4C c1, Color4C c2 ) {
    return Color4C( c1.red > c2.red ? c1.red : c2.red,
        c1.green > c2.green ? c1.green : c2.green,
        c1.blue > c2.blue ? c1.blue : c2.blue,
        c1.alpha > c2.alpha ? c1.alpha : c2.alpha );
}

Color3C mix( Color3C c1, Color3C c2, float alpha ) {
    return c1 * ( 1.0f - alpha ) + c2 * alpha;
}