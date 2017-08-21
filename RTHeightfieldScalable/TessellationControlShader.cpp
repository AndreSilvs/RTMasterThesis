#include "TessellationControlShader.h"

GLuint TessellationControlShader::generateShaderId() {
    return glCreateShader( GL_TESS_CONTROL_SHADER );
}
