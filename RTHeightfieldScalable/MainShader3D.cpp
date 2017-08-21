#include "MainShader3D.h"

#include "SystemErrors.h"
#include "Logging.h"
#include "StringManipulation.h"

#include <iostream>

namespace MainShader3D {

    TextureClip::TextureClip() {
    
    }
    TextureClip::TextureClip( float mX, float mY, float sX, float sY ) :
        minX( mX ), minY( mY ), sizeX( sX ), sizeY( sY )
    {
    }

    // Out
    ShaderProgram shader;

    MatrixStack projection;
    MatrixStack view;
    MatrixStack model;

    Matrix4D vm;
    Matrix4D pv;
    Matrix4D pvm;
    Matrix3D normalMat;

    Vao vaoCube;

    // Private
    GLint vertexAttribute, textureAttribute, normalAttribute;
    GLint pvmMatrix, vmMatrix, normalMatrix;
    GLint textureUnit, textureClip, colorMultiplier;
    GLint usingLighting, usingTexture;
    ShaderMaterial shaderMaterial;
    ShaderLight shaderLights[ 8 ];

    bool createCube() {
        // Y up
        /*float vertexData[] = {
            // Z negative (back)
            0.5f, -0.5f, -0.5f,
            -0.5f, -0.5f, -0.5f,
            -0.5f, 0.5f, -0.5f,
            0.5f, 0.5f, -0.5f,

            // Z positive (front)
            -0.5f, -0.5f, 0.5f,
            0.5f, -0.5f, 0.5f,
            0.5f, 0.5f, 0.5f,
            -0.5f, 0.5f, 0.5f,

            // Y positive (top)
            0.5f, 0.5f, -0.5f,
            -0.5f, 0.5f, -0.5f,
            -0.5f, 0.5f, 0.5f,
            0.5f, 0.5f, 0.5f,

            // Y positive (bottom)
            -0.5f, -0.5f, -0.5f,
            0.5f, -0.5f, -0.5f,
            0.5f, -0.5f, 0.5f,
            -0.5f, -0.5f, 0.5f,

            // X positive (left)
            0.5f, -0.5f, 0.5f,
            0.5f, -0.5f, -0.5f,
            0.5f, 0.5f, -0.5f,
            0.5f, 0.5f, 0.5f,

            // X negative (right)
            -0.5f, -0.5f, -0.5f,
            -0.5f, -0.5f, 0.5f,
            -0.5f, 0.5f, 0.5f,
            -0.5f, 0.5f, -0.5f
        };

        float normalData[] = {
            0.0f, 0.0f, -1.0f,
            0.0f, 0.0f, -1.0f,
            0.0f, 0.0f, -1.0f,
            0.0f, 0.0f, -1.0f,

            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,

            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,

            0.0f, -1.0f, 0.0f,
            0.0f, -1.0f, 0.0f,
            0.0f, -1.0f, 0.0f,
            0.0f, -1.0f, 0.0f,

            1.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f,

            -1.0f, 0.0f, 0.0f,
            -1.0f, 0.0f, 0.0f,
            -1.0f, 0.0f, 0.0f,
            -1.0f, 0.0f, 0.0f
        };*/

        VaoInitializer data{ 3 };
        data.primitiveType = GL_TRIANGLES;

        data.attributes[ 0 ].name = "position";
        data.attributes[ 0 ].set( 3, 0, GL_STATIC_DRAW );
        data.attributes[ 0 ].attributeData = {
            // Y positive ( front )
            0.5f, 0.5f, -0.5f,
            -0.5f, 0.5f, -0.5f,
            -0.5f, 0.5f, 0.5f,
            0.5f, 0.5f, 0.5f,

            // Y negative ( back )
            -0.5f, -0.5f, -0.5f,
            0.5f, -0.5f, -0.5f,
            0.5f, -0.5f, 0.5f,
            -0.5f, -0.5f, 0.5f,

            // X positive ( right )
            0.5f, -0.5f, -0.5f,
            0.5f, 0.5f, -0.5f,
            0.5f, 0.5f, 0.5f,
            0.5f, -0.5f, 0.5f,

            // X negative ( left )
            -0.5f, 0.5f, -0.5f,
            -0.5f, -0.5f, -0.5f,
            -0.5f, -0.5f, 0.5f,
            -0.5f, 0.5f, 0.5f,

            // Z positive ( up )
            -0.5f, -0.5f, 0.5f,
            0.5f, -0.5f, 0.5f,
            0.5f, 0.5f, 0.5f,
            -0.5f, 0.5f, 0.5f,

            // Z negative ( down )
            0.5f, -0.5f, -0.5f,
            -0.5f, -0.5f, -0.5f,
            -0.5f, 0.5f, -0.5f,
            0.5f, 0.5f, -0.5f
        };

        data.attributes[ 1 ].name = "normals";
        data.attributes[ 1 ].set( 3, 0, GL_STATIC_DRAW );
        data.attributes[ 1 ].attributeData = {
            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,

            0.0f, -1.0f, 0.0f,
            0.0f, -1.0f, 0.0f,
            0.0f, -1.0f, 0.0f,
            0.0f, -1.0f, 0.0f,

            1.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f,

            -1.0f, 0.0f, 0.0f,
            -1.0f, 0.0f, 0.0f,
            -1.0f, 0.0f, 0.0f,
            -1.0f, 0.0f, 0.0f,

            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,

            0.0f, 0.0f, -1.0f,
            0.0f, 0.0f, -1.0f,
            0.0f, 0.0f, -1.0f,
            0.0f, 0.0f, -1.0f
        };

        data.attributes[ 2 ].name = "texCoords";
        data.attributes[ 2 ].set( 2, 0, GL_STATIC_DRAW );
        data.attributes[ 2 ].attributeData = {
            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 1.0f,

            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 1.0f,

            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 1.0f,

            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 1.0f,

            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 1.0f,

            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 1.0f,
        };

        data.useIndices = true;
        data.indices = {
            0,1,2,0,2,3,
            4,5,6,4,6,7,
            8,9,10,8,10,11,
            12,13,14,12,14,15,
            16,17,18,16,18,19,
            20,21,22,20,22,23
        };

        return vaoCube.createVao( data );
    }

