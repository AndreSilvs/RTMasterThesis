#pragma once

#include "ShaderProgram.h"
#include "Texture.h"

namespace PostProcShader {

    extern ShaderProgram shader;

    bool initializeShader();
    void closeShader();

    void setTextureUnit( GLenum unit );

    bool isQuantizationEnabled();
    void enableQuantization();
    void disableQuantization();
    void setQuantizationColors( int quantization );

    void renderScene( Texture *texture );
}