#pragma once

#include "API_Headers.h"

#include <memory>
#include <vector>

#include "Texture.h"

struct FBO_BufferType {
    GLenum iFormat;
    GLenum format;
    GLenum attachment;

    FBO_BufferType();
    FBO_BufferType( GLenum iFormat, GLenum format, GLenum attachment );

};

// Use this to initialize a multi target FBO
struct FBO_MultiTargetData {
    std::vector< FBO_BufferType > bufferTypes;
    std::vector<GLenum> outputAttachments;
    int numTargets;
};

class FrameBufferObject {
public:
    FrameBufferObject();
    ~FrameBufferObject();

    bool wasInitialized() const;
    void destroy();

    bool initializeFBO( int width, int height );
    bool initializeFBO_ColorOnly( int width, int height );
    bool initializeFBO_MultiTarget( int width, int height, const FBO_MultiTargetData& data );
    void resize( int width, int height );

    void bindFBO() const;
    static void unbindFBO();

    Texture *getColorTexture();
    Texture *getDepthTexture();
    Texture *getMultiTexture( int index );

    int getWidth() const;
    int getHeight() const;

    void clearBuffer();

    GLuint getFboID();

private:

    bool _isInitialized;

    GLuint _frameBufferID;
    GLuint _depthTextureID;
    GLuint _colorTextureID;
    Texture _colorTexture;
    Texture _depthTexture;

    std::unique_ptr<Texture[]> _multiTexture;

    int _width, _height;

};