    void closeShader() {
        vaoCube.destroy();
        shader.deleteProgram();
    }

    bool initializeShader() {
        if ( !shader.compileProgram( "Shaders/VS3D.vert", "Shaders/FS3D.frag" ) ) {
            return false;
        }

        bool success = true;
        // Attributes
        if ( !shader.readAttributeLocation( "inVertex", vertexAttribute ) ) { success = false; }
        if ( !shader.readAttributeLocation( "inTexture", textureAttribute ) ) { success = false; }
        if ( !shader.readAttributeLocation( "inNormal", normalAttribute ) ) { success = false; }

        // Vertex Shader uniforms
        if ( !shader.readUniformLocation( "pvmMatrix", pvmMatrix ) ) { success = false; }
        if ( !shader.readUniformLocation( "vmMatrix", vmMatrix ) ) { success = false; }
        if ( !shader.readUniformLocation( "normalMatrix", normalMatrix ) ) { success = false; }

        // Fragment Shader uniforms
        if ( !shader.readUniformLocation( "usingTexture", usingTexture ) ) { success = false; }
        if ( !shader.readUniformLocation( "rgbaColor", colorMultiplier ) ) { success = false; }
        if ( !shader.readUniformLocation( "texture0", textureUnit ) ) { success = false; }
        if ( !shader.readUniformLocation( "textureClip", textureClip ) ) { success = false; }

        // Lights
        if ( !shader.readUniformLocation( "usingLights", usingLighting ) ) { success = false; }
        for ( int i = 0; i < 8; ++i ) {
            if ( !shader.readUniformLocation( "lights[" + std::to_string( i ) + "].enabled", shaderLights[ i ].enabled ) ) { success = false; }
            if ( !shader.readUniformLocation( "lights[" + std::to_string( i ) + "].type", shaderLights[ i ].type ) ) { success = false; }
            if ( !shader.readUniformLocation( "lights[" + std::to_string( i ) + "].position", shaderLights[ i ].position ) ) { success = false; }
            if ( !shader.readUniformLocation( "lights[" + std::to_string( i ) + "].direction", shaderLights[ i ].direction ) ) { success = false; }
            if ( !shader.readUniformLocation( "lights[" + std::to_string( i ) + "].color", shaderLights[ i ].color ) ) { success = false; }
            if ( !shader.readUniformLocation( "lights[" + std::to_string( i ) + "].maxIntensity", shaderLights[ i ].maxIntensity ) ) { success = false; }
            if ( !shader.readUniformLocation( "lights[" + std::to_string( i ) + "].radius", shaderLights[ i ].radius ) ) { success = false; }
            if ( !shader.readUniformLocation( "lights[" + std::to_string( i ) + "].spotCutoff", shaderLights[ i ].spotCutoff ) ) { success = false; }
        }

        // Material
        if ( !shader.readUniformLocation( "material.ambient", shaderMaterial.ambient ) ) { success = false; }
        if ( !shader.readUniformLocation( "material.diffuse", shaderMaterial.diffuse ) ) { success = false; }
        if ( !shader.readUniformLocation( "material.specular", shaderMaterial.specular ) ) { success = false; }
        if ( !shader.readUniformLocation( "material.opacity", shaderMaterial.opacity ) ) { success = false; }
        if ( !shader.readUniformLocation( "material.shininess", shaderMaterial.shininess ) ) { success = false; }

        if ( !success ) {
            /*auto unif = shader.getUniformList();
            for ( auto name : unif ) {
                std::cout << name << std::endl;
            }
            getchar();*/
            return false;
        }

        if ( !createCube() ) {
            return false;
        }

        // Set uniforms default values
        shader.useProgram();

        setTextureUnit( GL_TEXTURE0 );
        enableTextures();
        setColor( { 255, 255, 255, 255 } );

        projection.getMatrix().setIdentity();
        view.getMatrix().setIdentity();
        model.getMatrix().setIdentity();
        setMatrixes();

        enableLighting();
        disableAllLights();
        Light defaultLight = createPointLight( { 0.0f, 0.0f, 1.0f } );
        defaultLight.radius = 32.0f;
        setLight( defaultLight );

        Material defaultMaterial{ { 255, 255, 255 } };
        setMaterial( defaultMaterial );

        shader.disableProgram();

        if ( getOpenGLError( "Error occurred while setting uniform values." ) ) {
            return false;
        }

        return true;
    }

