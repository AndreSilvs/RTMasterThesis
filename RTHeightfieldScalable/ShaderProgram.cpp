#include "ShaderProgram.h"

#include <vector>

#include "Logging.h"
#include "StringManipulation.h"

ShaderProgram::ShaderProgram() :
    _programId( 0 ), _isCompiled( false ),
    _transformFeedbackBufferMode( GL_NONE )
{

}
ShaderProgram::~ShaderProgram() {

}

int ShaderProgram::addTransformFeedbackVariable( const std::string& name ) {
    if ( !isCompiled() ) {
        if ( !name.empty() ) {
            _transformFeedbackVariables.push_back( name );
            return (int)_transformFeedbackVariables.size() - 1;
        }
    }
    return 0;
}
void ShaderProgram::setTransformFeedbackBufferMode( GLenum bufferMode ) {
    if ( !isCompiled() ) {
        _transformFeedbackBufferMode = bufferMode;
    }
}
void ShaderProgram:: disableTransformFeedback() {
    if ( !isCompiled() ) {
        _transformFeedbackVariables.clear();
        _transformFeedbackBufferMode = GL_NONE;
    }
}

void ShaderProgram::deleteProgram() {
    if ( isCompiled() ) {
        glDeleteProgram( _programId );
        _programId = 0;
        _isCompiled = false;
        _lastError.clear();
    }
}

bool ShaderProgram::compileProgram( const VertexShader &vertexShader, const FragmentShader &fragmentShader ) {
    std::vector< const Shader* > shaders;
    shaders.push_back( &vertexShader );
    shaders.push_back( &fragmentShader );

    return compileProgram( shaders );
}
bool ShaderProgram::compileProgram( const std::string &vertexShaderFile, const std::string &fragmentShaderFile ) {
    VertexShader vShader;
    FragmentShader fShader;

    bool success = true;
    success = success && vShader.loadShaderFile( vertexShaderFile );
    success = success && fShader.loadShaderFile( fragmentShaderFile );

    if ( !success ) {
        Logging::error.reportWarning( "Shader program link failed. Shaders were not compiled successfully." );
        _lastError = ( !vShader.isCompiled() ? vShader.getShaderName() + ": " + vShader.getLastError() + "\n" : "" ) +
            ( !fShader.isCompiled() ? fShader.getShaderName() + ": " + fShader.getLastError() + "\n" : "" );
        return false;
    }

    return compileProgram( vShader, fShader );
}

// V+G+F
bool ShaderProgram::compileProgram( const VertexShader &vertexShader, const GeometryShader &geometryShader,
    const FragmentShader &fragmentShader )
{
    std::vector< const Shader* > shaders;
    shaders.push_back( &vertexShader );
    shaders.push_back( &geometryShader );
    shaders.push_back( &fragmentShader );

    return compileProgram( shaders );
}
bool ShaderProgram::compileProgram( const std::string &vertexShaderFile, const std::string &geometryShaderFile,
    const std::string &fragmentShaderFile )
{
    VertexShader vShader;
    GeometryShader gShader;
    FragmentShader fShader;

    bool success = true;
    success = success && vShader.loadShaderFile( vertexShaderFile );
    success = success && gShader.loadShaderFile( geometryShaderFile );
    success = success && fShader.loadShaderFile( fragmentShaderFile );

    if ( !success ) {
        Logging::error.reportWarning( "Shader program link failed. Shaders were not compiled successfully." );
        return false;
    }

    return compileProgram( vShader, gShader, fShader );
}

// V+TC+TE+F shaders
bool ShaderProgram::compileProgram( const VertexShader &vertexShader, const TessellationControlShader &tessControlShader,
    const TessellationEvaluationShader &tessEvalShader, const FragmentShader &fragmentShader )
{
    std::vector< const Shader* > shaders;
    shaders.push_back( &vertexShader );
    shaders.push_back( &tessControlShader );
    shaders.push_back( &tessEvalShader );
    shaders.push_back( &fragmentShader );

    return compileProgram( shaders );
}
bool ShaderProgram::compileProgram( const std::string &vertexShaderFile, const std::string &tessControlFile,
    const std::string &tessEvalFile, const std::string &fragmentShaderFile )
{
    VertexShader vShader;
    TessellationControlShader tcShader;
    TessellationEvaluationShader teShader;
    FragmentShader fShader;

    bool success = true;
    success = success && vShader.loadShaderFile( vertexShaderFile );
    success = success && tcShader.loadShaderFile( tessControlFile );
    success = success && teShader.loadShaderFile( tessEvalFile );
    success = success && fShader.loadShaderFile( fragmentShaderFile );

    if ( !success ) {
        Logging::error.reportWarning( "Shader program link failed. Shaders were not compiled successfully." );
        return false;
    }

    return compileProgram( vShader, tcShader, teShader, fShader );
}

