#include "Texture.h"

#include "Bits.h"

#include "SystemErrors.h"
#include "Logging.h"
#include "StringManipulation.h"

#include <iostream>

//namespace texture {

Clip::Clip(): top(0), bottom(0), right(0), left(0){}
Clip::Clip(float t, float b, float r, float l): top(t), bottom(b), right(r), left(l){}

// ============================================================================
// ============================================================================
// Texture

bool initializeDevIL() {
    //Initialize DevIL
    ilInit();
    ilClearColour( 255, 255, 255, 000 );

    ilEnable( IL_ORIGIN_SET );
    ilOriginFunc( IL_ORIGIN_LOWER_LEFT );

    //Check for error
    ILenum ilError = ilGetError();
    if ( getDevILError( ilError, "Error initializing DevIL." ) ) {
        return false;
    }

    return true;
}


// ============================================================================
// ============================================================================
// Load image - return TextureID (GLuint)

GLuint makeTexture( const GLuint *pixels, const GLuint width, const GLuint height ) {
    GLuint texture = 0;

    //Generate texture ID
    glGenTextures( 1, &texture );

    //Check for error
    if ( getOpenGLError( "Unable to generate TextureID." ) ) {
        return 0;
    }

    //Bind texture ID
    glBindTexture( GL_TEXTURE_2D, texture );

    //Generate texture
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels );

    //Set texture parameters
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

    //Unbind texture
    glBindTexture( GL_TEXTURE_2D, NULL );

    //Check for error
    if ( getOpenGLError( "Error loading texture to GPU." ) ) {
        glDeleteTextures( 1, &texture );
        return 0;
    }

    return texture;
}

GLuint makeTexture( const GLuint *pixels, const GLuint width, const GLuint height, const GLenum type ) {
    GLuint texture = 0;

    //Generate texture ID
    glGenTextures( 1, &texture );

    //Check for error
    if ( getOpenGLError( "Unable to generate TextureID." ) ) {
        return 0;
    }

    //Bind texture ID
    glBindTexture( GL_TEXTURE_2D, texture );

    //Generate texture
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, type, pixels );

    //Set texture parameters
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

    //Unbind texture
    glBindTexture( GL_TEXTURE_2D, NULL );

    //Check for error
    if ( getOpenGLError( "Error loading texture to GPU." ) ) {
        glDeleteTextures( 1, &texture );
        return 0;
    }

    return texture;
}

GLuint makeTexture( const void *pixels, const GLuint width, const GLuint height, const GLenum internalFormat, const GLenum dataFormat, const GLenum dataType ) {
    GLuint texture = 0;

    //Generate texture ID
    glGenTextures( 1, &texture );

    //Check for error
    if ( getOpenGLError( "Unable to generate TextureID." ) ) {
        return 0;
    }

    //Bind texture ID
    glBindTexture( GL_TEXTURE_2D, texture );

    //Generate texture
    glTexImage2D( GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, dataType, pixels );

    //Set texture parameters
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

    //Unbind texture
    glBindTexture( GL_TEXTURE_2D, NULL );

    //Check for error
    if ( getOpenGLError( "Error loading texture to GPU." ) ) {
        glDeleteTextures( 1, &texture );
        return 0;
    }

    return texture;
}



Texture::Texture() :
    _isLoaded( false ), _textureId( 0 ),
    _textureHeight( 0 ), _textureWidth( 0 ),
    _imageHeight( 0 ), _imageWidth( 0 ),
    _pixelsLoaded( false )
{
}

Texture::~Texture() {
    closeTexture();
}

bool Texture::saveScreenshot( const std::string &fileName, int width, int height ) {

    glReadBuffer( GL_BACK );
    PixelArray<GLuint> pixels;
    pixels.createPixelArray( width, height );
    glReadPixels( 0, 0, width, height, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, pixels.getPixels() );

    Texture texture{};
    texture.loadTexture( reinterpret_cast<Color4C*>( pixels.getPixels() ), width, height );

    return texture.savePNG( fileName );
}

bool Texture::isLoaded() const {
    return _isLoaded;
}