    void setColor( Color4C color ) {
        glUniform4f( colorMultiplier, (float)color.red / 255.f, (float)color.green / 255.f, (float)color.blue / 255.f, (float)color.alpha / 255.f );
    }

    void setMatrixes() {
        pv = view.getMatrix() * projection.getMatrix();
        pvm = model.getMatrix() * pv;
        vm = model.getMatrix() * view.getMatrix();
        normalMat = computeNormalMatrix( vm );

        glUniformMatrix4fv( pvmMatrix, 1, GL_FALSE, pvm.getMatrixAsArray() );
        glUniformMatrix4fv( vmMatrix, 1, GL_FALSE, vm.getMatrixAsArray() );
        glUniformMatrix3fv( normalMatrix, 1, GL_FALSE, normalMat.getMatrixAsArray() );
    }
    void setModelMatrix() {
        pvm = model.getMatrix() * pv;
        vm = model.getMatrix() * view.getMatrix();
        normalMat = computeNormalMatrix( vm );

        glUniformMatrix4fv( pvmMatrix, 1, GL_FALSE, pvm.getMatrixAsArray() );
        glUniformMatrix4fv( vmMatrix, 1, GL_FALSE, vm.getMatrixAsArray() );
        glUniformMatrix3fv( normalMatrix, 1, GL_FALSE, normalMat.getMatrixAsArray() );
    }

    void enableTextures() {
        glUniform1i( usingTexture, 1 );
    }
    void disableTextures() {
        glUniform1i( usingTexture, 0 );
    }

    void setTextureUnit( GLenum unit ) {
        glActiveTexture( unit );
        glUniform1i( textureUnit, unit - GL_TEXTURE0 );
    }

