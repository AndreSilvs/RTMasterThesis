#pragma once

#include "Color.h"

struct Material {
    Color3C ambientColor;
    Color3C diffuseColor;
    Color3C specularColor;
    float opacity;
    float shininess;

    Material();
    Material( Color3C color, float opacity = 1.0f, float shininess = 1.0f );
    Material( Color3C ambient, Color3C diffuse, Color3C specular, float opacity = 1.0f, float shininess = 1.0f );
};