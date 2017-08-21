#pragma once

#include "Shader.h"

class FragmentShader : public Shader {
protected:
    GLuint generateShaderId();
};