bool Texture::assignTextureHandle( GLuint textureHandler ) {
    if ( glIsTexture( textureHandler ) == GL_FALSE ) {
        return false;
    }
    closeTexture();

    GLint texWidth;
    GLint texHeight;
    glBindTexture( GL_TEXTURE_2D, textureHandler );
    glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &texWidth );
    glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &texHeight );
    glBindTexture( GL_TEXTURE_2D, 0 );
    _textureWidth = texWidth; _textureHeight = texHeight;

    _textureId = textureHandler;
    _isLoaded = true;

    return true;
}

void Texture::closeTexture() {
    if ( isLoaded() ) {
        if ( arePixelsLoaded() ) {
            _pixels.deletePixelArray();
            _pixelsLoaded = false;
        }
        glDeleteTextures( 1, &_textureId );
        _textureId = 0;
        _textureHeight = 0;
        _textureWidth = 0;
        _isLoaded = false;
    }
}
bool Texture::createEmptyTexture( int width, int height ) {
    if ( isLoaded() ) {
        closeTexture();
    }

    GLuint textureId = 0;

    glGenTextures( 1, &textureId );

    if ( getOpenGLError( "Unable to generate TextureID." ) ) {
        return false;
    }

    glBindTexture( GL_TEXTURE_2D, textureId );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    glBindTexture( GL_TEXTURE_2D, 0 );

    if ( getOpenGLError( "Error creating empty texture." ) ) {
        glDeleteTextures( 1, &textureId );
        return false;
    }

    _isLoaded = true;
    _textureWidth = _imageWidth = width;
    _textureHeight = _imageHeight = height;
    _textureId = textureId;

    return true;
}
bool Texture::createDepthTexture( int width, int height ) {
    if ( isLoaded() ) {
        closeTexture();
    }

    GLuint textureId = 0;

    glGenTextures( 1, &textureId );

    if ( getOpenGLError( "Unable to generate TextureID." ) ) {
        return false;
    }

    glBindTexture( GL_TEXTURE_2D, textureId );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );
    //glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE );
    //glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL );
    glBindTexture( GL_TEXTURE_2D, 0 );

    if ( getOpenGLError( "Error creating empty texture." ) ) {
        glDeleteTextures( 1, &textureId );
        return false;
    }

    _isLoaded = true;
    _textureWidth = _imageWidth = width;
    _textureHeight = _imageHeight = height;
    _textureId = textureId;

    return true;
}
bool Texture::createSpecialTexture( int width, int height, GLenum internalFormat, GLenum format ) {
    if ( isLoaded() ) {
        closeTexture();
    }

    GLuint textureId = 0;

    glGenTextures( 1, &textureId );

    if ( getOpenGLError( "Unable to generate TextureID." ) ) {
        return false;
    }

    glBindTexture( GL_TEXTURE_2D, textureId );
    glTexImage2D( GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, NULL );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );
    //glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE );
    //glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL );
    glBindTexture( GL_TEXTURE_2D, 0 );

    if ( getOpenGLError( "Error creating empty texture." ) ) {
        glDeleteTextures( 1, &textureId );
        return false;
    }

    _isLoaded = true;
    _textureWidth = _imageWidth = width;
    _textureHeight = _imageHeight = height;
    _textureId = textureId;

    return true;
}
bool Texture::createSpecialTexture( int width, int height, GLenum internalFormat, GLenum format, void* data ) {
    if ( isLoaded() ) {
        closeTexture();
    }

    GLuint textureId = 0;

    glGenTextures( 1, &textureId );

    if ( getOpenGLError( "Unable to generate TextureID." ) ) {
        return false;
    }

    glBindTexture( GL_TEXTURE_2D, textureId );
    glTexImage2D( GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );
    //glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE );
    //glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL );
    glBindTexture( GL_TEXTURE_2D, 0 );

    if ( getOpenGLError( "Error creating empty texture." ) ) {
        glDeleteTextures( 1, &textureId );
        return false;
    }

    _isLoaded = true;
    _textureWidth = _imageWidth = width;
    _textureHeight = _imageHeight = height;
    _textureId = textureId;

    return true;
}
bool Texture::loadTexture( const std::string &fileName ) {
    if ( isLoaded() ) {
        closeTexture();
    }

    //Generate and set current image ID
    ILuint imgID = 0;
    ilGenImages( 1, &imgID );
    ilBindImage( imgID );

    //Load image
    ILboolean success = ilLoadImage( fileName.c_str() );

    //Image loaded successfully
    if ( success == IL_TRUE ) {
        //Convert image to RGBA
        success = ilConvertImage( IL_RGBA, IL_UNSIGNED_BYTE );

        if ( success == IL_TRUE ) {
            //Create texture from file pixels
            GLuint textureId = 0;
            GLuint width = (GLuint)ilGetInteger( IL_IMAGE_WIDTH );
            GLuint height = (GLuint)ilGetInteger( IL_IMAGE_HEIGHT );

            textureId = makeTexture( (GLuint*)ilGetData(), width, height );

            if ( textureId == 0 ) {
                success = IL_FALSE;
            }
            else {
                _textureId = textureId;
                _textureWidth = (int)width;
                _textureHeight = (int)height;
            }
        }  
    }

    ilDeleteImages( 1, &imgID );

    if ( success != IL_TRUE ) {
        getDevILError( ilGetError(), "Failed to load image: " + quote( fileName ) );
        return false;
    }


    _isLoaded = true;
    return true;
}
bool Texture::loadTexture( const std::string &fileName, GLenum internalFormat, GLenum dataFormat, GLenum dataType ) {
    if ( isLoaded() ) {
        closeTexture();
    }

    //Generate and set current image ID
    ILuint imgID = 0;
    ilGenImages( 1, &imgID );
    ilBindImage( imgID );

    //Load image
    ILboolean success = ilLoadImage( fileName.c_str() );

    //Image loaded successfully
    if ( success == IL_TRUE ) {
        //Convert image to RGBA
        success = ilConvertImage( IL_RGBA, IL_UNSIGNED_BYTE );

        if ( success == IL_TRUE ) {
            //Create texture from file pixels
            GLuint textureId = 0;
            GLuint width = (GLuint)ilGetInteger( IL_IMAGE_WIDTH );
            GLuint height = (GLuint)ilGetInteger( IL_IMAGE_HEIGHT );

            textureId = makeTexture( (GLuint*)ilGetData(), width, height, internalFormat, dataFormat, dataType );

            if ( textureId == 0 ) {
                success = IL_FALSE;
            }
            else {
                _textureId = textureId;
                _textureWidth = (int)width;
                _textureHeight = (int)height;
            }
        }
    }

    ilDeleteImages( 1, &imgID );

    if ( success != IL_TRUE ) {
        getDevILError( ilGetError(), "Failed to load image: " + quote( fileName ) );
        return false;
    }


    _isLoaded = true;
    return true;
}
bool Texture::loadTexture( const Color4C *pixels, int width, int height ) {
    if ( isLoaded() ) {
        closeTexture();
    }

    GLuint textureId = makeTexture( reinterpret_cast< const unsigned int * >( pixels ), width, height, GL_UNSIGNED_INT_8_8_8_8 );

    if ( textureId == 0 ) {
        return false;
    }

    _textureId = textureId;
    _textureWidth = width;
    _textureHeight = height;
    _isLoaded = true;

    return true;
}
bool Texture::loadTexture( const Color4C *pixels, int width, int height, GLenum internalFormat, GLenum dataFormat, GLenum dataType ) {
    if ( isLoaded() ) {
        closeTexture();
    }

    GLuint textureId = makeTexture( reinterpret_cast< const unsigned int * >( pixels ), width, height, internalFormat, dataFormat, dataType );

    if ( textureId == 0 ) {
        return false;
    }

    _textureId = textureId;
    _textureWidth = width;
    _textureHeight = height;
    _isLoaded = true;

    return true;
}

