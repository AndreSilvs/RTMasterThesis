#pragma once

#include "Shader.h"

class VertexShader : public Shader {
protected:
    GLuint generateShaderId();
};
