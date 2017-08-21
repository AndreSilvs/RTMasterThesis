#include "ColorCalcs.h"

#include <cmath>
#include "MathCalcs.h"

Color3C convertVectorToColor( const Color3F& color ) {
    return Color3C( (unsigned char)(color.x * 255.f), (unsigned char)(color.y * 255.f), (unsigned char)(color.z * 255.f) );
}
Color3F convertColorToVector( const Color3C& color ) {
    return Color3F( (float)color.red / 255.f, (float)color.green / 255.f, (float)color.blue / 255.f );
}

Color4C convertVectorToColor( const Color4F& color ) {
    return Color4C( (unsigned char)(color.x * 255.f), (unsigned char)(color.y * 255.f), (unsigned char)(color.z * 255.f), (unsigned char)(color.w * 255.f) );
}
Color4F convertColorToVector( const Color4C& color ) {
    return Color4F( (float)color.red / 255.f, (float)color.green / 255.f, (float)color.blue / 255.f, (float)color.alpha / 255.f );
}


Color3F mixColor( const Color3F& color1, const Color3F& color2, float rate ) {
    return color1 * ( 1.0f - rate ) + color2 * rate;
}
Color4F mixColor( const Color4F& color1, const Color4F& color2, float rate ) {
    return color1 * ( 1.0f - rate ) + color2 * rate;
}


void clampColor( Color3F& color ) {
    color.x = mathCalcs::clampToRange( color.x, 0.0f, 1.0f );
    color.y = mathCalcs::clampToRange( color.y, 0.0f, 1.0f );
    color.z = mathCalcs::clampToRange( color.z, 0.0f, 1.0f );
}
void clampColor( Color4F& color ) {
    color.x = mathCalcs::clampToRange( color.x, 0.0f, 1.0f );
    color.y = mathCalcs::clampToRange( color.y, 0.0f, 1.0f );
    color.z = mathCalcs::clampToRange( color.z, 0.0f, 1.0f );
    color.w = mathCalcs::clampToRange( color.w, 0.0f, 1.0f );
}

Color3F convertHSLtoRGB( float hue, float saturation, float value ) {
    Color3F color;

    // Apply hue
    float theta = hue * 360;
    float angler = mathCalcs::getShortestAngleDifference( theta, 0 );
    float angleg = mathCalcs::getShortestAngleDifference( theta, 240 );
    float angleb = mathCalcs::getShortestAngleDifference( theta, 120 );
    color[ 0 ] = mathCalcs::clampToRange( ( 120.0f - angler ) / 60.0f, 0.0f, 1.0f );
    color[ 1 ] = mathCalcs::clampToRange( ( 120.0f - angleg ) / 60.0f, 0.0f, 1.0f );
    color[ 2 ] = mathCalcs::clampToRange( ( 120.0f - angleb ) / 60.0f, 0.0f, 1.0f );

    // Apply value
    // 0 = black, 0.5f original color, 1 = white
    if ( value > 0.5f ) {
        color = mixColor( color, cWHITE, 2.0f * ( value - 0.5f ) );
    }
    else if ( value < 0.5f ) {
        color = mixColor( cBLACK, color, 2.0f * value );
    }

    // Apply saturation
    Color3F luma{ luminance( color ) };
    return mixColor( luma, color, saturation );
}

float luminance( Color3F& color ) {
    //(0.2126*R + 0.7152*G + 0.0722*B)
    return 0.2126f * color[0] + 0.7152f * color[ 1 ] + 0.0722f * color[ 2 ];
}