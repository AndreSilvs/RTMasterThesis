#include "GeometryShader.h"

GLuint GeometryShader::generateShaderId() {
    return glCreateShader( GL_GEOMETRY_SHADER );
}