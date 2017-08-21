#include "Shader.h"

#include "FileReading.h"

//#include <iostream>
#include "Logging.h"
#include "StringManipulation.h"

Shader::Shader() :
    _isCompiled( false ), _shaderId( 0 )
{
}
Shader::~Shader() {
    if ( isCompiled() ) {
        glDeleteShader( _shaderId );
    }
}

bool Shader::loadShader( const std::string &shader, const std::string &shaderName ) {
    if ( isCompiled() ) {
        Logging::error.reportWarning( "Shader is already compiled as " + quote( _shaderName ) + ". Unable to load " + quote( shaderName ) + "." );
        return false;
    }

    GLuint shaderId;

    const GLchar *content[] = { shader.c_str() };
    
    shaderId = generateShaderId();

    glShaderSource( shaderId, 1, content, NULL );

    glCompileShader( shaderId );

    GLint compilationStatus = 0;
    glGetShaderiv( shaderId, GL_COMPILE_STATUS, &compilationStatus );
    if ( compilationStatus == GL_FALSE ) {

        GLint maxLength = 0;
        glGetShaderiv( shaderId, GL_INFO_LOG_LENGTH, &maxLength );

        std::string error;
        error.resize( maxLength );
        glGetShaderInfoLog( shaderId, maxLength, &maxLength, &error[ 0 ] );
        error.pop_back(); // Pop null character
        _lastError = error;

        glDeleteShader( shaderId );

        //std::cout << shaderName << "\n" << error << std::endl;
        Logging::error.reportError( 0, shaderName + "\n" + error );

        return false;
    }

    _shaderId = shaderId;
    _isCompiled = true;
    _shaderName = shaderName;
    return true;
}
bool Shader::loadShaderFile( const std::string &shaderFileName ) {
    std::string shaderContent;
    if ( !readFileIntoString( shaderFileName, shaderContent ) ) {
        Logging::error.reportError( "Shader file " + quote( shaderFileName ) + " could not be loaded. " );
        return false;
    }
    return loadShader( shaderContent, shaderFileName );
}

bool Shader::isCompiled() const {
    return _isCompiled;
}

GLuint Shader::getShaderId() const {
    return _shaderId;
}

const std::string &Shader::getShaderName() const {
    return _shaderName;
}

const std::string &Shader::getLastError() const {
    return _lastError;
}
