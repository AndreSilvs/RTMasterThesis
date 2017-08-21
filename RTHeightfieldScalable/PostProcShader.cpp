#include "PostProcShader.h"

#include "SystemErrors.h"
#include "Vao.h"

#include "MathCalcs.h"

namespace PostProcShader {

    // External
    ShaderProgram shader;


    // Internal
    GLint vertexAttribute = -1;
    GLint textureAttribute = -1;
    GLint textureUnit = -1;
    GLint sQuantization = -1;
    GLint sDoQuantization = -1;

    bool quantizationState = false;

    Vao vaoQuad;

    void closeShader() {
        vaoQuad.destroy();
        shader.deleteProgram();
    }

    bool createQuad() {
        VaoInitializer qData{ 2 };
        qData.primitiveType = GL_TRIANGLES;

        qData.attributes[ 0 ].name = "position";
        qData.attributes[ 0 ].attributeSize = 2;
        qData.attributes[ 0 ].instanceDivisor = 0;
        qData.attributes[ 0 ].usageHint = GL_STATIC_DRAW;
        qData.attributes[ 0 ].attributeData = {
            -1.0f, -1.0f,
            1.0f, -1.0f,
            1.0f,  1.0f,
            -1.0f,  1.0f
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
        if ( !shader.compileProgram( "Shaders/PPVS.vert", "Shaders/PPFS.frag" ) ) {
            return false;
        }

        bool success = true;
        if ( !shader.readAttributeLocation( "inVertex", vertexAttribute ) ) { success = false; }
        if ( !shader.readAttributeLocation( "inTexture", textureAttribute ) ) { success = false; }

        if ( !shader.readUniformLocation( "texture0", textureUnit ) ) { success = false; }
        if ( !shader.readUniformLocation( "doQuantization", sDoQuantization ) ) { success = false; }
        if ( !shader.readUniformLocation( "quantization", sQuantization ) ) { success = false; }

        if ( !success ) {
            return false;
        }

        if ( !createQuad() ) {
            return false;
        }

        // Set uniforms default values
        shader.useProgram();

        setTextureUnit( GL_TEXTURE0 );
        disableQuantization();
        setQuantizationColors( 8 );

        shader.disableProgram();

        if ( getOpenGLError( "Error occurred while setting uniform values." ) ) {
            return false;
        }

        return true;
    }

    void setTextureUnit( GLenum unit ) {
        glActiveTexture( unit );
        glUniform1i( textureUnit, unit - GL_TEXTURE0 );
    }

    void renderScene( Texture *texture ) {
        texture->bindTexture();

        vaoQuad.render();

        texture->unbindTexture();
    }

    bool isQuantizationEnabled() {
        return quantizationState;
    }
    void enableQuantization() {
        glUniform1i( sDoQuantization, 1 );
        quantizationState = true;
    }
    void disableQuantization() {
        glUniform1i( sDoQuantization, 0 );
        quantizationState = false;
    }
    void setQuantizationColors( int quantization ) {
        glUniform1i( sQuantization, mathCalcs::clampToRange( quantization, 2, 255 ) );
    }
}