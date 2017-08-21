#include "MainShader2D.h"

#include "SystemErrors.h"
#include "Logging.h"
#include "StringManipulation.h"

#include <iostream>

namespace MainShader2D {

    // Out
    ShaderProgram shader;

    MatrixStack projection;
    MatrixStack view;
    MatrixStack model;

    Matrix4D pv;
    Matrix4D pvm;

    Vao vaoQuad;
    
    // Private
    GLint vertexAttribute, textureAttribute;
    GLint pvmMatrix;
    GLint textureUnit, textureClip, colorMultiplier;

    bool createQuad() {
        VaoInitializer qData{ 2 };
        qData.primitiveType = GL_TRIANGLES;

        qData.attributes[ 0 ].name = "position";
        qData.attributes[ 0 ].attributeSize = 2;
        qData.attributes[ 0 ].instanceDivisor = 0;
        qData.attributes[ 0 ].usageHint = GL_STATIC_DRAW;
        qData.attributes[ 0 ].attributeData = {
            -0.5f, -0.5f,
            0.5f, -0.5f,
            0.5f,  0.5f,
            -0.5f,  0.5f
        };
        qData.attributes[ 1 ].name = "texCoords";
        qData.attributes[ 1 ].attributeSize = 2;
        qData.attributes[ 1 ].instanceDivisor = 0;
        qData.attributes[ 1 ].usageHint = GL_STATIC_DRAW;
        qData.attributes[ 1 ].attributeData = {
            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 1.0f
        };

        //IBO data
        qData.useIndices = true;
        qData.indices = { 0, 1, 2, 0, 2, 3 };

        return vaoQuad.createVao( qData );
    }

    bool initializeShader() {
        if ( !shader.compileProgram( "Shaders/VS2D.vert", "Shaders/FS2D.frag" ) ) {
            return false;
        }

        bool success = true;
        if ( !shader.readAttributeLocation( "inVertex", vertexAttribute ) ) { success = false; }
        if ( !shader.readAttributeLocation( "inTexture", textureAttribute ) ) { success = false; }

        if ( !shader.readUniformLocation( "pvmMatrix", pvmMatrix ) ) { success = false; }

        if ( !shader.readUniformLocation( "texture0", textureUnit ) ) { success = false; }
        if ( !shader.readUniformLocation( "textureClip", textureClip ) ) { success = false; }
        if ( !shader.readUniformLocation( "colorMultiplier", colorMultiplier ) ) { success = false; }

        if ( !success ) {
            /*auto unif = shader.getUniformList();
            for ( auto name : unif ) {
                std::cout << name << std::endl;
            }
            getchar();*/
            return false;
        }

        if ( !createQuad() ) {
            return false;
        }

        // Set uniforms default values
        shader.useProgram();

        setTextureUnit( GL_TEXTURE0 );
        setTextureClip( 0, 1, 0, 1 );
        setColor( { 255, 255, 255, 255 } );

        projection.getMatrix().setIdentity();
        view.getMatrix().setIdentity();
        model.getMatrix().setIdentity();
        setMatrixes();

        shader.disableProgram();

        if ( getOpenGLError( "Error occurred while setting uniform values." ) ) {
            return false;
        }

        return true;
    }

    void closeShader() {
        vaoQuad.destroy();
        shader.deleteProgram();
    }

    void setColor( Color4C color ) {
        glUniform4f( colorMultiplier, (float)color.red / 255.f, (float)color.green / 255.f, (float)color.blue / 255.f, (float)color.alpha / 255.f );
    }

    void setMatrixes() {
        pv = view.getMatrix() * projection.getMatrix();
        pvm = model.getMatrix() * pv;
        glUniformMatrix4fv( pvmMatrix, 1, GL_FALSE, pvm.getMatrixAsArray() );
    }
    void setModelMatrix() {
        pvm = model.getMatrix() * pv;
        glUniformMatrix4fv( pvmMatrix, 1, GL_FALSE, pvm.getMatrixAsArray() );
    }

    void setTextureUnit( GLenum unit ) {
        glActiveTexture( unit );
        glUniform1i( textureUnit, unit - GL_TEXTURE0 );
    }
    void setTextureClip( float minX, float minY, float sizeX, float sizeY ) {
        glUniform4f( textureClip, minX, minY, sizeX, sizeY );
    }

