#pragma once

#include "ShaderProgram.h"

#include "Matrix.h"

/* These Skybox shaders assume the usage of the following axis:
X -> -Left  +Right
Y -> -Back  +Front
Z -> -Down  +Up

Usage
Initialization:
    if ( !initializeSkyboxShaders() ) {
        return false;
    }
    Skybox skybox; 
    skybox.setCubemap( &cubemap );

Rendering:
    Skybox::pvMatrix = extendMatrix(extractMatrix( view )) * projection;
    gSkybox.render();
*/

class CubemapTexture;

class Skybox {
public:
    static bool initializeSkyboxShaders();
    static void closeShaders();

    Skybox();
    ~Skybox();

    void setCubemap( CubemapTexture *cubemap );

    void render() const;

    static void setTextureUnit( GLenum unit );
    static void setPVMatrix();

    static ShaderProgram shader;

    static Matrix4D pvMatrix;

private:
    static bool createBoxGeometry();
    CubemapTexture *_cubemap;
};