#include "VertexShader.h"

GLuint VertexShader::generateShaderId() {
    return glCreateShader( GL_VERTEX_SHADER );
}
