#include "TessellationEvaluationShader.h"

GLuint TessellationEvaluationShader::generateShaderId() {
    return glCreateShader( GL_TESS_EVALUATION_SHADER );
}
