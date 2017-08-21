#pragma once

#include "Shader.h"

class TessellationEvaluationShader : public Shader {
protected:
    GLuint generateShaderId();
};
