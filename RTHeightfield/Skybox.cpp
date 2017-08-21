#include "Skybox.h"

#include "Logging.h"
#include "Texture.h"

#include "SystemErrors.h"

#include "MatrixCalcs.h"

namespace {
    GLint sVertexAttribute, sTextureAttribute;
    GLint sPvMatrix;

    GLint sTextureUnit;

    bool skyboxCreated;
    GLuint skyboxVao;
    GLuint skyboxVbos[ 2 ];
}

ShaderProgram Skybox::shader;
Matrix4D Skybox::pvMatrix;

bool Skybox::initializeSkyboxShaders() {
    if ( !shader.compileProgram( "Shaders/SkyVS.vert", "Shaders/SkyFS.frag" ) ) {
        Logging::error.reportError( 0, "Shader error." );
        return false;
    }

    bool success = true;
    if ( !shader.readAttributeLocation( "inPosition", sVertexAttribute ) ) { success = false; }
    if ( !shader.readAttributeLocation( "inTexture", sTextureAttribute ) ) { success = false; }

    if ( !shader.readUniformLocation( "pvMatrix", sPvMatrix ) ) { success = false; }
    if ( !shader.readUniformLocation( "skyboxTexture", sTextureUnit ) ) { success = false; }

    if ( !success ) {
        Logging::error.reportError( 0, "Attribute error." );
        return false;
    }

    if ( !createBoxGeometry() ) {
        Logging::error.reportError( 0, "VAO error." );
        return false;
    }

    shader.useProgram();

    setTextureUnit( GL_TEXTURE0 );
    pvMatrix.setIdentity();
    setPVMatrix();

    shader.disableProgram();

    if ( getOpenGLError( "Error occurred while setting uniform values." ) ) {
        return false;
    }

    return true;
}

bool Skybox::createBoxGeometry() {
    GLfloat skyboxVertices[] = {
        -1.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,

        1.0f, 1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,

        1.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,

        -1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, -1.0f,

        -1.0f, 1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,

        -1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, 1.0f,
    };

    // Coords using Z as the up vector
    GLfloat skyboxTexCoords[ 72 ];
    for ( int i = 0; i < 24; ++i ) {
        skyboxTexCoords[ i * 3 + 0 ] = -skyboxVertices[ i * 3 + 0 ]; // x = - x
        skyboxTexCoords[ i * 3 + 1 ] = -skyboxVertices[ i * 3 + 2 ]; // y = - z
        skyboxTexCoords[ i * 3 + 2 ] = -skyboxVertices[ i * 3 + 1 ]; // z = - y
    }

    unsigned int skyboxIndices[] = {
        0, 1, 2, 0, 2, 3,
        4, 5, 6, 4, 6, 7,
        8, 9, 10, 8, 10, 11,
        12, 13, 14, 12, 14, 15,
        16, 17, 18, 16, 18, 19,
        20, 21, 22, 20, 22, 23
    };

    GLuint vao = 0;
    GLuint vbos[ 2 ] = { 0, 0 };

    glGenVertexArrays( 1, &vao ); // Generate VAO handler
    glBindVertexArray( vao ); // Bind the VAO

    glGenBuffers( 2, vbos ); // Generate two VBOs, one for data and the other for indices
    glBindBuffer( GL_ARRAY_BUFFER, vbos[ 0 ] );

    unsigned int vertexSize = sizeof( float ) * 72;
    unsigned int textureSize = sizeof( float ) * 72;

    glBufferData( GL_ARRAY_BUFFER, vertexSize + textureSize, NULL, GL_STATIC_DRAW );

    glBufferSubData( GL_ARRAY_BUFFER, 0, vertexSize, skyboxVertices );
    glBufferSubData( GL_ARRAY_BUFFER, vertexSize, textureSize, skyboxTexCoords );

    glEnableVertexAttribArray( sVertexAttribute );
    glVertexAttribPointer( sVertexAttribute, 3, GL_FLOAT, 0, 0, 0 );
    glEnableVertexAttribArray( sTextureAttribute );
    glVertexAttribPointer( sTextureAttribute, 3, GL_FLOAT, 0, 0, (void *)( vertexSize ) );

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vbos[ 1 ] );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, 36 * sizeof( unsigned int ), skyboxIndices, GL_STATIC_DRAW );

    /*glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );*/
    glBindVertexArray( 0 );

    if ( getOpenGLError() ) {
        glDeleteVertexArrays( 1, &vao );
        glDeleteVertexArrays( 2, vbos );

        ::skyboxCreated = false;
        return false;
    }

    ::skyboxVao = vao;
    ::skyboxVbos[ 0 ] = vbos[ 0 ];
    ::skyboxVbos[ 1 ] = vbos[ 1 ];

    ::skyboxCreated = true;

    return true;
}

void Skybox::closeShaders() {
    if ( ::skyboxCreated ) {
        glDeleteVertexArrays( 1, &::skyboxVao );
        glDeleteVertexArrays( 2, ::skyboxVbos );
        ::skyboxCreated = false;
        Skybox::shader.deleteProgram();
    }
}

void Skybox::setTextureUnit( GLenum unit ) {
    glActiveTexture( unit );
    glUniform1i( sTextureUnit, unit - GL_TEXTURE0 );
}

void Skybox::setPVMatrix() {
    glUniformMatrix4fv( sPvMatrix, 1, GL_FALSE, pvMatrix.getMatrixAsArray() );
}

Skybox::Skybox() :
    _cubemap( nullptr )
{

}
Skybox::~Skybox() {

}

void Skybox::setCubemap( CubemapTexture *cubemap ) {
    _cubemap = cubemap;
}

void Skybox::render() const {
    Skybox::shader.useProgram();

    GLboolean depthMask = GL_TRUE;
    glGetBooleanv( GL_DEPTH_WRITEMASK, &depthMask );

    glDepthMask( GL_FALSE );
    _cubemap->bindTexture();

    setPVMatrix();
    glBindVertexArray( skyboxVao );
    glDrawElements( GL_TRIANGLES, 36, GL_UNSIGNED_INT, NULL );
    glBindVertexArray( 0 );

    _cubemap->unbindTexture();
    glDepthMask( depthMask );

    Skybox::shader.disableProgram();
}