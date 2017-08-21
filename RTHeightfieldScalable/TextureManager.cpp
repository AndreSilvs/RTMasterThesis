#include "TextureManager.h"

#include "Logging.h"

#include "StringManipulation.h"

TextureOptions::TextureOptions() :
    minFilter( GL_LINEAR ), magFilter( GL_LINEAR ),
    wrapS( GL_REPEAT ), wrapT( GL_REPEAT )
{

}

TextureManager::TextureManager() {

}
TextureManager::~TextureManager() {
	clear();
}

bool TextureManager::initialize() {
    PixelArray< Color4C > defaultPixels;
    defaultPixels.createPixelArrayEmpty( 16, 16 );
    defaultPixels.fill( Color4C( 255, 255, 255, 255 ) );

    _nullTexture.reset( new Texture() );
    return _nullTexture->loadTexture( defaultPixels.getPixels(), defaultPixels.getWidth(), defaultPixels.getHeight() );
}
void TextureManager::clear() {
    _textures.clear();
}

bool TextureManager::hasTexture( const std::string &textureName ) {
    return _textures.find( textureName ) != _textures.end();
}
bool TextureManager::renameTexture( const std::string &oldName, const std::string &newName ) {
    if ( !hasTexture( oldName ) ) {
        return false;
    }
    if ( hasTexture( newName ) ) {
        return false;
    }

    _textures[ newName ] = std::move( _textures[ oldName ] );
    _textures.erase( oldName );
    return true;
}
bool TextureManager::loadTexture( const std::string &fileName, TextureOptions options ) {
    // Get raw name file (to make texture indexing easier)
    std::string rawFileName = getRawFileName( fileName );

    if ( hasTexture( rawFileName ) ) {
        Logging::error.reportWarning( 0, "Loading " + quote( fileName ) + " failed. Texture name " + quote( rawFileName ) + " already exists." );
        return false;
    }

    std::unique_ptr< Texture > newTexture( new Texture() );
    if ( !newTexture->loadTexture( fileName ) ) {
        Logging::error.reportError( 0, "Unable to load " + quote( fileName ) + "." );
        return false;
    }
    
    _textures[ rawFileName ] = std::move( newTexture );

    Texture *tex = _textures[ rawFileName ].get();
    tex->setMinMagFilter( options.minFilter, options.magFilter );
    tex->setWrapMode( options.wrapS, options.wrapT );
 
    return true;
}

bool TextureManager::loadTexture( const std::string &textureName, PixelArray< Color4C > pixels, TextureOptions options ) {
    if ( hasTexture( textureName ) ) {
        Logging::error.reportWarning( 0, "Loading " + quote( textureName ) + " failed. Texture already exists." );
        return false;
    }

    std::unique_ptr< Texture > newTexture( new Texture() );
    if ( !newTexture->loadTexture( pixels.getPixels(), pixels.getWidth(), pixels.getHeight() ) ) {
        Logging::error.reportError( 0, "Unable to load pixels into texture " + quote( textureName ) );
        return false;
    }

    _textures[ textureName ] = std::move( newTexture );

    Texture *tex = _textures[ textureName ].get();
    tex->setMinMagFilter( options.minFilter, options.magFilter );
    tex->setWrapMode( options.wrapS, options.wrapT );

    return true;
}
//bool loadTexture( const std::string &textureName, std::unique_ptr< Texture > texture );

void TextureManager::removeTexture( const std::string &textureName ) {
    _textures.erase( textureName );
}

Texture *TextureManager::operator[]( const std::string &textureName ) {
    auto it = _textures.find( textureName );
    return ( it == _textures.end() ? _nullTexture.get() : it->second.get() );
}
