#pragma once

#include <map>
#include <memory>
#include <string>

#include "Texture.h"

struct TextureOptions {
    GLenum minFilter;
    GLenum magFilter;

    GLenum wrapS;
    GLenum wrapT;

    TextureOptions();
};

class TextureManager {
public:
    TextureManager();
    ~TextureManager();

    bool initialize();
    void clear();

    bool hasTexture( const std::string &textureName );
    bool renameTexture( const std::string &oldName, const std::string &newName );

    bool loadTexture( const std::string &fileName, TextureOptions options = TextureOptions{} );
    bool loadTexture( const std::string &textureName, PixelArray< Color4C > pixels, TextureOptions options = TextureOptions{} );

    void removeTexture( const std::string &textureName );

    Texture *operator[]( const std::string &textureName );

private:
    std::unique_ptr< Texture > _nullTexture;
    std::map< std::string, std::unique_ptr< Texture > > _textures;
};