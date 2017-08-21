#pragma once

#include <list>
#include <map>
#include <string>
#include <vector>

#include "Shader.h"
#include "VertexShader.h"
#include "FragmentShader.h"
#include "GeometryShader.h"
#include "TessellationControlShader.h"
#include "TessellationEvaluationShader.h"

class ShaderProgram {
public:
    ShaderProgram();
    ~ShaderProgram();

    // Initialization

    // Add a new output variable for the transform feedback. Index order is insertion order.
    // Returns the output index
    int addTransformFeedbackVariable( const std::string& name );
    // Use GL_INTERLEAVED_ATTRIBS or GL_SEPARATE_ATTRIBS
    void setTransformFeedbackBufferMode( GLenum bufferMode );
    // Disable and clear previous initialization for transform feedback
    // No effect if the program is already compiled
    void disableTransformFeedback();

    bool compileProgram( const VertexShader &vertexShader, const FragmentShader &fragmentShader );
    bool compileProgram( const std::string &vertexShaderFile, const std::string &fragmentShaderFile );

    // V+G+F
    bool compileProgram( const VertexShader &vertexShader, const GeometryShader &geometryShader, const FragmentShader &fragmentShader );
    bool compileProgram( const std::string &vertexShaderFile, const std::string &geometryShaderFile, const std::string &fragmentShaderFile );

    // V+TC+TE+F shaders
    bool compileProgram( const VertexShader &vertexShader, const TessellationControlShader &tessControlShader,
        const TessellationEvaluationShader &tessEvalShader, const FragmentShader &fragmentShader );
    bool compileProgram( const std::string &vertexShaderFile, const std::string &tessControlFile, const std::string &tessEvalFile, const std::string &fragmentShaderFile );

    // All 5 shaders
    bool compileProgram( const VertexShader &vertexShader,
        const TessellationControlShader &tessControlShader,
        const TessellationEvaluationShader &tessEvalShader,
        const GeometryShader &geometryShader,
        const FragmentShader &fragmentShader );
    bool compileProgram( const std::string &vertexShaderFile,
        const std::string &tessControlFile,
        const std::string &tessEvalFile,
        const std::string &geometryShaderFile,
        const std::string &fragmentShaderFile );

    bool isCompiled() const;
    GLuint getProgramId();

    // Uniforms methods

    GLuint getSubroutineLocation( GLenum shaderType, const std::string &name ) const;

    GLint getAttributeLocation( const std::string &name ) const;
    GLint getUniformLocation( const std::string &name ) const;

    bool readAttributeLocation( const std::string &name, GLint &destination ) const;
    bool readUniformLocation( const std::string &name, GLint &destination ) const;

    std::list< std::string > getAttributeList() const;
    std::list< std::string > getUniformList() const;

    std::list< std::pair< std::string, int > > getAttributeData() const;
    std::list< std::pair< std::string, int > > getUniformData() const;

    void useProgram() const;
    static void disableProgram();

    void deleteProgram();

    const std::string& getLastError() const;

private:

    GLuint _programId;
    bool _isCompiled;

    std::map< std::string, GLint > _attributeLocations;
    std::map< std::string, GLint > _uniformLocations;
    std::map< std::string, GLint > _blockLocations;

    // Initialization data
    std::vector< std::string > _transformFeedbackVariables;
    GLenum _transformFeedbackBufferMode;

    std::string _lastError;

    // Utility for shader compilation
    bool ShaderProgram::compileProgram( const std::vector<const Shader*>& shaders );
};