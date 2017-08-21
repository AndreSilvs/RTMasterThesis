#include "Light.h"

Light::Light()
{
}

void Light::setPoint( const Vector3D &p ) {
    type = LIGHT_POINT;
    position = p;
}
void Light::setDirectional( const Vector3D &d ) {
    type = LIGHT_DIRECTIONAL;
    direction = d;
}
void Light::setSpotlight( const Vector3D &p, const Vector3D &d ) {
    type = LIGHT_SPOTLIGHT;
    position = p;
    direction = d;
}

Light createPointLight( const Vector3D &position, float maxIntensity ) {
    Light light;
    light.setPoint( position );
    light.maxIntensity = maxIntensity;
    light.radius = 1.0f;
    /*light.constantAttenuation = 1.0f;
    light.linearAttenuation = 1.0f;
    light.quadraticAttenuation = 0.0f;*/
    light.color.setColor( 0xFF, 0xFF, 0xFF );
    return light;
}
Light createPointLight( const Vector3D &position, Color3C color, float maxIntensity ) {
    Light light;
    light.setPoint( position );
    light.maxIntensity = maxIntensity;
    light.radius = 1.0f;
    /*light.constantAttenuation = 1.0f;
    light.linearAttenuation = 1.0f;
    light.quadraticAttenuation = 0.0f;*/
    light.color = color;
    return light;
}

Light createDirectionalLight( const Vector3D &direction, float maxIntensity ) {
    Light light;
    light.setDirectional( direction.getNormalized() );
    light.maxIntensity = maxIntensity;
    light.color.setColor( 0xFF, 0xFF, 0xFF );
    return light;
}
Light createDirectionalLight( const Vector3D &direction, Color3C color, float maxIntensity ) {
    Light light;
    light.setDirectional( direction.getNormalized() );
    light.maxIntensity = maxIntensity;
    light.color = color;
    return light;
}

Light createSpotlight( const Vector3D &position, const Vector3D &direction, float spotCutoff, float maxIntensity ) {
    Light light;
    light.setSpotlight( position, direction );
    light.maxIntensity = maxIntensity;
    light.color.setColor( 0xFF, 0xFF, 0xFF );
    light.radius = 1.0f;
    light.spotCutoff = spotCutoff;
    /*light.constantAttenuation = 1.0f;
    light.linearAttenuation = 1.0f;
    light.quadraticAttenuation = 0.0f;*/
    return light;
}
Light createSpotlight( const Vector3D &position, const Vector3D &direction, Color3C color, float spotCutoff, float maxIntensity ) {
    Light light;
    light.setSpotlight( position, direction );
    light.maxIntensity = maxIntensity;
    light.color = color;
    light.radius = 1.0f;
    light.spotCutoff = spotCutoff;
    return light;
}
