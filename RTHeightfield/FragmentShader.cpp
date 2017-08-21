#include "FragmentShader.h"

GLuint FragmentShader::generateShaderId() {
    return glCreateShader( GL_FRAGMENT_SHADER );
}