// All 5 shaders
bool ShaderProgram::compileProgram( const VertexShader &vertexShader,
    const TessellationControlShader &tessControlShader,
    const TessellationEvaluationShader &tessEvalShader,
    const GeometryShader &geometryShader,
    const FragmentShader &fragmentShader )
{
    std::vector< const Shader* > shaders;
    shaders.push_back( &vertexShader );
    shaders.push_back( &tessControlShader );
    shaders.push_back( &tessEvalShader );
    shaders.push_back( &geometryShader );
    shaders.push_back( &fragmentShader );

    return compileProgram( shaders );
}
bool ShaderProgram::compileProgram( const std::string &vertexShaderFile,
    const std::string &tessControlFile,
    const std::string &tessEvalFile,
    const std::string &geometryShaderFile,
    const std::string &fragmentShaderFile )
{
    VertexShader vShader;
    TessellationControlShader tcShader;
    TessellationEvaluationShader teShader;
    GeometryShader gShader;
    FragmentShader fShader;

    bool success = true;
    success = success && vShader.loadShaderFile( vertexShaderFile );
    success = success && tcShader.loadShaderFile( tessControlFile );
    success = success && teShader.loadShaderFile( tessEvalFile );
    success = success && gShader.loadShaderFile( geometryShaderFile );
    success = success && fShader.loadShaderFile( fragmentShaderFile );

    if ( !success ) {
        Logging::error.reportWarning( "Shader program link failed. Shaders were not compiled successfully." );
        return false;
    }

    return compileProgram( vShader, tcShader, teShader, gShader, fShader );
}

bool ShaderProgram::isCompiled() const {
    return _isCompiled;
}

GLuint ShaderProgram::getProgramId() {
    return _programId;
}

GLuint ShaderProgram::getSubroutineLocation( GLenum shaderType, const std::string &name ) const {
    return glGetSubroutineIndex( _programId, shaderType, name.c_str() );
    //... get subroutine code ...
}

GLint ShaderProgram::getAttributeLocation( const std::string &name ) const {
    auto it = _attributeLocations.find( name );
    return ( it != _attributeLocations.end() ? it->second : -1 );
}
GLint ShaderProgram::getUniformLocation( const std::string &name ) const {
    auto it = _uniformLocations.find( name );
    return ( it != _uniformLocations.end() ? it->second : -1 );
}

bool ShaderProgram::readAttributeLocation( const std::string &name, GLint &destination ) const {
    destination = getAttributeLocation( name );
    if ( destination < 0 ) {
        Logging::error.reportError( 0, "Unknown attribute " + quote( name ) + "." );
        return false;
    }
    return true;
}

bool ShaderProgram::readUniformLocation( const std::string &name, GLint &destination ) const {
    destination = getUniformLocation( name );
    if ( destination < 0 ) {
        Logging::error.reportError( 0, "Unknown uniform " + quote( name ) + "." );
        return false;
    }
    return true;
}

std::list< std::string > ShaderProgram::getAttributeList() const {
    std::list< std::string > attributes;
    for ( auto it : _attributeLocations ) {
        attributes.push_back( it.first );
    }
    return attributes;
}
std::list< std::string > ShaderProgram::getUniformList() const {
    std::list< std::string > uniforms;
    for ( auto it : _uniformLocations ) {
        uniforms.push_back( it.first );
    }
    return uniforms;
}

std::list< std::pair< std::string, int > > ShaderProgram::getAttributeData() const {
    std::list< std::pair< std::string, int > > attributes;
    for ( auto it : _attributeLocations ) {
        attributes.push_back( { it.first, it.second } );
    }
    return attributes;
}
std::list< std::pair< std::string, int > > ShaderProgram::getUniformData() const {
    std::list< std::pair< std::string, int > > uniforms;
    for ( auto it : _uniformLocations ) {
        uniforms.push_back( { it.first, it.second } );
    }
    return uniforms;
}

void ShaderProgram::useProgram() const {
    glUseProgram( _programId );
}
void ShaderProgram::disableProgram() {
    glUseProgram( 0 );
}



