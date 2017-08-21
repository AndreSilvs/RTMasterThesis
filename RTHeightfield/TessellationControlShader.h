#pragma once

#include "Shader.h"

class TessellationControlShader : public Shader {
protected:
    GLuint generateShaderId();
};