bool Texture::loadTexture_16bit( const std::string &fileName ) {
    if ( isLoaded() ) {
        closeTexture();
    }

    //Generate and set current image ID
    ILuint imgID = 0;
    ilGenImages( 1, &imgID );
    ilBindImage( imgID );

    //Load image
    ILboolean success = ilLoadImage( fileName.c_str() );

    //Image loaded successfully
    if ( success == IL_TRUE ) {
        ILint bpp = ilGetInteger( IL_IMAGE_BITS_PER_PIXEL );
        ILint nch = ilGetInteger( IL_IMAGE_CHANNELS );
        ILint iform = ilGetInteger( IL_IMAGE_FORMAT );

        std::ios::fmtflags f( std::cout.flags() );

        /*std::cout << "Bpp: " << bpp << std::endl;
        std::cout << "Channels: " << nch << std::endl;
        std::cout << "Bpc: " << bpp / nch << std::endl;
        std::cout << std::hex;
        std::cout << "IFormat before conv: " << iform << std::endl;
        std::cout.flags( f );*/
        if ( nch != 1 && ( bpp / nch ) != 16 ) {
            std::cout << "Image is not single channel 16-bit." << std::endl;
            return false;
        }

        //Convert image to USHORT - may be unnecessary
        //ilConvertImage( IL_LUMINANCE, IL_UNSIGNED_SHORT );

        //if ( success == IL_TRUE ) {
        //Create texture from file pixels
        GLuint textureId = 0;
        GLuint width = (GLuint)ilGetInteger( IL_IMAGE_WIDTH );
        GLuint height = (GLuint)ilGetInteger( IL_IMAGE_HEIGHT );

        GLushort* imgData = (GLushort*)ilGetData();
        textureId = makeTexture( imgData, width, height, GL_R16, GL_RED, GL_UNSIGNED_SHORT );

        if ( textureId == 0 ) {
            success = IL_FALSE;
        }
        else {
            _textureId = textureId;
            _textureWidth = (int)width;
            _textureHeight = (int)height;
        }
    }

    ilDeleteImages( 1, &imgID );

    if ( success != IL_TRUE ) {
        getDevILError( ilGetError(), "Failed to load image: " + quote( fileName ) );
        return false;
    }


    _isLoaded = true;
    return true;
}

