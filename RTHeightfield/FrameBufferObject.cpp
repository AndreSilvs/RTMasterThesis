#include "FrameBufferObject.h"

#include "SystemErrors.h"

FBO_BufferType::FBO_BufferType() {}
FBO_BufferType::FBO_BufferType( GLenum iFormat, GLenum format, GLenum attachment ) {
    this->iFormat = iFormat;
    this->format = format;
    this->attachment = attachment;
}

FrameBufferObject::FrameBufferObject() :
    _isInitialized( false ), _frameBufferID( 0 ), _depthTextureID( 0 ),
    _colorTextureID( 0 ), _width( 0 ), _height( 0 )
{
}

FrameBufferObject::~FrameBufferObject() {
}

bool FrameBufferObject::wasInitialized() const {
    return _isInitialized;
}
void FrameBufferObject::destroy() {
    if ( wasInitialized() ) {
        _isInitialized = false;

        _depthTexture.closeTexture();
        _colorTexture.closeTexture();
        glDeleteFramebuffers( 1, &_frameBufferID );

        _depthTextureID = 0;
        _colorTextureID = 0;
        _frameBufferID = 0;
        _width = 0;
        _height = 0;
    }
}

bool FrameBufferObject::initializeFBO( int width, int height ) {
    glActiveTexture( GL_TEXTURE0 );

    if ( !_colorTexture.createEmptyTexture( width, height ) ) {
        return false;
    }
    _colorTextureID = _colorTexture.getTextureId();

    /* Depth buffer */
    if ( !_depthTexture.createDepthTexture( width, height ) ) {
        return false;
    }
    _depthTextureID = _depthTexture.getTextureId();

    /* Framebuffer to link everything together */
    glGenFramebuffers( 1, &_frameBufferID );
    glBindFramebuffer( GL_FRAMEBUFFER, _frameBufferID );
    glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _colorTextureID, 0 );
    glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _depthTextureID, 0 );

    if ( glCheckFramebufferStatus( GL_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE ) {
        return false;
    }

    glBindFramebuffer( GL_FRAMEBUFFER, 0 );

    _width = width;
    _height = height;

    _isInitialized = true;

    return true;
}
bool FrameBufferObject::initializeFBO_ColorOnly( int width, int height ) {
    glActiveTexture( GL_TEXTURE0 );

    if ( !_colorTexture.createEmptyTexture( width, height ) ) {
        return false;
    }
    _colorTextureID = _colorTexture.getTextureId();

    /* Framebuffer to link everything together */
    glGenFramebuffers( 1, &_frameBufferID );
    glBindFramebuffer( GL_FRAMEBUFFER, _frameBufferID );
    glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _colorTextureID, 0 );

    if ( glCheckFramebufferStatus( GL_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE ) {
        return false;
    }

    // Set the target for the fragment shader outputs
    /*GLenum drawBufs[] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers( 1, drawBufs );*/

    glBindFramebuffer( GL_FRAMEBUFFER, 0 );

    _width = width;
    _height = height;

    _isInitialized = true;

    return true;
}

bool FrameBufferObject::initializeFBO_MultiTarget( int width, int height, const FBO_MultiTargetData& data ) {
    glActiveTexture( GL_TEXTURE0 );

    _multiTexture.reset( new Texture[ data.numTargets ] );

    for ( int i = 0; i < data.numTargets; ++i ) {
        if ( !_multiTexture[ i ].createSpecialTexture( width, height, data.bufferTypes[ i ].iFormat, data.bufferTypes[ i ].format ) ) {
            //std::cout << "Unable to create special texture!\n";
        }
    }


    /* Framebuffer to link everything together */
    glGenFramebuffers( 1, &_frameBufferID );
    glBindFramebuffer( GL_FRAMEBUFFER, _frameBufferID );

    for ( int i = 0; i < data.numTargets; ++i ) {
        glFramebufferTexture2D( GL_FRAMEBUFFER, data.bufferTypes[ i ].attachment, GL_TEXTURE_2D, _multiTexture[ i ].getTextureId(), 0 );
    }

    glDrawBuffers( data.numTargets, &( data.outputAttachments[ 0 ] ) );

    if ( glCheckFramebufferStatus( GL_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE ) {
        return false;
    }

    glBindFramebuffer( GL_FRAMEBUFFER, 0 );

    return true;
}

void FrameBufferObject::resize( int width, int height ) {
    /*_sceneTexture.createEmptyTexture( width, height );
    _textureHandler = _sceneTexture.getTextureId();

    _depthTexture.createDepthTexture( width, height );
    _renderBuffer = _depthTexture.getTextureId();*/
}

void FrameBufferObject::clearBuffer() {
    //glBindFramebuffer( GL_FRAMEBUFFER, _frameBufferID );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    /*GLfloat bColor[ 4 ] = { 0.0f, 0.0f, 0.0f, 0.0f };
    glClearBufferfv( GL_COLOR, 0, bColor );
    GLfloat depthV = 1.0f;
    glClearBufferfv( GL_DEPTH, 0, &depthV );*/
}

void FrameBufferObject::bindFBO() const {
    // Render to our framebuffer
    glBindFramebuffer( GL_FRAMEBUFFER, _frameBufferID );
}
void FrameBufferObject::unbindFBO() {
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}
Texture *FrameBufferObject::getColorTexture() {
    return &_colorTexture;
}
Texture *FrameBufferObject::getDepthTexture() {
    return &_depthTexture;
}
Texture *FrameBufferObject::getMultiTexture( int index ) {
    return &_multiTexture[ index ];
}
int FrameBufferObject::getWidth() const {
    return _width;
}
int FrameBufferObject::getHeight() const {
    return _height;
}
GLuint FrameBufferObject::getFboID() {
    return _frameBufferID;
}
