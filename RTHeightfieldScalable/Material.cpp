#include "Material.h"

Material::Material() {

}
Material::Material( Color3C color, float op, float sh ) :
    ambientColor( color ), diffuseColor( color ), specularColor( color ),
    opacity( op ), shininess( sh )
{
}
Material::Material( Color3C ambient, Color3C diffuse, Color3C specular, float op, float sh ) :
    ambientColor( ambient ), diffuseColor( diffuse ), specularColor( specular ),
    opacity( op ), shininess( sh )
{
}