bool Texture::loadTexture_16bitRGBA( const std::string &fileName ) {
    if ( isLoaded() ) {
        closeTexture();
    }

    //Generate and set current image ID
    ILuint imgID = 0;
    ilGenImages( 1, &imgID );
    ilBindImage( imgID );

    //Load image
    ILboolean success = ilLoadImage( fileName.c_str() );

    //Image loaded successfully
    if ( success == IL_TRUE ) {
        ILint bpp = ilGetInteger( IL_IMAGE_BITS_PER_PIXEL );
        ILint nch = ilGetInteger( IL_IMAGE_CHANNELS );
        ILint iform = ilGetInteger( IL_IMAGE_FORMAT );

        /*std::ios::fmtflags f( std::cout.flags() );
        std::cout << "Bpp: " << bpp << std::endl;
        std::cout << "Channels: " << nch << std::endl;
        std::cout << "Bpc: " << bpp / nch << std::endl;
        std::cout << std::hex;
        std::cout << "IFormat before conv: " << iform << std::endl;
        std::cout.flags( f );*/

        if ( ( bpp / nch ) != 16 ) {
            std::cout << "Texture is not in RGBA-16 format." << std::endl;
            return false;
        }

        //Convert image to RGBA
        //success = ilConvertImage( IL_RGBA, IL_UNSIGNED_BYTE );

        //ilConvertImage( IL_RGBA, IL_UNSIGNED_SHORT );

        //if ( success == IL_TRUE ) {
        //Create texture from file pixels
        GLuint textureId = 0;
        GLuint width = (GLuint)ilGetInteger( IL_IMAGE_WIDTH );
        GLuint height = (GLuint)ilGetInteger( IL_IMAGE_HEIGHT );
        GLushort* imgData = (GLushort*)ilGetData();

        textureId = makeTexture( imgData, width, height, GL_RGBA16, GL_RGBA, GL_UNSIGNED_SHORT );

        if ( textureId == 0 ) {
            success = IL_FALSE;
        }
        else {
            _textureId = textureId;
            _textureWidth = (int)width;
            _textureHeight = (int)height;
        }
        //}
    }

    ilDeleteImages( 1, &imgID );

    if ( success != IL_TRUE ) {
        getDevILError( ilGetError(), "Failed to load image: " + quote( fileName ) );
        return false;
    }


    _isLoaded = true;
    return true;
}

