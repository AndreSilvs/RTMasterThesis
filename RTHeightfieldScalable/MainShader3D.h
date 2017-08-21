#pragma once

#include "API_Headers.h"

#include "ShaderProgram.h"

#include "MatrixCalcs.h"
#include "MatrixStack.h"
#include "Color.h"

#include "Vao.h"
#include "Texture.h"
#include "Light.h"
#include "Material.h"

namespace MainShader3D {

    struct TextureClip {
        float minX;
        float minY;
        float sizeX;
        float sizeY;

        TextureClip();
        TextureClip( float minX, float minY, float sizeX, float sizeY );
    };

    struct ShaderMaterial {
        GLint ambient;
        GLint diffuse;
        GLint specular;
        GLint opacity;
        GLint shininess;
    };

    struct ShaderLight {
        GLint enabled;
        GLint type;
        GLint position;
        GLint direction;
        GLint color;
        GLint maxIntensity;
        GLint radius;
        GLint spotCutoff;
    };

    extern ShaderProgram shader;

    extern MatrixStack projection;
    extern MatrixStack view;
    extern MatrixStack model;

    bool initializeShader();
    void closeShader();

    void setColor( Color4C color );
    void setMatrixes();
    void setModelMatrix();

    void enableTextures();
    void disableTextures();
    void setTextureUnit( GLenum unit );
    void setTextureClip( float minX, float minY, float sizeX, float sizeY );

    void setMaterial( const Material &material );

    void enableLighting();
    void disableLighting();

    void setLight( const Light &light, int index = 0 );
    void disableLight( int index = 0 );
    void disableAllLights();

    void renderModel( Vao *vao, Texture *texture, const Material &material );
    void renderModel( Vao *vao, Texture *texture, const Material &material, const TextureClip &clip );
    
    void renderCube( Texture *texture, const Material &material );
    void renderCube( Texture *texture, const Material &material, const TextureClip &clip );
}
