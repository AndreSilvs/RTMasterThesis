#pragma once

#include "Color.h"
#include "Vector.h"

enum LightType {
    LIGHT_DIRECTIONAL,
    LIGHT_POINT,
    LIGHT_SPOTLIGHT
};

struct Light {
    Light();

    void setPoint( const Vector3D &position );
    void setDirectional( const Vector3D &direction );
    void setSpotlight( const Vector3D &position, const Vector3D &direction );

    Vector3D position;
    Vector3D direction;
    Color3C color;

    float maxIntensity;
    float radius;

    float spotCutoff;

    LightType type;
};

Light createPointLight( const Vector3D &position, float maxIntensity = 1.0f );
Light createPointLight( const Vector3D &position, Color3C color, float maxIntensity = 1.0f );

Light createDirectionalLight( const Vector3D &direction, float maxIntensity = 1.0f );
Light createDirectionalLight( const Vector3D &direction, Color3C color, float maxIntensity = 1.0f );

Light createSpotlight( const Vector3D &position, const Vector3D &direction, float spotCutoff, float maxIntensity = 1.0f );
Light createSpotlight( const Vector3D &position, const Vector3D &direction, Color3C color, float spotCutoff, float maxIntensity = 1.0f );