bool Texture::loadPixelsFromImage( const std::string &fileName, PixelArray< Color4C > &pixels ) {

    //Generate and set current image ID
    ILuint imgID = 0;
    ilGenImages( 1, &imgID );
    ilBindImage( imgID );

    //Load image
    ILboolean success = ilLoadImage( fileName.c_str() );

    //Image loaded successfully
    if ( success == IL_TRUE ) {
        //Convert image to RGBA
        success = ilConvertImage( IL_RGBA, IL_UNSIGNED_BYTE );

        if ( success == IL_TRUE ) {
            //Create texture from file pixels
            //GLuint textureId = 0;
            GLuint width = (GLuint)ilGetInteger( IL_IMAGE_WIDTH );
            GLuint height = (GLuint)ilGetInteger( IL_IMAGE_HEIGHT );

            pixels.copyPixelArray( reinterpret_cast<const Color4C*>( ilGetData() ), width, height );
        }
    }

    ilDeleteImages( 1, &imgID );

    if ( success != IL_TRUE ) {
        getDevILError( ilGetError(), "Failed to read image file: " + quote( fileName ) );
        return false;
    }

    return true;
}
bool Texture::loadPixelsFromImage16b_RGBA( const std::string &fileName, PixelArray< Color4C_16b > &pixels ) {
    //Generate and set current image ID
    ILuint imgID = 0;
    ilGenImages( 1, &imgID );
    ilBindImage( imgID );

    //Load image
    ILboolean success = ilLoadImage( fileName.c_str() );

    //Image loaded successfully
    if ( success == IL_TRUE ) {
        ILint bpp = ilGetInteger( IL_IMAGE_BITS_PER_PIXEL );
        ILint nch = ilGetInteger( IL_IMAGE_CHANNELS );
        ILint iform = ilGetInteger( IL_IMAGE_FORMAT );

        /*std::ios::fmtflags f( std::cout.flags() );
        std::cout << "Bpp: " << bpp << std::endl;
        std::cout << "Channels: " << nch << std::endl;
        std::cout << "Bpc: " << bpp / nch << std::endl;
        std::cout << std::hex;
        std::cout << "IFormat before conv: " << iform << std::endl;
        std::cout.flags( f );*/

        if ( ( nch != 4 ) || (( bpp / nch ) != 16 ) ) {
            Logging::error.reportError( "Texture is not in RGBA-16 format" );
            std::cout << "Texture is not in RGBA-16 format" << std::endl;
            success = IL_FALSE;
        }
        else {
            //Convert image to RGBA
            //success = ilConvertImage( IL_RGBA, IL_UNSIGNED_BYTE );

            //ilConvertImage( IL_RGBA, IL_UNSIGNED_SHORT );

            //if ( success == IL_TRUE ) {
            //Create texture from file pixels
            GLuint textureId = 0;
            GLuint width = (GLuint)ilGetInteger( IL_IMAGE_WIDTH );
            GLuint height = (GLuint)ilGetInteger( IL_IMAGE_HEIGHT );
            GLushort* imgData = (GLushort*)ilGetData();

            pixels.createPixelArrayEmpty( width, height );
            auto *pixelsIn = (GLushort*)pixels.getPixels();
            for ( int i = 0, end = width*height*4; i < end; ++i ) {
                pixelsIn[ i ] = imgData[ i ];
            }
        }
    }

    ilDeleteImages( 1, &imgID );

    if ( success != IL_TRUE ) {
        getDevILError( ilGetError(), "Failed to read image file: " + quote( fileName ) );
        return false;
    }

    return true;
}
bool Texture::loadCompressedPixelsFromImage( const std::string &fileName, ArrayWrapper< GLubyte > &pixels ) {
    //Generate and set current image ID
    ILuint imgID = 0;
    ilGenImages( 1, &imgID );
    ilBindImage( imgID );

    ilSetInteger( IL_DXTC_FORMAT, IL_DXT1 );

    //Load image
    ILboolean success = ilLoadImage( fileName.c_str() );

    //Image loaded successfully
    if ( success == IL_TRUE ) {
        //Create texture from file pixels
        //GLuint textureId = 0;
        GLuint width = (GLuint)ilGetInteger( IL_IMAGE_WIDTH );
        GLuint height = (GLuint)ilGetInteger( IL_IMAGE_HEIGHT );

        ILuint bufferSize = ilGetDXTCData( NULL, 0, IL_DXT1 );

        if ( bufferSize > 0 ) {
            pixels.reallocate( bufferSize );
            if ( ilGetDXTCData( pixels.getPointer(), bufferSize, IL_DXT1 ) == 0 ) {
                std::cout << "Failed to retrieve compressed data." << std::endl;
                pixels.erase();
                return false;
            }
        }
        else {
            success = IL_FALSE;
        }
    }

    ilDeleteImages( 1, &imgID );

    if ( success != IL_TRUE ) {
        getDevILError( ilGetError(), "Failed to load image: " + quote( fileName ) );
        return false;
    }

    return true;
}

