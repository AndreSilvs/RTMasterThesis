#pragma once

#include "Shader.h"

class GeometryShader : public Shader {
protected:
    GLuint generateShaderId();
};

