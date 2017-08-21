#pragma once

#include "API_Headers.h"
#include <string>

class Shader {
public:
    Shader();
    ~Shader();

    bool loadShader( const std::string &shader, const std::string &shaderName );
    bool loadShaderFile( const std::string &shaderFileName );

    bool isCompiled() const;
    GLuint getShaderId() const;
    const std::string &getShaderName() const;

    const std::string& getLastError() const;

protected:
    virtual GLuint generateShaderId() = 0;

private:

    std::string _lastError;

    bool _isCompiled;
    GLuint _shaderId;
    std::string _shaderName;
};