bool Texture::saveTexture( const std::string &name ) {
    if ( !isLoaded() ) {
        return false;
    }

    ILuint imgID = 0;
    ilGenImages( 1, &imgID );
    ilBindImage( imgID );

    bindTexture();

    PixelArray< Color4C > pixels;
    pixels.createPixelArrayEmpty( getWidth(), getHeight() );
    glGetTexImage( GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.getPixels() );
    unbindTexture();

    ilTexImage( pixels.getWidth(), pixels.getHeight(), 1, 4, IL_RGBA, IL_UNSIGNED_BYTE, pixels.getPixels() );

    ILboolean imageSaved = ilSaveImage( name.c_str() );
    ilDeleteImages( 1, &imgID );

    if ( getOpenGLError( "Error saving file." ) ) {
        return false;
    }
    if ( getDevILError( ilGetError(), "Error saving file." ) ) {
        return false;
    }
    if ( imageSaved == GL_FALSE ) {
        Logging::error.reportError( 0, "Image could not be saved." );
        return false;
    }

    return true;
}
bool Texture::saveBMP( const std::string &name ) {
    std::string bmpName = name + ".bmp";
    return saveTexture( bmpName );
}
bool Texture::savePNG( const std::string &name ) {
    std::string pngName = name + ".png";
    return saveTexture( pngName );
}
bool Texture::saveJPG( const std::string &name ) {
    std::string jpgName = name + ".jpg";
    return saveTexture( jpgName );
}

void Texture::bindTexture() const {
    glBindTexture( GL_TEXTURE_2D, _textureId );
}
void Texture::bindTexture( int unit ) const {
    glActiveTexture( GL_TEXTURE0 + unit );
    glBindTexture( GL_TEXTURE_2D, _textureId );
}
void Texture::unbindTexture() {
    glBindTexture( GL_TEXTURE_2D, 0 );
}
void Texture::unbindTexture( int unit ) {
    glActiveTexture( GL_TEXTURE0 + unit );
    glBindTexture( GL_TEXTURE_2D, 0 );
}

int Texture::getWidth() const { return _textureWidth; }
int Texture::getHeight() const { return _textureHeight; }
int Texture::getOriginalWidth() const { return _imageWidth; }
int Texture::getOriginalHeight() const { return _imageHeight; }
GLuint Texture::getTextureId() const { return _textureId; }

// ====================================

void Texture::setMinMagFilter(GLenum minFilter, GLenum magFilter) {
	glBindTexture( GL_TEXTURE_2D , _textureId );

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);

    glBindTexture( GL_TEXTURE_2D, 0 );
}
// ====================================
void Texture::setWrapMode(GLenum wrapS, GLenum wrapT) {
	glBindTexture( GL_TEXTURE_2D , _textureId );

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT );

    glBindTexture( GL_TEXTURE_2D, 0 );
}
// ====================================
void Texture::setTextureCompareMode( GLenum compareMode, GLenum compareFunc ) {
    glBindTexture( GL_TEXTURE_2D, _textureId );

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, compareMode );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, compareFunc );

    glBindTexture( GL_TEXTURE_2D, 0 );
}


bool Texture::arePixelsLoaded() const {
    return _pixelsLoaded;
}
void Texture::downloadPixels() {
    if ( isLoaded() && !arePixelsLoaded() ) {
        bindTexture();
        _pixels.createPixelArrayEmpty( _textureWidth, _textureHeight );
        glGetTexImage( GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, _pixels.getPixels() );
        unbindTexture();

        _pixelsLoaded = true;
    }
}
void Texture::uploadPixels( bool unloadPixels ) {
    if ( isLoaded() && arePixelsLoaded() ) {
        bindTexture();

        glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, _textureWidth, _textureHeight, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, _pixels.getPixels() );
        unbindTexture();

        if ( unloadPixels ) {
            _pixels.deletePixelArray();
            _pixelsLoaded = false;
        }
    }
}
void Texture::discardPixels() {
    if ( isLoaded() && arePixelsLoaded() ) {
        _pixels.deletePixelArray();
        _pixelsLoaded = false;
    }
}