bool ShaderProgram::compileProgram( const std::vector< const Shader* >& shaders ) {
    _lastError.clear();
    // Determine if shaders are already successfully compiled
    bool allCompiled = true;
    for ( int i = 0; i < shaders.size(); ++i ) {
        if ( !shaders[ i ]->isCompiled() ) {
            allCompiled = false;
            _lastError += shaders[ i ]->getLastError() + "\n";
        }
    }
    if ( !allCompiled ) {
        Logging::error.reportWarning( "Shader program link failed. Shaders were not compiled successfully." );
        return false;
    }

    // Create handle for program
    GLuint programId = glCreateProgram();

    // Attach shaders to program
    for ( int i = 0; i < shaders.size(); ++i ) {
        glAttachShader( programId, shaders[ i ]->getShaderId() );
    }

    // Check transform feedback usage
    if ( _transformFeedbackBufferMode != GL_NONE && _transformFeedbackVariables.size() > 0 ) {
        std::vector< const char* > outputs( _transformFeedbackVariables.size(), nullptr );
        for ( int i = 0; i < outputs.size(); ++i ) {
            outputs[ i ] = _transformFeedbackVariables[ i ].c_str();
        }

        glTransformFeedbackVaryings( programId, outputs.size(), &outputs[ 0 ], _transformFeedbackBufferMode );
    }

    // Link program
    glLinkProgram( programId );

    // Check if linking was successful
    GLint programStatus = GL_TRUE;
    glGetProgramiv( programId, GL_LINK_STATUS, &programStatus );
    if ( programStatus != GL_TRUE ) {
        GLint maxLength = 0;
        glGetProgramiv( programId, GL_INFO_LOG_LENGTH, &maxLength );

        std::string error;
        error.resize( maxLength );
        glGetProgramInfoLog( programId, maxLength, &maxLength, &error[ 0 ] );
        error.pop_back(); // Pop null character
        _lastError = error;

        glDeleteProgram( programId );

        //std::cout << "Shader Program\n" << error << std::endl;
        Logging::error.reportError( 0, "Shader Program\n" + error );

        return false;
    }

    // Detach shaders - we don't delete shaders here because they may be supplied
    // directly by the user
    for ( int i = 0; i < shaders.size(); ++i ) {
        glDetachShader( programId, shaders[ i ]->getShaderId() );
    }

    // Store attribute locations
    GLint numAttributes = 0;
    glGetProgramiv( programId, GL_ACTIVE_ATTRIBUTES, &numAttributes );
    GLint attMaxLength = 0;
    glGetProgramiv( programId, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &attMaxLength );
    for ( int attId = 0; attId < numAttributes; ++attId ) {
        GLint length = 0;
        GLint size = 0;
        GLenum type = 0;

        std::string name;
        name.resize( attMaxLength );
        glGetActiveAttrib( programId, attId, attMaxLength, &length, &size, &type, &name[ 0 ] );
        name.resize( length );

        _attributeLocations[ name ] = glGetAttribLocation( programId, name.c_str() );
    }

    // Store uniform locations
    GLint numUniforms = 0;
    glGetProgramiv( programId, GL_ACTIVE_UNIFORMS, &numUniforms );
    GLint unifMaxLength = 0;
    glGetProgramiv( programId, GL_ACTIVE_UNIFORM_MAX_LENGTH, &unifMaxLength );
    for ( int unifId = 0; unifId < numUniforms; ++unifId ) {
        GLint length = 0;
        GLint size = 0;
        GLenum type = 0;

        std::string name;
        name.resize( unifMaxLength );
        glGetActiveUniform( programId, unifId, unifMaxLength, &length, &size, &type, &name[ 0 ] );
        name.resize( length );

        _uniformLocations[ name ] = glGetUniformLocation( programId, name.c_str() );
    }

    // Get blocks -- TODO LATER
    /*GLint numBlocks = 0;
    glGetProgramiv( programId, GL_ACTIVE_UNIFORM_BLOCKS, &numBlocks );
    std::cout << "Blocks: " << numBlocks << std::endl;

    for ( int blockIx = 0; blockIx < numBlocks; ++blockIx ) {
    GLint nameLen;
    glGetActiveUniformBlockiv( programId, blockIx, GL_UNIFORM_BLOCK_NAME_LENGTH, &nameLen );

    std::string name;
    name.resize( nameLen );
    glGetActiveUniformBlockName( programId, blockIx, nameLen, NULL, &name[ 0 ] );
    name.pop_back();

    //_blockLocations[ name ] = blockIx;
    }*/

    _programId = programId;
    _isCompiled = true;

    return true;
}

const std::string& ShaderProgram::getLastError() const {
    return _lastError;
}