    void setTextureClip( float minX, float minY, float sizeX, float sizeY ) {
        glUniform4f( textureClip, minX, minY, sizeX, sizeY );
    }

    void setMaterial( const Material &material ) {
        glUniform3f( shaderMaterial.ambient, (float)material.ambientColor.red / 255.f, (float)material.ambientColor.green / 255.f, (float)material.ambientColor.blue / 255.f );
        glUniform3f( shaderMaterial.diffuse, (float)material.diffuseColor.red / 255.f, (float)material.diffuseColor.green / 255.f, (float)material.diffuseColor.blue / 255.f );
        glUniform3f( shaderMaterial.specular, (float)material.specularColor.red / 255.f, (float)material.specularColor.green / 255.f, (float)material.specularColor.blue / 255.f );
        glUniform1f( shaderMaterial.opacity, material.opacity );
        glUniform1f( shaderMaterial.shininess, material.shininess );
    }

    void enableLighting() {
        glUniform1i( usingLighting, 1 );
    }
    void disableLighting() {
        glUniform1i( usingLighting, 0 );
    }

    void setLight( const Light &light, int index ) {
        glUniform1i( shaderLights[ index ].enabled, 1 );
        glUniform1i( shaderLights[ index ].type, light.type );
        glUniform3f( shaderLights[ index ].color, (float)light.color.red / 255.f, (float)light.color.green / 255.f, (float)light.color.blue / 255.f );
        glUniform1f( shaderLights[ index ].maxIntensity, light.maxIntensity );

        if ( light.type == 0 ) {
            Vector4D lightCamDirection = view.getMatrix().multiplyTranspose( Vector4D{ light.direction, 0.0f } );
            glUniform4f( shaderLights[ index ].direction, lightCamDirection.x, lightCamDirection.y, lightCamDirection.z, lightCamDirection.w );
        }
        else if ( light.type == 1 ) {
            Vector4D lightCamPosition = view.getMatrix().multiplyTranspose( Vector4D{ light.position, 1.0f } );
            glUniform4f( shaderLights[ index ].position, lightCamPosition.x, lightCamPosition.y, lightCamPosition.z, lightCamPosition.w );
            glUniform1f( shaderLights[ index ].radius, light.radius );
        }
        else if ( light.type == 2 ) {
            Vector4D lightCamDirection = view.getMatrix().multiplyTranspose( Vector4D{ light.direction, 0.0f } );
            glUniform4f( shaderLights[ index ].direction, lightCamDirection.x, lightCamDirection.y, lightCamDirection.z, lightCamDirection.w );

            Vector4D lightCamPosition = view.getMatrix().multiplyTranspose( Vector4D{ light.position, 1.0f } );
            glUniform4f( shaderLights[ index ].position, lightCamPosition.x, lightCamPosition.y, lightCamPosition.z, lightCamPosition.w );

            glUniform1f( shaderLights[ index ].radius, light.radius );
            glUniform1f( shaderLights[ index ].spotCutoff, light.spotCutoff );
        }
    }
    void disableLight( int index ) {
        glUniform1i( shaderLights[ index ].enabled, 0 );
    }
    void disableAllLights() {
        for ( int i = 0; i < 8; ++i ) {
            disableLight( i );
        }
    }

    void renderModel( Vao *vao, Texture *texture, const Material &material ) {
        texture->bindTexture();
        setTextureClip( 0, 0, 1, 1 );
        setMaterial( material );
        setModelMatrix();
        vao->render();
        texture->unbindTexture();
    }
    void renderModel( Vao *vao, Texture *texture, const Material &material, const TextureClip &clip ) {
        texture->bindTexture();
        setTextureClip( clip.minX, clip.minY, clip.sizeX, clip.sizeY );
        setMaterial( material );
        setModelMatrix();
        vao->render();
        texture->unbindTexture();
    }
    void renderCube( Texture *texture, const Material &material ) {
        renderModel( &vaoCube, texture, material );
    }
    void renderCube( Texture *texture, const Material &material, const TextureClip &clip ) {
        renderModel( &vaoCube, texture, material, clip );
    }
}