Color4C *Texture::getPixels() {
    return _pixels.getPixels();
}



CubemapTexture::CubemapTexture() :
    _isLoaded( false ), _textureId( 0 )
{
}
CubemapTexture::~CubemapTexture() {
    closeTexture();
}

void CubemapTexture::closeTexture() {
    if ( isLoaded() ) {
        glDeleteTextures( 1, &_textureId );
        getchar();
        _textureId = 0;
        _isLoaded = false;
    }
}

bool CubemapTexture::isLoaded() const {
    return _isLoaded;
}

bool CubemapTexture::loadTexture( const std::string &right, const std::string &left,
    const std::string &top, const std::string &bottom,
    const std::string &front, const std::string &back ) {
    
    if ( isLoaded() ) {
        return false;
    }

    GLuint textureID;
    glGenTextures( 1, &textureID );
    
    //Check for error
    if ( getOpenGLError( "Error creating CubeTexture handler." ) ) {
        return false;
    }

    std::vector< std::string > fileNames{ right, left, bottom, top, back, front };
    std::vector< GLenum > sides{ GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
        GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
        GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z };

    glBindTexture( GL_TEXTURE_CUBE_MAP, textureID );

    bool success = true;
    for ( int i = 0; i < 6; i++ ) {
        PixelArray< Color4C > pixels;

        if ( !Texture::loadPixelsFromImage( fileNames[ i ], pixels ) ) {
            Logging::error.reportError( 0, "Could not load " + quote( fileNames[ i ] ) + ".\n" );
            success = false;
            break;
        }
        
        glTexImage2D( sides[ i ], 0, GL_RGBA, pixels.getWidth(), pixels.getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.getPixels() );
    }

    glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );
    glBindTexture( GL_TEXTURE_CUBE_MAP, 0 );

    //Check for error
    if ( getOpenGLError( "Failed to create CubeMap texture.\n" ) ) {
        success = false;
    }

    if ( !success ) {
        glDeleteTextures( 1, &textureID );
        return false;
    }

    _textureId = textureID;
    _isLoaded = true;

    return true;
}

bool CubemapTexture::loadTexture( const std::string &all ) {
    if ( isLoaded() ) {
        return false;
    }

    GLuint textureID;
    glGenTextures( 1, &textureID );

    //Check for error
    if ( getOpenGLError( "Error creating CubeTexture handler." ) ) {
        return false;
    }

    bool success = true;

    glBindTexture( GL_TEXTURE_CUBE_MAP, textureID );

    PixelArray< Color4C > pixels;
    std::vector< GLenum > sides{ GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
        GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
        GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z };

    if ( !Texture::loadPixelsFromImage( all, pixels ) ) {
        Logging::error.reportError( 0, "Could not load " + quote( all ) + ".\n" );
        success = false;
    }
    else {
        for ( int i = 0; i < 6; ++i ) {
            glTexImage2D( sides[ i ], 0, GL_RGBA, pixels.getWidth(), pixels.getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.getPixels() );
        }
    }

    glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );
    glBindTexture( GL_TEXTURE_CUBE_MAP, 0 );

    //Check for error
    if ( getOpenGLError( "Failed to create CubeMap texture.\n" ) ) {
        success = false;
    }

    if ( !success ) {
        glDeleteTextures( 1, &textureID );
        return false;
    }

    _textureId = textureID;
    _isLoaded = true;

    return true;
}

void CubemapTexture::bindTexture() const {
    glBindTexture( GL_TEXTURE_CUBE_MAP, _textureId );
}

void CubemapTexture::unbindTexture() {
    glBindTexture( GL_TEXTURE_CUBE_MAP, 0 );
}


// GL_LINEAR, GL_NEAREST, ...
void CubemapTexture::setMinMagFilter( GLenum minFilter, GLenum magFilter ) {
    bindTexture();

    glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, minFilter );
    glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, magFilter );

    unbindTexture();
}

// GL_CLAMP, GL_REPEATING, ...
void CubemapTexture::setWrapMode( GLenum wrapS, GLenum wrapT, GLenum wrapR ) {
    bindTexture();

    glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, wrapS );
    glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, wrapT );
    glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, wrapR );

    unbindTexture();
}