    void renderQuad( Texture* texture, Color4C color ) {
        texture->bindTexture();
        setColor( color );
        setTextureClip( 0, 0, 1, 1 );

        model.pushMatrix();
        scaleMatrix( model.getMatrix(), texture->getWidth(), texture->getHeight(), 1.0f );
        setModelMatrix();
        vaoQuad.render();
        model.popMatrix();

        texture->unbindTexture();
    }
    void renderQuad( Texture* texture, float width, float height, Color4C color ) {
        texture->bindTexture();
        setColor( color );
        setTextureClip( 0, 0, 1, 1 );

        model.pushMatrix();
        scaleMatrix( model.getMatrix(), width, height, 1.0f );
        setModelMatrix();
        vaoQuad.render();
        model.popMatrix();

        texture->unbindTexture();
    }
    void renderQuadFit( Texture* texture, float width, float height, Color4C color ) {
        texture->bindTexture();
        setColor( color );
        setTextureClip( 0, 0, width / (float)texture->getWidth(), (float)texture->getHeight() );

        model.pushMatrix();
        scaleMatrix( model.getMatrix(), width, height, 1.0f );
        setModelMatrix();
        vaoQuad.render();
        model.popMatrix();

        texture->unbindTexture();
    }
    void renderQuadFit( Texture* texture, float phaseX, float phaseY, float width, float height, Color4C color ) {
        texture->bindTexture();
        setColor( color );
        setTextureClip( phaseX, phaseY, width / (float)texture->getWidth(), (float)texture->getHeight() );

        model.pushMatrix();
        scaleMatrix( model.getMatrix(), width, height, 1.0f );
        setModelMatrix();
        vaoQuad.render();
        model.popMatrix();

        texture->unbindTexture();
    }
    void renderQuadPhased( Texture *texture, float phaseX, float phaseY, Color4C color ) {
        texture->bindTexture();
        setColor( color );
        setTextureClip( 0 + phaseX, 0 + phaseY, 1, 1 );

        model.pushMatrix();
        scaleMatrix( model.getMatrix(), texture->getWidth(), texture->getHeight(), 1.0f );
        setModelMatrix();
        vaoQuad.render();
        model.popMatrix();

        texture->unbindTexture();
    }
    void renderQuadPhased( Texture *texture, float phaseX, float phaseY, float width, float height, Color4C color ) {
        texture->bindTexture();
        setColor( color );
        setTextureClip( 0 + phaseX, 0 + phaseY, 1, 1 );

        model.pushMatrix();
        scaleMatrix( model.getMatrix(), width, height, 1.0f );
        setModelMatrix();
        vaoQuad.render();
        model.popMatrix();

        texture->unbindTexture();
    }
    void renderQuadRepeating( Texture *texture, float stretchX, float stretchY, Color4C color ) {
        texture->bindTexture();
        setColor( color );
        setTextureClip( 0, 0, stretchX, stretchY );

        model.pushMatrix();
        scaleMatrix( model.getMatrix(), texture->getWidth(), texture->getHeight(), 1.0f );
        setModelMatrix();
        vaoQuad.render();
        model.popMatrix();

        texture->unbindTexture();
    }
    void renderQuadRepeating( Texture *texture, float stretchX, float stretchY, float width, float height, Color4C color ) {
        texture->bindTexture();
        setColor( color );
        setTextureClip( 0, 0, stretchX, stretchY );

        model.pushMatrix();
        scaleMatrix( model.getMatrix(), width, height, 1.0f );
        setModelMatrix();
        vaoQuad.render();
        model.popMatrix();

        texture->unbindTexture();
    }
    void renderQuadRotated( Texture *texture, float orientation, Color4C color ) {
        texture->bindTexture();
        setColor( color );
        setTextureClip( 0, 0, 1, 1 );

        model.pushMatrix();
        rotateZMatrix( model.getMatrix(), orientation );
        scaleMatrix( model.getMatrix(), texture->getWidth(), texture->getHeight(), 1.0f );
        setModelMatrix();
        vaoQuad.render();
        model.popMatrix();

        texture->unbindTexture();
    }
    void renderQuadRotated( Texture *texture, float orientation, float width, float height, Color4C color ) {
        texture->bindTexture();
        setColor( color );
        setTextureClip( 0, 0, 1, 1 );

        model.pushMatrix();
        rotateZMatrix( model.getMatrix(), orientation );
        scaleMatrix( model.getMatrix(), width, height, 1.0f );
        setModelMatrix();
        vaoQuad.render();
        model.popMatrix();

        texture->unbindTexture();
    }
    void renderQuadClip( Texture* texture, int divX, int divY, int x, int y, Color4C color ) {
        texture->bindTexture();

        float xUnits = 1.0f / (float)divX;
        float yUnits = 1.0f / (float)divY;
        setTextureClip( xUnits * (float)x, 1.0f - yUnits * (float)( y + 1 ), xUnits, yUnits );
        setColor( color );

        model.pushMatrix();
        scaleMatrix( model.getMatrix(), (float)texture->getWidth() * xUnits, (float)texture->getHeight() * yUnits, 1.0f );
        setModelMatrix();
        vaoQuad.render();
        model.popMatrix();

        texture->unbindTexture();
    }
    void renderQuadClip( Texture* texture, int divX, int divY, int x, int y, int width, int height, Color4C color ) {
        texture->bindTexture();

        float xUnits = 1.0f / (float)divX;
        float yUnits = 1.0f / (float)divY;
        setTextureClip( xUnits * (float)x, 1.0f - yUnits * (float)( y + 1 ), xUnits, yUnits );
        setColor( color );

        model.pushMatrix();
        scaleMatrix( model.getMatrix(), width, height, 1.0f );
        setModelMatrix();
        vaoQuad.render();
        model.popMatrix();

        texture->unbindTexture();
    }
    void renderQuadClipInPlace( Texture *texture, int cellsX, int cellsY, int x, int y, Color4C color ) {
        float dodgeX = ( 0.5f + (float)x ) * (float)( (float)texture->getWidth() / (float)( cellsX ) );
        float dodgeY = ( 0.5f + (float)y ) * (float)( (float)texture->getHeight() / (float)( cellsY ) );

        model.pushMatrix();
        translateMatrix( MainShader2D::model.getMatrix(), -texture->getWidth() / 2 + dodgeX, texture->getHeight() / 2 - dodgeY, 0 );
        MainShader2D::renderQuadClip( texture, cellsX, cellsY, x, y, color );
        model.popMatrix();
    }
}