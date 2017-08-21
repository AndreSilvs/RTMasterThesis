#pragma once

#include "API_Headers.h"

#include <string>

#include "ArrayWrapper.h"
#include "PixelArray.h"
#include "Color.h"

/*Revisions
20-08-2015 - Support for PixelArrays and directly accessing pixels
*/

/* This banana will remain here for historical reasons.

/`-.__                                  /\
 `. .  ~~--..__                   __..-' ,'
   `.`-.._     ~~---...___...---~~  _,~,/
     `-._ ~--..__             __..-~ _-~
         ~~-..__ ~~--.....--~~   _.-~
                ~~--...___...--~~
*/

// ============================================================================

struct Clip {
	float top;
	float bottom;
	float right;
	float left;

	Clip();
	Clip(float top, float bottom, float right, float left);
};

// ============================================================================

bool initializeDevIL();

GLuint makeTexture( const GLuint *pixels, const GLuint width, const GLuint height );
GLuint makeTexture( const GLuint *pixels, const GLuint width, const GLuint height, const GLenum type );
GLuint makeTexture( const void *pixels, const GLuint width, const GLuint height, const GLenum internalFormat, const GLenum dataFormat, const GLenum dataType );

class Texture {
public:
    Texture();
	~Texture();

    static bool saveScreenshot( const std::string &fileName, int width, int height );

    bool isLoaded() const;

    void closeTexture();
    bool createEmptyTexture( int width, int height );
    bool createDepthTexture( int width, int height );
    bool createSpecialTexture( int width, int height, GLenum internalFormat, GLenum format );
    bool createSpecialTexture( int width, int height, GLenum internalFormat, GLenum format, void* data );

    bool loadTexture( const std::string &fileName );
    bool loadTexture( const std::string &fileName, GLenum internalFormat, GLenum dataFormat, GLenum dataType );
    bool loadTexture( const Color4C *pixels, int width, int height );
    bool loadTexture( const void *pixels, int width, int height, GLenum internalFormat, GLenum dataFormat, GLenum dataType );

    bool loadTexture_16bit( const std::string &fileName );
    bool loadTexture_16bitRGBA( const std::string &fileName );

    static bool loadPixelsFromImage( const std::string &fileName, PixelArray< Color4C > &pixels );

    bool saveTexture( const std::string &name );
    bool saveBMP( const std::string &name );
    bool savePNG( const std::string &name );
    bool saveJPG( const std::string &name );

    void bindTexture() const;
    void bindTexture( int unit ) const;
    static void unbindTexture();
    static void unbindTexture( int unit );
	int getWidth() const;
	int getHeight() const;

    // Original Width will be a different value if padding was applied
	int getOriginalWidth() const;
    // Original Height will be a different value if padding was applied
	int getOriginalHeight() const;

	GLuint getTextureId() const;

	// GL_LINEAR, GL_NEAREST, ...
	void setMinMagFilter(GLenum minFilter, GLenum magFilter);

    // GL_CLAMP, GL_REPEAT, GL_CLAMP_TO_BORDER, GL_CLAMP_TO_EDGE...
    void setWrapMode( GLenum repeatS, GLenum repeatT );

    void setTextureCompareMode( GLenum compareMode, GLenum compareFunc );

    // Returns whether texture pixel data is currently locally stored
    bool arePixelsLoaded() const;
    // Download texture pixels from the GPU to allow for modifications
    void downloadPixels();
    void downloadPixels( void* dataPtr, GLenum dataFormat, GLenum dataType );
        // Upload pixels back into the GPU texture handle
    // If 'unloadPixels' is false then the local data won't be deallocated and may be re-uploaded
    void uploadPixels( bool unloadPixels = true );
    // Deallocate local pixel storage without uploading
    void discardPixels();

    Color4C *getPixels();
    template <typename T>
    T* getPixels() {
        return reinterpret_cast<T*>( _pixelsAW.getPointer() );
    }

private:
    // Bits per pixel
    int _bpp;
    // Bits per red/green/blue/alpha
    //int _bpr, _bpg, _bpb, _bpa;

    bool _isLoaded;

	int _imageWidth;
	int _imageHeight;

	int _textureWidth;
	int _textureHeight;

	GLuint _textureId;

    bool _pixelsLoaded;
    ArrayWrapper< GLubyte > _pixelsAW;
};

// ============================================================================


class CubemapTexture {
public:
    CubemapTexture();
    ~CubemapTexture();

    void closeTexture();

    bool isLoaded() const;

    bool loadTexture( const std::string &right, const std::string &left,
                      const std::string &top,   const std::string &bottom,
                      const std::string &front, const std::string &back );

    bool loadTexture( const std::string &all );

    void bindTexture() const;

    static void unbindTexture();

    // GL_LINEAR, GL_NEAREST, ...
    void setMinMagFilter( GLenum minFilter, GLenum magFilter );

    void setWrapMode( GLenum wrapS, GLenum wrapT, GLenum wrapR );

private:
    bool _isLoaded;
    GLuint _textureId;
};
