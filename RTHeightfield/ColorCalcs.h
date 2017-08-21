#pragma once

#include "Color.h"
#include "Vector.h"

typedef Vector4D Color4F;
typedef Vector3D Color3F;

const Color3F cWHITE{ 1.0f };
const Color3F cBLACK{ 0.0f };

const float cHSL_HUE_RED = 0.0f;
const float cHSL_HUE_PURPLE = 0.1666666f;
const float cHSL_HUE_BLUE = 0.3333333f;
const float cHSL_HUE_CYAN = 0.5f;
const float cHSL_HUE_GREEN = 0.66666666f;
const float cHSL_HUE_YELLOW = 0.83333333f;

Color3C convertVectorToColor( const Color3F& color );
Color3F convertColorToVector( const Color3C& color );

Color4C convertVectorToColor( const Color4F& color );
Color4F convertColorToVector( const Color4C& color );

// Mix color1 with color2: rate = 0.0f -> color1 | rate = 1.0f -> color2
Color3F mixColor( const Color3F& color1, const Color3F& color2, float rate );

// Mix color1 with color2: rate = 0.0f -> color1 | rate = 1.0f -> color2
Color4F mixColor( const Color4F& color1, const Color4F& color2, float rate );

void clampColor( Color3F& color );
void clampColor( Color4F& color );

Color3F convertHSLtoRGB( float hue, float saturation, float lightness );

float luminance( Color3F& color );