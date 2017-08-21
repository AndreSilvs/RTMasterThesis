#include "HeightfieldSC-16.h"

#include <iostream>
#include "MathCalcs.h"
#include "SystemErrors.h"

#include "ThreadSplitter.h"
#include <cmath>
#include <memory>

#include "DebugTimer.h"

#include "VectorCalcs.h"

HeightfieldScalable16::HeightfieldScalable16() : _loadingTiles( false ) {

}
HeightfieldScalable16::~HeightfieldScalable16() {

}

/*bool HeightfieldScalable16::initializeHeightmap( std::string hmapFileFormat, std::string hmapFileExtension,
    std::string ptexFileFormat, std::string ptexFileExtension,
    int numTilesX, int numTilesY, int tileSize, float loadFar, int regionSize ) {

    _hFileName = hmapFileFormat;
    _hFileExtension = hmapFileExtension;

    _hNumTilesX = numTilesX;
    _hNumTilesY = numTilesY;

    _hTileSize = tileSize;

    _regionSize = regionSize;

    _pFileName = ptexFileFormat;
    _pFileExtension = ptexFileExtension;

    _lastLevel = (int)log2( getHeightmapWidth() );
    _maxMipmapLevel = (int)log2( _hTileSize );

    // Initialize Sparse textures
    GLuint sparseHfCompact, sparsePhotoTex, sparseMaxMip;
    glGenTextures( 1, &sparseHfCompact );
    glGenTextures( 1, &sparsePhotoTex );
    glGenTextures( 1, &sparseMaxMip );

    if ( getOpenGLError( "Error generating texture ids." ) ) {
        return false;
    }

    // Initialize heightfield sparse texture
    glBindTexture( GL_TEXTURE_2D, sparseHfCompact );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_RED );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_GREEN );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_BLUE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_ALPHA );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0 );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    //glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_SPARSE_ARB, GL_TRUE );
    //glTexParameteri( GL_TEXTURE_2D, GL_VIRTUAL_PAGE_SIZE_INDEX_ARB, 0 );
    glTexStorage2D( GL_TEXTURE_2D, 1, GL_RGBA8, _hTileSize * 3, _hTileSize * 3 );
    glBindTexture( GL_TEXTURE_2D, 0 );
    if ( getOpenGLError( "Error happened after initializing Sparse Heightmap." ) ) {
        return false;
    }

    // Initialize maximum mipmap sparse texture
    glBindTexture( GL_TEXTURE_2D, sparseMaxMip );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_RED );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_GREEN );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_BLUE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_ALPHA );
    //glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0 );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    //glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_SPARSE_ARB, GL_TRUE );
    //glTexParameteri( GL_TEXTURE_2D, GL_VIRTUAL_PAGE_SIZE_INDEX_ARB, 0 );
    glTexStorage2D( GL_TEXTURE_2D, _maxMipmapLevel+1, GL_RGBA8, _hTileSize * 3, _hTileSize * 3 );
    glBindTexture( GL_TEXTURE_2D, 0 );
    if ( getOpenGLError( "Error happened after initializing Sparse Maximum Mipmap Tex." ) ) {
        return false;
    }

    // Initialize photo sparse texture
    glBindTexture( GL_TEXTURE_2D, sparsePhotoTex );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_RED );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_GREEN );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_BLUE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_ALPHA );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0 );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    //glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_SPARSE_ARB, GL_TRUE );
    //glTexParameteri( GL_TEXTURE_2D, GL_VIRTUAL_PAGE_SIZE_INDEX_ARB, 0 );
    glTexStorage2D( GL_TEXTURE_2D, 1, GL_RGBA8, _hTileSize * 3, _hTileSize * 3 );
    glBindTexture( GL_TEXTURE_2D, 0 );
    if ( getOpenGLError( "Error happened after initializing Sparse Photo Tex." ) ) {
        return false;
    }

    // Assign handles to textures (only for automatic management)
    _hfCompact.assignTextureHandle( sparseHfCompact );
    _photoTexture.assignTextureHandle( sparsePhotoTex );
    _maximumMipmaps.assignTextureHandle( sparseMaxMip );
    // Maximum mipmap structure will be accessed per index. Interpolation is not needed.
    _maximumMipmaps.setMinMagFilter( GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST );
    _maximumMipmaps.setWrapMode( GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE );
    if ( getOpenGLError( "Error happened after assigning texture handles." ) ) {
        return false;
    }

    // Default center of loaded region to Tile 1,1
    _llTileX = 0;
    _llTileY = 0;
    _tileStates.resize( numTilesX, std::vector< int >( numTilesY, -1 ) );
    _tileOrder = { -1,-1,-1, -1,-1,-1, -1,-1,-1 };
    _availableTiles = { 8, 7, 6, 5, 4, 3, 2, 1, 0 };
    _nextTile = 0;
    _farz = loadFar;

    _tileDimensions.setVector( _hTileSize, _hTileSize );

    for ( int i = 0; i < 9; ++i ) {
        int x = i % 3;
        int y = i / 3;

        if ( !loadTile( x+_llTileX, y+_llTileY, _nextTile ) ) {
            std::cout << "Failed to initialize tile" << std::endl;
            return false;
        }
        _nextTile = ( i + 1 ) % 9;
    }

    userPosition.setVector( 0, 0, 0 );
    boundsLL.setVector( 0.0f, 0.0f ); // tileSizeX * ( tileCenterX-1), tileSizeY * (tileCenterY-1)
    boundsDIM.setVector( _hTileSize * 3, _hTileSize * 3 );

    spawnWorkerThread();

    return true;
}*/


bool HeightfieldScalable16::initializeHeightmap( std::string hmapFileFormat, std::string hmapFileExtension,
    std::string ptexFileFormat, std::string ptexFileExtension,
    int numTilesX, int numTilesY, int tileSize, float loadFar, float mapHeight, float patchSize, int regionSize ) {

    _hFileName = hmapFileFormat;
    _hFileExtension = hmapFileExtension;

    _hNumTilesX = numTilesX;
    _hNumTilesY = numTilesY;

    _hTileSize = tileSize;

    _regionSize = regionSize;

    _pFileName = ptexFileFormat;
    _pFileExtension = ptexFileExtension;

    _lastLevel = (int)log2( getHeightmapWidth() );
    _maxMipmapLevel = (int)log2( _hTileSize );

    _mapHeight = mapHeight;
    _patchSize = patchSize;

    // Initialize Sparse textures
    GLuint sparseHfCompact, sparsePhotoTex, sparseMaxMip, sparseNormalMap;
    glGenTextures( 1, &sparseHfCompact );
    glGenTextures( 1, &sparsePhotoTex );
    glGenTextures( 1, &sparseMaxMip );
    glGenTextures( 1, &sparseNormalMap );

    if ( getOpenGLError( "Error generating texture ids." ) ) {
        return false;
    }

    // Initialize heightfield sparse texture
    glBindTexture( GL_TEXTURE_2D, sparseHfCompact );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_RED );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_GREEN );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_BLUE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_ALPHA );
    //glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0 );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    //glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_SPARSE_ARB, GL_TRUE );
    //glTexParameteri( GL_TEXTURE_2D, GL_VIRTUAL_PAGE_SIZE_INDEX_ARB, 0 );
    glTexStorage2D( GL_TEXTURE_2D, _maxMipmapLevel + 1, GL_RGBA16, _hTileSize * 3, _hTileSize * 3 );
    glBindTexture( GL_TEXTURE_2D, 0 );
    if ( getOpenGLError( "Error happened after initializing Sparse Heightmap." ) ) {
        return false;
    }

    // Initialize maximum mipmap sparse texture
    glBindTexture( GL_TEXTURE_2D, sparseMaxMip );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_RED );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_GREEN );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_BLUE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_ALPHA );
    //glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0 );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    //glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_SPARSE_ARB, GL_TRUE );
    //glTexParameteri( GL_TEXTURE_2D, GL_VIRTUAL_PAGE_SIZE_INDEX_ARB, 0 );
    glTexStorage2D( GL_TEXTURE_2D, _maxMipmapLevel + 1, GL_R16, _hTileSize * 3, _hTileSize * 3 );
    glBindTexture( GL_TEXTURE_2D, 0 );
    if ( getOpenGLError( "Error happened after initializing Sparse Maximum Mipmap Tex." ) ) {
        return false;
    }

    // Initialize photo sparse texture
    glBindTexture( GL_TEXTURE_2D, sparsePhotoTex );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_RED );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_GREEN );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_BLUE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_ALPHA );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0 );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    //glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_SPARSE_ARB, GL_TRUE );
    //glTexParameteri( GL_TEXTURE_2D, GL_VIRTUAL_PAGE_SIZE_INDEX_ARB, 0 );
    glTexStorage2D( GL_TEXTURE_2D, 1, GL_RGBA8, _hTileSize * 3, _hTileSize * 3 );
    glBindTexture( GL_TEXTURE_2D, 0 );
    if ( getOpenGLError( "Error happened after initializing Sparse Photo Tex." ) ) {
        return false;
    }

    // Initialize normal map
    _normalW = _hTileSize;
    _normalH = _hTileSize;
    glBindTexture( GL_TEXTURE_2D, sparseNormalMap );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_RED );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_GREEN );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_BLUE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_ALPHA );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0 );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    //glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_SPARSE_ARB, GL_TRUE );
    //glTexParameteri( GL_TEXTURE_2D, GL_VIRTUAL_PAGE_SIZE_INDEX_ARB, 0 );
    glTexStorage2D( GL_TEXTURE_2D, 1, GL_RGB8, _normalW * 3, _normalH * 3 );
    glBindTexture( GL_TEXTURE_2D, 0 );
    if ( getOpenGLError( "Error happened after initializing Sparse Normal Map." ) ) {
        return false;
    }

    // Assign handles to textures (only for automatic management)
    _hfCompact.assignTextureHandle( sparseHfCompact );
    _photoTexture.assignTextureHandle( sparsePhotoTex );
    _maximumMipmaps.assignTextureHandle( sparseMaxMip );
    _normalMap.assignTextureHandle( sparseNormalMap );
    // Maximum mipmap structure will be accessed per index. Interpolation is not needed.
    _maximumMipmaps.setMinMagFilter( GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST );
    _maximumMipmaps.setWrapMode( GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE );
    if ( getOpenGLError( "Error happened after assigning texture handles." ) ) {
        return false;
    }

    // Default center of loaded region to Tile 1,1
    _llTileX = 0;
    _llTileY = 0;
    _tileStates.resize( numTilesX, std::vector< int >( numTilesY, -1 ) );
    _tileOrder = { -1,-1,-1, -1,-1,-1, -1,-1,-1 };
    _availableTiles = { 8, 7, 6, 5, 4, 3, 2, 1, 0 };
    _nextTile = 0;
    _farz = loadFar;

    _tileDimensions.setVector( _hTileSize, _hTileSize );

    for ( int i = 0; i < 9; ++i ) {
        int x = i % 3;
        int y = i / 3;

        if ( !loadTile( x + _llTileX, y + _llTileY, _nextTile ) ) {
            std::cout << "Failed to initialize tile" << std::endl;
            return false;
        }
        _nextTile = ( i + 1 ) % 9;
    }

    userPosition.setVector( 0, 0, 0 );
    boundsLL.setVector( 0.0f, 0.0f ); // tileSizeX * ( tileCenterX-1), tileSizeY * (tileCenterY-1)
    boundsDIM.setVector( _hTileSize * 3, _hTileSize * 3 );

    spawnWorkerThread();

    return true;
}

bool HeightfieldScalable16::loadCompactFormat( const std::string& file ) {

    DebugTimer tmr;

    Texture hfTex;

    tmr.start();
    std::cout << "Loading heightmap image: ";
    // Load texture - assuming dimensions equal to POWER-OF-TWO+1 (ex: 257, 1025, etc..)
    if ( !hfTex.loadTexture( file ) ) {
        std::cout << "Failed." << std::endl;
        return false;
    }
    tmr.stop();
    std::cout << " Complete. " << tmr.getTimeElapsed() / 1000.0 << "s" << std::endl;
    hfTex.downloadPixels();

    int width = hfTex.getWidth(); // POT+1
    int height = hfTex.getHeight(); // POT+1

    int newWidth = width - 1; // POT
    int newHeight = height - 1; // POT

                                /*std::cout << "W:  " << width << " " << " H:  " << height << std::endl;
                                std::cout << "NW: " << newWidth << " " << " NH: " << newHeight << std::endl;*/

    auto pixels = hfTex.getPixels<Color4C>();
    PixelArray<Color4C> compact;
    compact.createPixelArrayEmpty( newWidth, newHeight ); // Ex: 1024x1024

    // Convert heightfield to compact format
    /*
    red  -x = p00
    green-y = p10
    blue -z = p01
    alpha-w = p11
    */
    // For each pixel xy, copy the height values from the surrounding patch in the original image
    // into a single pixel in the compact buffer
    tmr.start();
    ThreadSplitter::threadizeDual(
        [&]( int xa, int xb, int ya, int yb ) {
        for ( int y = ya; y < yb; ++y ) {
            for ( int x = xa; x < xb; ++x ) {
                compact.getPixel( x, y ).red = pixels[ y* width + x ].red;
                compact.getPixel( x, y ).green = pixels[ y* width + x + 1 ].red;
                compact.getPixel( x, y ).blue = pixels[ ( y + 1 )* width + x ].red;
                compact.getPixel( x, y ).alpha = pixels[ ( y + 1 )* width + x + 1 ].red;
            }
        }
    },
        newWidth, newHeight, 8 );
    tmr.stop();
    std::cout << "Reformat heightfield: " << tmr.getTimeElapsed() / 1000.0 << "s" << std::endl;
    // No need to keep the heightmap texture open
    hfTex.closeTexture();

    // Create a texture from the compact buffer (ex: 1024x1024)
    /*if ( !_hfCompact.createSpecialTexture( newWidth, newHeight, GL_COMPRESSED_RGBA, GL_RGBA, compact.getPixels() ) ) {
    return false;
    }*/

    tmr.start();
    if ( !_hfCompact.loadTexture( compact.getPixels(), newWidth, newHeight ) ) {
        std::cout << "Error loading image to GPU." << std::endl;
        getchar();
        return false;
    }
    tmr.stop();
    std::cout << "Upload heightfield: " << tmr.getTimeElapsed() / 1000.0 << "s" << std::endl;

    // Create finest mipmap level
    // Where each pixel contains the maximum height value from the corresponding patch in the compact format
    PixelArray<Color4C> topMipmap;
    topMipmap.createPixelArrayEmpty( newWidth, newHeight );

    tmr.start();
    ThreadSplitter::threadizeDual(
        [&]( int xa, int xb, int ya, int yb ) {
        for ( int x = xa; x < xb; ++x ) {
            for ( int y = ya; y < yb; ++y ) {
                Color4C sample = compact.getPixel( x, y );
                topMipmap.getPixel( x, y ).red =
                    (GLubyte)mathCalcs::max( mathCalcs::max( (int)sample.green, (int)sample.red ),
                        mathCalcs::max( (int)sample.blue, (int)sample.alpha ) );
            }
        }
    },
        newWidth, newHeight, 8 );
    tmr.stop();
    std::cout << "Create mipmap toplevel: " << tmr.getTimeElapsed() / 1000.0 << "s" << std::endl;

    // Create finest mipmap level with POT dimensions (ex: 1024x1024)
    /*if ( !_maximumMipmaps.createSpecialTexture( newWidth, newHeight, GL_R8, GL_RED, topMipmap.getPixels() ) ) {
    return false;
    }*/
    if ( !_maximumMipmaps.loadTexture( topMipmap.getPixels(), newWidth, newHeight ) ) {
        std::cout << "Failed to create maximum mipmap texture." << std::endl;
        return false;
    }

    // Create remaining mipmap levels starting at level 1
    // Each consecutive level has half the size of the previous level (ex: L1=512x512,L2=256x256, etc)
    int level = 1;
    _maximumMipmaps.bindTexture();

    int maxLevel = (int)log2( newWidth );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0 );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, maxLevel );
    std::cout << "Max LOD: " << maxLevel << std::endl;

    tmr.start();
    while ( newHeight != 1 && newWidth != 1 ) {
        newWidth /= 2;
        newHeight /= 2;

        std::cout << "MipLevel(" << level << ") W:  " << newWidth << " " << " H:  " << newHeight << std::endl;

        // Create half sized buffer to hold data for the next level
        PixelArray<Color4C> mipmapData;
        mipmapData.createPixelArrayEmpty( newWidth, newHeight );

        // Multithreading can be applied to the following nested for loop
        // However, the last few levels have very small workloads and do not require multithreading
        for ( int x = 0; x < newWidth; ++x ) {
            for ( int y = 0; y < newHeight; ++y ) {
                int tx = x * 2;
                int ty = y * 2;

                mipmapData.getPixel( x, y ).red =
                    mathCalcs::max( mathCalcs::max( (int)topMipmap.getPixel( tx, ty ).red, (int)topMipmap.getPixel( tx + 1, ty ).red ),
                        mathCalcs::max( (int)topMipmap.getPixel( tx, ty + 1 ).red, (int)topMipmap.getPixel( tx + 1, ty + 1 ).red ) );
            }
        }

        // Create next level mipmap
        //glTexImage2D( GL_TEXTURE_2D, level, GL_R8, newWidth, newHeight, 0, GL_RED, GL_UNSIGNED_BYTE, mipmapData.getPixels() );
        glTexImage2D( GL_TEXTURE_2D, level, GL_RGBA, newWidth, newHeight, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, mipmapData.getPixels() );

        topMipmap = mipmapData;
        level++;
    }
    tmr.stop();
    std::cout << "Mipmap generation: " << tmr.getTimeElapsed() / 1000.0 << "s" << std::endl;
    _maximumMipmaps.unbindTexture();

    // Maximum mipmap structure will be accessed per index. Interpolation is not needed.
    _maximumMipmaps.setMinMagFilter( GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST );
    _maximumMipmaps.setWrapMode( GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE );

    // #level is incremented one last time by the end of the previous loop
    _lastLevel = level - 1;

    // Maximum value could aid in determining a better/smaller value for the heightmap Z dimensions, as a small optimization
    /*int max = topMipmap.getPixel(0,0);*/

    return true;
}

bool HeightfieldScalable16::loadCompactFormatA2( int tileX, int tileY, int orderTile ) {
    // Determine which tile in the sparse texture is to be used
    int texTile = findAvailableTile();
    int sparseX = texTile % 3;
    int sparseY = texTile / 3;
    
    std::string heightFile = getHeighTileFilename( tileX, tileY );
    std::string photoFile = getPhotoTileFilename( tileX, tileY );

    DebugTimer tmr;

    // Read heightfield tile
    tmr.start();
    PixelArray< Color4C > compactPixels;
    if ( !Texture::loadPixelsFromImage( heightFile, compactPixels ) ) {
        std::cout << "Tile loading error: Unable to read data from \"" << heightFile << "\"." << std::endl;
        getchar();
        return false;
    }
    tmr.stop();
    std::cout << "Read heightfield tile: " << tmr.getTimeElapsed() / 1000.0 << "s" << std::endl;

    int width = compactPixels.getWidth();
    int height = compactPixels.getHeight();

    if ( width != _hTileSize || height != _hTileSize ) {
        std::cout << "Tile loading error: Tile \"" << heightFile << " " << " size mismatch (" << width << " , " << height << ")" << std::endl;
        return false;
    }

    if ( getOpenGLError( "Random error" ) ) {
        std::cout << "Random error" << std::endl;
        return false;
    }

    // Upload heightfield tile to sparse texture
    tmr.start();
    _hfCompact.bindTexture();
    /*glTexPageCommitmentARB( GL_TEXTURE_2D, 0,
        _hTileSize * sparseX, _hTileSize * sparseY, 0,
        _hTileSize, _hTileSize, 1,
        GL_TRUE );*/
    glTexSubImage2D( GL_TEXTURE_2D, 0, _hTileSize * sparseX, _hTileSize * sparseY, _hTileSize, _hTileSize, GL_RGBA, GL_UNSIGNED_BYTE, compactPixels.getPixels() );
    _hfCompact.unbindTexture();
    tmr.stop();
    std::cout << "Stored HF tile: " << tmr.getTimeElapsed() / 1000.0 << "s" << std::endl;
    if ( getOpenGLError( "Error committing heightfield compact" ) ) {
        std::cout << "Error comitting heightfield compact." << std::endl;
        return false;
    }

    // Read in the photo texture
    tmr.start();
    PixelArray< Color4C > photoPixels;
    if ( !Texture::loadPixelsFromImage( photoFile, photoPixels ) ) {
        std::cout << "PhotoTexture could not be read: " << photoFile << std::endl;
        return false;
    }
    _photoTexture.bindTexture();
    /*glTexPageCommitmentARB( GL_TEXTURE_2D, 0,
    _hTileSize * sparseX, _hTileSize * sparseY, 0,
    _hTileSize, _hTileSize, 1,
    GL_TRUE );
    if ( getOpenGLError( "Error committing photo texture pages" ) ) {
    std::cout << "Error committing photo texture pages: " << std::endl;
    std::cout << "Offsets: " << _hTileSize * sparseX << " , " << _hTileSize * sparseY << std::endl;
    std::cout << "Sizes: " << _hTileSize << " , " << _hTileSize << std::endl;
    return false;
    }*/
    glTexSubImage2D( GL_TEXTURE_2D, 0, _hTileSize * sparseX, _hTileSize * sparseY, _hTileSize, _hTileSize, GL_RGBA, GL_UNSIGNED_BYTE, photoPixels.getPixels() );
    if ( getOpenGLError( "Error uploading photo texture data" ) ) {
        std::cout << "Error uploading photo texture data." << std::endl;
        return false;
    }
    _photoTexture.unbindTexture();
    photoPixels.deletePixelArray();
    tmr.stop();
    std::cout << "Photo texture: " << tmr.getTimeElapsed() / 1000.0 << "s" << std::endl;

    // Generate the mipmap levels
    // Create finest mipmap level
    // Where each pixel contains the maximum height value from the corresponding patch in the compact format
    PixelArray<Color4C> topMipmap;
    topMipmap.createPixelArrayEmpty( width, height );

    tmr.start();
    ThreadSplitter::threadizeDual(
        [&]( int xa, int xb, int ya, int yb ) {
        for ( int x = xa; x < xb; ++x ) {
            for ( int y = ya; y < yb; ++y ) {
                Color4C sample = compactPixels[ y * width + x ];
                topMipmap.getPixel( x, y ).red =
                    (GLubyte)mathCalcs::max( mathCalcs::max( (int)sample.green, (int)sample.red ),
                        mathCalcs::max( (int)sample.blue, (int)sample.alpha ) );
            }
        }
    },
        width, height, 8 );
    tmr.stop();
    std::cout << "Create mipmap toplevel: " << tmr.getTimeElapsed() / 1000.0 << "s" << std::endl;
    compactPixels.deletePixelArray();

    // Create remaining mipmap levels starting at level 1
    // Each consecutive level has half the size of the previous level (ex: L1=512x512,L2=256x256, etc)
    _maximumMipmaps.bindTexture();
    /*glTexPageCommitmentARB( GL_TEXTURE_2D, 0,
        _hTileSize * sparseX, _hTileSize * sparseY, 0,
        _hTileSize, _hTileSize, 1,
        GL_TRUE );
    if ( getOpenGLError( "Error committing mipmap top" ) ) {
        std::cout << "Error committing mipmap top" << std::endl;
        return false;
    }*/
    glTexSubImage2D( GL_TEXTURE_2D, 0, width * sparseX, height * sparseY, width, height, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, topMipmap.getPixels() );
    if ( getOpenGLError( "Error storing mipmap top" ) ) {
        std::cout << "Error storing mipmap top" << std::endl;
        return false;
    }

    int level = 1;
    tmr.start();
    while ( level <= _maxMipmapLevel ) {
        width /= 2;
        height /= 2;

        //std::cout << "MipLevel(" << level << ") W:  " << width << " " << " H:  " << height << std::endl;

        // Create half sized buffer to hold data for the next level
        PixelArray<Color4C> mipmapData;
        mipmapData.createPixelArrayEmpty( width, height );

        // Multithreading can be applied to the following nested for loop
        // However, the last few levels have very small workloads and do not require multithreading
        for ( int x = 0; x < width; ++x ) {
            for ( int y = 0; y < height; ++y ) {
                int tx = x * 2;
                int ty = y * 2;

                mipmapData.getPixel( x, y ).red =
                    mathCalcs::max( mathCalcs::max( (int)topMipmap.getPixel( tx, ty ).red, (int)topMipmap.getPixel( tx + 1, ty ).red ),
                        mathCalcs::max( (int)topMipmap.getPixel( tx, ty + 1 ).red, (int)topMipmap.getPixel( tx + 1, ty + 1 ).red ) );
            }
        }

        // Create next level mipmap
        //glTexImage2D( GL_TEXTURE_2D, level, GL_R8, newWidth, newHeight, 0, GL_RED, GL_UNSIGNED_BYTE, mipmapData.getPixels() );
        //glTexImage2D( GL_TEXTURE_2D, level, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, mipmapData.getPixels() );

        /*glTexPageCommitmentARB( GL_TEXTURE_2D, level,
            _hTileSize * sparseX, _hTileSize * sparseY, 0,
            _hTileSize, _hTileSize, 1,
            GL_TRUE );*/
        //std::cout << level << " : Off: " << width * sparseX << " , " << height * sparseY << " D: " << width << " , " << height << std::endl;
        glTexSubImage2D( GL_TEXTURE_2D, level, width * sparseX, height * sparseY, width, height, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, mipmapData.getPixels() );
        if ( getOpenGLError( "Error committing mipmap level: " + std::to_string( level )  ) ) {
            std::cout << "Error committing mipmap level: " << level << std::endl;
            return false;
        }

        topMipmap = mipmapData;
        level++;
    }
    tmr.stop();
    std::cout << "Mipmap generation: " << tmr.getTimeElapsed() / 1000.0 << "s" << std::endl;
    _maximumMipmaps.unbindTexture();

    /*ArrayWrapper< GLubyte > photoPixels;
    if ( !Texture::loadCompressedPixelsFromImage( photoFile, photoPixels ) ) {
        std::cout << "Photo pixels are BAD" << std::endl;
        return false;
    }

    _photoTexture.bindTexture();
    glTexPageCommitmentARB( GL_TEXTURE_2D, 0,
        _hTileSize * sparseX, _hTileSize * sparseY, 0,
        _hTileSize, _hTileSize, 1,
        GL_TRUE );
    if ( getOpenGLError( "Error committing photo texture pages" ) ) {
        std::cout << "Error committing photo texture pages: " << photoPixels.getSize() << std::endl;
        std::cout << "Offsets: " << _hTileSize * sparseX << " , " << _hTileSize * sparseY << std::endl;
        std::cout << "Sizes: " << _hTileSize << " , " << _hTileSize << std::endl;
        return false;
    }
    glCompressedTexSubImage2D( GL_TEXTURE_2D, 0, width * sparseX, height * sparseY, width, height,
        GL_COMPRESSED_RGB_S3TC_DXT1_EXT, photoPixels.getSize(), photoPixels.getPointer() );
    //glTexSubImage2D( GL_TEXTURE_2D, level, width * sparseX, height * sparseY, width, height, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, photoPixels.getPointer() );
    _photoTexture.unbindTexture();
    if ( getOpenGLError( "Error uploading photo texture data" ) ) {
        std::cout << "Error uploading photo texture data. Buffer size: " << photoPixels.getSize() << std::endl;
        return false;
    }*/

    // Update round-robin index
    _tileOrder[ orderTile ] = texTile;
    _tileStates[ tileX ][ tileY ] = orderTile;

    // Maximum value could aid in determining a better/smaller value for the heightmap Z dimensions, as a small optimization
    /*int max = topMipmap.getPixel(0,0);*/

    return true;
}

bool HeightfieldScalable16::loadPerPatchTile_16b( int tileX, int tileY, int orderTile ) {
    // Determine which tile in the sparse texture is to be used
    int texTile = findAvailableTile();
    int sparseX = texTile % 3;
    int sparseY = texTile / 3;

    std::string heightFile = getHeighTileFilename( tileX, tileY );
    std::string photoFile = getPhotoTileFilename( tileX, tileY );

    _tileCacheMap[ std::pair<int, int>( tileX, tileY ) ] = std::shared_ptr<TileCache16>( new TileCache16() );
    TileCache16* tCache = getTileCache( tileX, tileY );
    tCache->tileX = tileX;
    tCache->tileY = tileY;
    tCache->assignedIndex = -1;
    tCache->error = false;
    tCache->loaded = true;
    tCache->heightTile.push_back( PixelArray<Color4C_16b>() );

    DebugTimer tmr;

    // Read heightfield tile
    tmr.start();
    PixelArray< Color4C_16b > compactPixels;
    if ( !Texture::loadPixelsFromImage16b_RGBA( heightFile, compactPixels ) ) {
        std::cout << "Tile loading error: Unable to read data from \"" << heightFile << "\"." << std::endl;
        getchar();
        return false;
    }
    tmr.stop();
    std::cout << "Read heightfield tile: " << tmr.getTimeElapsed() / 1000.0 << "s" << std::endl;

    int width = compactPixels.getWidth();
    int height = compactPixels.getHeight();

    if ( width != _hTileSize || height != _hTileSize ) {
        std::cout << "Tile loading error: Tile \"" << heightFile << " " << " size mismatch (" << width << " , " << height << ")" << std::endl;
        return false;
    }

    tCache->heightTile[ 0 ].createPixelArrayEmpty( width, height );
    tCache->heightTile[ 0 ].overwritePixelArray( compactPixels, 0, 0 );

    if ( getOpenGLError( "Random error" ) ) {
        std::cout << "Random error" << std::endl;
        return false;
    }

    // Upload heightfield tile to sparse texture
    tmr.start();
    _hfCompact.bindTexture();
    /*glTexPageCommitmentARB( GL_TEXTURE_2D, 0,
    _hTileSize * sparseX, _hTileSize * sparseY, 0,
    _hTileSize, _hTileSize, 1,
    GL_TRUE );*/
    glTexSubImage2D( GL_TEXTURE_2D, 0, _hTileSize * sparseX, _hTileSize * sparseY, _hTileSize, _hTileSize, GL_RGBA, GL_UNSIGNED_SHORT, compactPixels.getPixels() );
    _hfCompact.unbindTexture();
    tmr.stop();
    std::cout << "Stored HF tile: " << tmr.getTimeElapsed() / 1000.0 << "s" << std::endl;
    if ( getOpenGLError( "Error committing heightfield compact" ) ) {
        std::cout << "Error comitting heightfield compact." << std::endl;
        return false;
    }

    // Read in the photo texture
    tmr.start();
    PixelArray< Color4C > photoPixels;
    if ( !Texture::loadPixelsFromImage( photoFile, photoPixels ) ) {
        std::cout << "PhotoTexture could not be read: " << photoFile << std::endl;
        return false;
    }
    _photoTexture.bindTexture();
    /*glTexPageCommitmentARB( GL_TEXTURE_2D, 0,
    _hTileSize * sparseX, _hTileSize * sparseY, 0,
    _hTileSize, _hTileSize, 1,
    GL_TRUE );
    if ( getOpenGLError( "Error committing photo texture pages" ) ) {
    std::cout << "Error committing photo texture pages: " << std::endl;
    std::cout << "Offsets: " << _hTileSize * sparseX << " , " << _hTileSize * sparseY << std::endl;
    std::cout << "Sizes: " << _hTileSize << " , " << _hTileSize << std::endl;
    return false;
    }*/
    glTexSubImage2D( GL_TEXTURE_2D, 0, _hTileSize * sparseX, _hTileSize * sparseY, _hTileSize, _hTileSize, GL_RGBA, GL_UNSIGNED_BYTE, photoPixels.getPixels() );
    if ( getOpenGLError( "Error uploading photo texture data" ) ) {
        std::cout << "Error uploading photo texture data." << std::endl;
        return false;
    }
    _photoTexture.unbindTexture();
    photoPixels.deletePixelArray();
    tmr.stop();
    std::cout << "Photo texture: " << tmr.getTimeElapsed() / 1000.0 << "s" << std::endl;

    // Generate the mipmap levels
    // Create finest mipmap level
    // Where each pixel contains the maximum height value from the corresponding patch in the compact format
    PixelArray<unsigned short> topMipmap;
    topMipmap.createPixelArrayEmpty( width, height );

    tmr.start();
    ThreadSplitter::threadizeDual(
        [&]( int xa, int xb, int ya, int yb ) {
        for ( int x = xa; x < xb; ++x ) {
            for ( int y = ya; y < yb; ++y ) {
                Color4C_16b sample = compactPixels[ y * width + x ];
                topMipmap.getPixel( x, y ) =
                    mathCalcs::max( mathCalcs::max( sample.green, sample.red ),
                        mathCalcs::max( sample.blue, sample.alpha ) );
                //compactPixels[ y * width + x ].reverseChannels();
            }
        }
    },
        width, height, 8 );
    tmr.stop();
    std::cout << "Create mipmap toplevel: " << tmr.getTimeElapsed() / 1000.0 << "s" << std::endl;
    //compactPixels.deletePixelArray();

    // Create remaining mipmap levels starting at level 1
    // Each consecutive level has half the size of the previous level (ex: L1=512x512,L2=256x256, etc)
    _maximumMipmaps.bindTexture();
    /*glTexPageCommitmentARB( GL_TEXTURE_2D, 0,
    _hTileSize * sparseX, _hTileSize * sparseY, 0,
    _hTileSize, _hTileSize, 1,
    GL_TRUE );
    if ( getOpenGLError( "Error committing mipmap top" ) ) {
    std::cout << "Error committing mipmap top" << std::endl;
    return false;
    }*/
    std::cout << "About to store top level" << std::endl;
    glTexSubImage2D( GL_TEXTURE_2D, 0, width * sparseX, height * sparseY, width, height, GL_RED, GL_UNSIGNED_SHORT, topMipmap.getPixels() );
    if ( getOpenGLError( "Error storing mipmap top" ) ) {
        std::cout << "Error storing mipmap top" << std::endl;
        return false;
    }

    std::cout << "Generate mipmaps" << std::endl;

    int level = 1;
    int lodWidth = width;
    int lodHeight = height;
    tmr.start();
    while ( level <= _maxMipmapLevel ) {
        lodWidth /= 2;
        lodHeight /= 2;

        //std::cout << "MipLevel(" << level << ") W:  " << width << " " << " H:  " << height << std::endl;

        // Create half sized buffer to hold data for the next level
        PixelArray<unsigned short> mipmapData;
        mipmapData.createPixelArrayEmpty( lodWidth, lodHeight );

        // Multithreading can be applied to the following nested for loop
        // However, the last few levels have very small workloads and do not require multithreading
        for ( int x = 0; x < lodWidth; ++x ) {
            for ( int y = 0; y < lodHeight; ++y ) {
                int tx = x * 2;
                int ty = y * 2;

                mipmapData.getPixel( x, y ) =
                    mathCalcs::max( mathCalcs::max( topMipmap.getPixel( tx, ty ), topMipmap.getPixel( tx + 1, ty ) ),
                        mathCalcs::max( topMipmap.getPixel( tx, ty + 1 ), topMipmap.getPixel( tx + 1, ty + 1 ) ) );
            }
        }

        // Create next level mipmap
        //glTexImage2D( GL_TEXTURE_2D, level, GL_R8, newWidth, newHeight, 0, GL_RED, GL_UNSIGNED_BYTE, mipmapData.getPixels() );
        //glTexImage2D( GL_TEXTURE_2D, level, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, mipmapData.getPixels() );

        /*glTexPageCommitmentARB( GL_TEXTURE_2D, level,
        _hTileSize * sparseX, _hTileSize * sparseY, 0,
        _hTileSize, _hTileSize, 1,
        GL_TRUE );*/
        //std::cout << level << " : Off: " << width * sparseX << " , " << height * sparseY << " D: " << width << " , " << height << std::endl;
        glTexSubImage2D( GL_TEXTURE_2D, level, lodWidth * sparseX, lodHeight * sparseY, lodWidth, lodHeight, GL_RED, GL_UNSIGNED_SHORT, mipmapData.getPixels() );
        if ( getOpenGLError( "Error committing mipmap level: " + std::to_string( level ) ) ) {
            std::cout << "Error committing mipmap level: " << level << std::endl;
            return false;
        }

        topMipmap = mipmapData;
        level++;
    }
    tmr.stop();
    std::cout << "Mipmap generation: " << tmr.getTimeElapsed() / 1000.0 << "s" << std::endl;
    _maximumMipmaps.unbindTexture();

    /*ArrayWrapper< GLubyte > photoPixels;
    if ( !Texture::loadCompressedPixelsFromImage( photoFile, photoPixels ) ) {
    std::cout << "Photo pixels are BAD" << std::endl;
    return false;
    }

    _photoTexture.bindTexture();
    glTexPageCommitmentARB( GL_TEXTURE_2D, 0,
    _hTileSize * sparseX, _hTileSize * sparseY, 0,
    _hTileSize, _hTileSize, 1,
    GL_TRUE );
    if ( getOpenGLError( "Error committing photo texture pages" ) ) {
    std::cout << "Error committing photo texture pages: " << photoPixels.getSize() << std::endl;
    std::cout << "Offsets: " << _hTileSize * sparseX << " , " << _hTileSize * sparseY << std::endl;
    std::cout << "Sizes: " << _hTileSize << " , " << _hTileSize << std::endl;
    return false;
    }
    glCompressedTexSubImage2D( GL_TEXTURE_2D, 0, width * sparseX, height * sparseY, width, height,
    GL_COMPRESSED_RGB_S3TC_DXT1_EXT, photoPixels.getSize(), photoPixels.getPointer() );
    //glTexSubImage2D( GL_TEXTURE_2D, level, width * sparseX, height * sparseY, width, height, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, photoPixels.getPointer() );
    _photoTexture.unbindTexture();
    if ( getOpenGLError( "Error uploading photo texture data" ) ) {
    std::cout << "Error uploading photo texture data. Buffer size: " << photoPixels.getSize() << std::endl;
    return false;
    }*/

    std::cout << "Generating normal map: ";
    tmr.start();
    int normW = _hTileSize + 0;
    int normH = _hTileSize + 0;
    PixelArray< Color3C > normals;
    normals.createPixelArray( normW, normH );

    ThreadSplitter::threadizeDual(
        [&]( int xa, int xb, int ya, int yb ) {
        for ( int x = xa; x < xb; ++x ) {
            for ( int y = ya; y < yb; ++y ) {
                Vector3D nUp = genNormal16( &compactPixels, x, y, x, y + 1 );
                Vector3D nDown = genNormal16( &compactPixels, x, y, x, y - 1 );
                Vector3D nLeft = genNormal16( &compactPixels, x, y, x - 1, y );
                Vector3D nRight = genNormal16( &compactPixels, x, y, x + 1, y );

                Vector3D norm = nUp + nDown + nLeft + nRight; norm.normalize();
                normals.getPixel( x, y ) = convertNormalToPixel( norm );
            }
        }
    },
        normH, normH, 8 );
    _normalMap.bindTexture();
    glTexSubImage2D( GL_TEXTURE_2D, 0, normW * sparseX, normH * sparseY, normW, normH, GL_RGB, GL_UNSIGNED_BYTE, normals.getPixels() );
    if ( getOpenGLError( "Error uploading normal map data" ) ) {
        std::cout << "Error uploading normal map data." << std::endl;
        return false;
    }
    _normalMap.unbindTexture();
    std::cout << tmr.getTimeElapsed() / 1000.0 << "s" << std::endl;


    // Generate heightfield mipmaps
    tmr.start();
    _hfCompact.bindTexture();
    level = 1;
    lodWidth = width;
    lodHeight = height;
    while ( lodHeight != 1 && lodWidth != 1 ) {
        lodWidth /= 2;
        lodHeight /= 2;

        //std::cout << "HF MipLevel(" << level << ") W:  " << lodWidth << " " << " H:  " << lodHeight << std::endl;

        // Create half sized buffer to hold data for the next level
        PixelArray<Color4C_16b> compactData;
        compactData.createPixelArrayEmpty( lodWidth, lodHeight );

        // Multithreading can be applied to the following nested for loop
        // However, the last few levels have very small workloads and do not require multithreading
        for ( int x = 0; x < lodWidth; ++x ) {
            for ( int y = 0; y < lodHeight; ++y ) {
                int tx = x * 2;
                int ty = y * 2;

                // If there's an equivalent to GL_UNSIGNED_INT_8_8_8_8 but for UShort, this should be used
                // as the order is much simpler to understand
                /*compactData.getPixel( x, y ).setColor(
                compact.getPixel( tx, ty ).red,
                compact.getPixel( tx + 1, ty ).green,
                compact.getPixel( tx, ty + 1 ).blue,
                compact.getPixel( tx + 1, ty + 1 ).alpha );*/
                // GL_RGBA,GL_UNSIGNED_SHORT has the byte order reversed
                compactData.getPixel( x, y ).setColor(
                    compactPixels.getPixel( tx + 1, ty + 1 ).red,
                    compactPixels.getPixel( tx, ty + 1 ).green,
                    compactPixels.getPixel( tx + 1, ty ).blue,
                    compactPixels.getPixel( tx, ty ).alpha );
            }
        }

        // Create next level mipmap
        glTexSubImage2D( GL_TEXTURE_2D, level, lodWidth * sparseX, lodHeight * sparseY, lodWidth, lodHeight, GL_RGBA, GL_UNSIGNED_SHORT, compactData.getPixels() );

        compactPixels = compactData;
        level++;
    }
    _hfCompact.unbindTexture();
    tmr.stop();
    std::cout << tmr.getTimeElapsed() / 1000.0 << "s" << std::endl;

    // Update round-robin index
    _tileOrder[ orderTile ] = texTile;
    _tileStates[ tileX ][ tileY ] = orderTile;

    // Maximum value could aid in determining a better/smaller value for the heightmap Z dimensions, as a small optimization
    /*int max = topMipmap.getPixel(0,0);*/

    return true;
}

bool HeightfieldScalable16::uploadTile( int tileX, int tileY ) {
    TileCache16* tCache = getTileCache( tileX, tileY );
    if ( tCache == nullptr ) {
        return false;
    }

    // Determine which tile in the sparse texture is to be used
    int orderTile = ( tileX - _llTileX ) + ( tileY - _llTileY ) * 3;
    if ( orderTile < 0 || orderTile > 8 ) { std::cout << "Attempting to upload tile outside the active region" << std::endl; return false; }
    int texTile = findAvailableTile();
    int sparseX = texTile % 3;
    int sparseY = texTile / 3;

    std::string heightFile = getHeighTileFilename( tileX, tileY );
    std::string photoFile = getPhotoTileFilename( tileX, tileY );

    DebugTimer tmr;

    // Upload base heightfield tile
    tmr.start();
    for ( int i = 0; i < tCache->heightTile.size(); ++i ) {
        _hfCompact.bindTexture();
        //glTexSubImage2D( GL_TEXTURE_2D, 0, _hTileSize * sparseX, _hTileSize * sparseY, _hTileSize, _hTileSize, GL_RGBA, GL_UNSIGNED_SHORT, tCache->heightTile[ 0 ].getPixels() );
        glTexSubImage2D( GL_TEXTURE_2D, i, tCache->heightTile[ i ].getWidth() * sparseX, tCache->heightTile[ i ].getHeight() * sparseY,
            tCache->heightTile[ i ].getWidth(), tCache->heightTile[ i ].getHeight(), GL_RGBA, GL_UNSIGNED_SHORT, tCache->heightTile[i].getPixels() );
    }
    _hfCompact.unbindTexture();
    tmr.stop();
    std::cout << "Stored HF tile: " << tmr.getTimeElapsed() / 1000.0 << "s" << std::endl;
    if ( getOpenGLError( "Error committing heightfield compact" ) ) {
        std::cout << "Error comitting heightfield compact." << std::endl;
        return false;
    }

    // Read in the photo texture
    tmr.start();
    _photoTexture.bindTexture();
    glTexSubImage2D( GL_TEXTURE_2D, 0, _hTileSize * sparseX, _hTileSize * sparseY, _hTileSize, _hTileSize, GL_RGBA, GL_UNSIGNED_BYTE, tCache->photoTile.getPixels() );
    if ( getOpenGLError( "Error uploading photo texture data" ) ) {
        std::cout << "Error uploading photo texture data." << std::endl;
        return false;
    }
    _photoTexture.unbindTexture();
    tmr.stop();
    std::cout << "Photo texture: " << tmr.getTimeElapsed() / 1000.0 << "s" << std::endl;

    // Read in mipmaps
    for ( int i = 0; i < tCache->mipmaps.size(); ++i ) {
        _maximumMipmaps.bindTexture();
        glTexSubImage2D( GL_TEXTURE_2D, i, tCache->mipmaps[i].getWidth() * sparseX, tCache->mipmaps[ i ].getHeight() * sparseY,
            tCache->mipmaps[ i ].getWidth(), tCache->mipmaps[ i ].getHeight(), GL_RED, GL_UNSIGNED_SHORT, tCache->mipmaps[i].getPixels() );
    }

    // Generate the mipmap levels
    // Create finest mipmap level
    // Where each pixel contains the maximum height value from the corresponding patch in the compact format
    /*PixelArray<Color4C> topMipmap;
    topMipmap.createPixelArrayEmpty( _hTileSize, _hTileSize );

    tmr.start();
    ThreadSplitter::threadizeDual(
        [&]( int xa, int xb, int ya, int yb ) {
        for ( int x = xa; x < xb; ++x ) {
            for ( int y = ya; y < yb; ++y ) {
                Color4C sample = tCache->heightTile[ y * _hTileSize + x ];
                topMipmap.getPixel( x, y ).red =
                    (GLubyte)mathCalcs::max( mathCalcs::max( (int)sample.green, (int)sample.red ),
                        mathCalcs::max( (int)sample.blue, (int)sample.alpha ) );
            }
        }
    },
        _hTileSize, _hTileSize, 8 );
    tmr.stop();
    std::cout << "Create mipmap toplevel: " << tmr.getTimeElapsed() / 1000.0 << "s" << std::endl;

    // Create remaining mipmap levels starting at level 1
    // Each consecutive level has half the size of the previous level (ex: L1=512x512,L2=256x256, etc)
    _maximumMipmaps.bindTexture();
    glTexSubImage2D( GL_TEXTURE_2D, 0, _hTileSize * sparseX, _hTileSize * sparseY, _hTileSize, _hTileSize, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, topMipmap.getPixels() );
    if ( getOpenGLError( "Error storing mipmap top" ) ) {
        std::cout << "Error storing mipmap top" << std::endl;
        return false;
    }

    int width = _hTileSize, height = _hTileSize;
    int level = 1;
    tmr.start();
    while ( level <= _maxMipmapLevel ) {
        width /= 2;
        height /= 2;

        //std::cout << "MipLevel(" << level << ") W:  " << width << " " << " H:  " << height << std::endl;

        // Create half sized buffer to hold data for the next level
        PixelArray<Color4C> mipmapData;
        mipmapData.createPixelArrayEmpty( width, height );

        // Multithreading can be applied to the following nested for loop
        // However, the last few levels have very small workloads and do not require multithreading
        for ( int x = 0; x < width; ++x ) {
            for ( int y = 0; y < height; ++y ) {
                int tx = x * 2;
                int ty = y * 2;

                mipmapData.getPixel( x, y ).red =
                    mathCalcs::max( mathCalcs::max( (int)topMipmap.getPixel( tx, ty ).red, (int)topMipmap.getPixel( tx + 1, ty ).red ),
                        mathCalcs::max( (int)topMipmap.getPixel( tx, ty + 1 ).red, (int)topMipmap.getPixel( tx + 1, ty + 1 ).red ) );
            }
        }

        // Create next level mipmap
        //std::cout << level << " : Off: " << width * sparseX << " , " << height * sparseY << " D: " << width << " , " << height << std::endl;
        glTexSubImage2D( GL_TEXTURE_2D, level, width * sparseX, height * sparseY, width, height, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, mipmapData.getPixels() );
        if ( getOpenGLError( "Error committing mipmap level: " + std::to_string( level ) ) ) {
            std::cout << "Error committing mipmap level: " << level << std::endl;
            return false;
        }

        topMipmap = mipmapData;
        level++;
    }
    tmr.stop();*/
    std::cout << "Maximum Mipmap generation: " << tmr.getTimeElapsed() / 1000.0 << "s" << std::endl;
    _maximumMipmaps.unbindTexture();

    // Read in the normal map
    _normalMap.bindTexture();
    glTexSubImage2D( GL_TEXTURE_2D, 0, tCache->normalTile.getWidth() * sparseX, tCache->normalTile.getHeight() * sparseY,
        tCache->normalTile.getWidth(), tCache->normalTile.getHeight(), GL_RGB, GL_UNSIGNED_BYTE, tCache->normalTile.getPixels() );
    if ( getOpenGLError( "Error uploading normal map data" ) ) {
        std::cout << "Error uploading normal map data." << std::endl;
        return false;
    }
    _normalMap.unbindTexture();

    // Update round-robin index
    _tileOrder[ orderTile ] = texTile;
    _tileStates[ tileX ][ tileY ] = orderTile;


    return true;
}

bool HeightfieldScalable16::loadPhotoTexture( const std::string &file ) {
    return _photoTexture.loadTexture( file, GL_COMPRESSED_RGBA_S3TC_DXT3_EXT, GL_RGBA, GL_UNSIGNED_BYTE );
}

TileCache16* HeightfieldScalable16::getTileCache( int tileX, int tileY ) {
    auto it = _tileCacheMap.find( std::make_pair( tileX, tileY ) );
    if ( it != _tileCacheMap.end() ) {
        return it->second.get();
    }
    return nullptr;
}
void HeightfieldScalable16::prepareToLoadTile( int tileX, int tileY ) {
    _tilesToLoad.push_back( std::pair< int, int>( tileX, tileY ) );
}
bool HeightfieldScalable16::preLoadTile( int tileX, int tileY ) {
    // Determine which tile in the sparse texture is to be used
    if ( isTilePreLoaded( tileX, tileY ) ) {
        return true;
    }

    std::cout << "Loading: " << tileX << " " << tileY << std::endl;
    
    _tileCacheMap[ std::pair<int, int>( tileX, tileY ) ] = std::shared_ptr<TileCache16>( new TileCache16() );
    TileCache16* tCache = getTileCache( tileX, tileY );
    tCache->tileX = tileX;
    tCache->tileY = tileY;
    tCache->assignedIndex = -1;
    tCache->error = false;
    tCache->loaded = false;

    std::string heightFile = getHeighTileFilename( tileX, tileY );
    std::string photoFile = getPhotoTileFilename( tileX, tileY );

    DebugTimer tmr;

    // Read heightfield tile
    tmr.start();
    tCache->heightTile.push_back( PixelArray<Color4C_16b>() );
    if ( !Texture::loadPixelsFromImage16b_RGBA( heightFile, tCache->heightTile[0] ) ) {
        //std::cout << "Tile loading error: Unable to read data from \"" << heightFile << "\"." << std::endl;
        tCache->error = true;
        return false;
    }
    tmr.stop();
    //std::cout << "Read heightfield tile: " << tmr.getTimeElapsed() / 1000.0 << "s" << std::endl;

    int width = tCache->heightTile[0].getWidth();
    int height = tCache->heightTile[0].getHeight();

    if ( width != _hTileSize || height != _hTileSize ) {
        //std::cout << "Tile loading error: Tile \"" << heightFile << " " << " size mismatch (" << width << " , " << height << ")" << std::endl;
        tCache->error = true;
        return false;
    }
    // Generate remaining heightmap levels
    for ( int i = 1; i <= _maxMipmapLevel; ++i ) {
        tCache->heightTile.push_back( PixelArray<Color4C_16b>() );
        PixelArray<Color4C_16b>& prevLevel = tCache->heightTile[ i - 1 ];
        PixelArray<Color4C_16b>& nextLevel = tCache->heightTile[ i ];
        int newW = prevLevel.getWidth() / 2;
        int newH = prevLevel.getHeight() / 2;
        nextLevel.createPixelArrayEmpty( newW, newH );

        for ( int x = 0; x < newW; ++x ) {
            for ( int y = 0; y < newH; ++y ) {
                int tx = x * 2;
                int ty = y * 2;

                nextLevel.getPixel( x, y ).setColor(
                    prevLevel.getPixel( tx + 1, ty + 1 ).red,
                    prevLevel.getPixel( tx, ty + 1 ).green,
                    prevLevel.getPixel( tx + 1, ty ).blue,
                    prevLevel.getPixel( tx, ty ).alpha );
            }
        }

    }

    // Read in the photo texture
    tmr.start();
    if ( !Texture::loadPixelsFromImage( photoFile, tCache->photoTile ) ) {
        //std::cout << "PhotoTexture could not be read: " << photoFile << std::endl;
        tCache->error = true;
        return false;
    }
    tmr.stop();
    //std::cout << "Read photo texture: " << tmr.getTimeElapsed() / 1000.0 << "s" << std::endl;

    // Preload maximum mipmaps
    // Generate top level mipmap
    tCache->mipmaps.push_back( PixelArray<unsigned short>() );
    PixelArray<unsigned short>& topMip = tCache->mipmaps[ 0 ];
    topMip.createPixelArrayEmpty( _hTileSize, _hTileSize );
    tmr.start();
    ThreadSplitter::threadizeDual(
        [&]( int xa, int xb, int ya, int yb ) {
        for ( int x = xa; x < xb; ++x ) {
            for ( int y = ya; y < yb; ++y ) {
                Color4C_16b sample = tCache->heightTile[0][ y * _hTileSize + x ];
                topMip.getPixel( x, y ) =
                    mathCalcs::max( mathCalcs::max( sample.green, sample.red ),
                        mathCalcs::max( sample.blue, sample.alpha ) );
            }
        }
    },
        _hTileSize, _hTileSize, 8 );
    // Generate remaining mipmap levels
    for ( int i = 1; i <= _maxMipmapLevel; ++i ) {
        tCache->mipmaps.push_back( PixelArray<unsigned short>() );
        PixelArray<unsigned short>& prevLevel = tCache->mipmaps[ i-1 ];
        PixelArray<unsigned short>& nextLevel = tCache->mipmaps[ i ];
        int newW = prevLevel.getWidth() / 2;
        int newH = prevLevel.getHeight() / 2;
        nextLevel.createPixelArrayEmpty( newW, newH );

        for ( int x = 0; x < newW; ++x ) {
            for ( int y = 0; y < newH; ++y ) {
                int tx = x * 2;
                int ty = y * 2;

                nextLevel.getPixel( x, y ) =
                    mathCalcs::max( mathCalcs::max( prevLevel.getPixel( tx, ty ), prevLevel.getPixel( tx + 1, ty ) ),
                        mathCalcs::max( prevLevel.getPixel( tx, ty + 1 ), prevLevel.getPixel( tx + 1, ty + 1 ) ) );
            }
        }

    }

    // Generate normals
    int normW = _hTileSize + 0;
    int normH = _hTileSize + 0;
    tCache->normalTile.createPixelArrayEmpty( normW, normH );

    ThreadSplitter::threadizeDual(
        [&]( int xa, int xb, int ya, int yb ) {
        for ( int x = xa; x < xb; ++x ) {
            for ( int y = ya; y < yb; ++y ) {
                Vector3D nUp = genNormal16( &tCache->heightTile[0], x, y, x, y + 1 );
                Vector3D nDown = genNormal16( &tCache->heightTile[ 0 ], x, y, x, y - 1 );
                Vector3D nLeft = genNormal16( &tCache->heightTile[ 0 ], x, y, x - 1, y );
                Vector3D nRight = genNormal16( &tCache->heightTile[ 0 ], x, y, x + 1, y );

                Vector3D norm = nUp + nDown + nLeft + nRight; norm.normalize();
                tCache->normalTile.getPixel( x, y ) = convertNormalToPixel( norm );
            }
        }
    },
        normW, normH, 8 );
    std::cout << tmr.getTimeElapsed() / 1000.0 << "s" << std::endl;

    tCache->loaded = true;

    return true;
}
bool HeightfieldScalable16::isTilePreLoaded( int tileX, int tileY ) {
    TileCache16* tCache = getTileCache( tileX, tileY );
    return ( tCache != nullptr ) && ( tCache->loaded );
}
/*bool HeightfieldScalable16::loadDefaultPhotoTexture() {
    PixelArray< Color4C > defaultPtex;
    defaultPtex.createPixelArrayEmpty( 16, 16 );
    defaultPtex.fill( Color4C( 255, 255, 255, 255 ) );
    return _photoTexture.loadTexture( defaultPtex.getPixels(), 16, 16 );
}*/

/*Texture *HeightfieldScalable16::getHeightmapTexture() {
    return &_hftex;
}*/

bool HeightfieldScalable16::unloadTile( int x, int y ) {
    int tileIndex = _tileStates[ x ][ y ];
    if ( tileIndex == -1 ) { return true; }

    // If the actual tile is still being loaded, an invalid old tile may be rendered until the new tile is ready!
    // Actually erase the data instead of just marking it unused

    _tileStates[ x ][ y ] = -1;
    _availableTiles.push_back( _tileOrder[ tileIndex ] );
    _tileOrder[ tileIndex ] = -1;

    return true;
}
void HeightfieldScalable16::deleteTile( int tileX, int tileY ) {
    auto it = _tileCacheMap.find( std::pair<int, int>( tileX, tileY ) );
    if ( it != _tileCacheMap.end() ) {
        _tileCacheMap.erase( it );
    }
}
bool HeightfieldScalable16::loadTile( int x, int y, int texTile ) {
    // Check if next tile is suitable
    if ( !loadPerPatchTile_16b( x, y, texTile ) ) {
        std::cout << "Failed to load tile: " << x << " , " << y << std::endl;
        return false;
    }
    return true;
}
void HeightfieldScalable16::reloadRegion( int x, int y ) {
    for ( int x = 0; x < 3; ++x ) {
        for ( int y = 0; y < 3; ++y ) {
            unloadTile( _llTileX + x, _llTileY + y );
        }
    }
    _llTileX = x; _llTileY = y;
    for ( int x = 0; x < 3; ++x ) {
        for ( int y = 0; y < 3; ++y ) {
            loadTile( _llTileX + x, _llTileY + y, (x+1) + (y+1)*3 );
        }
    }
}
void HeightfieldScalable16::recenterRegion( int nx, int ny ) {
    // Region data
    for ( int x = 0; x < 3; ++x ) {
        for ( int y = 0; y < 3; ++y ) {
            if ( abs( ( _llTileX + x ) - (nx+1) ) > 1 || abs( ( _llTileY + y ) - (ny+1) ) > 1 ) {
                std::cout << "Unload: " << _llTileX + x << "  ,  " << _llTileY + y << std::endl;
                unloadTile( _llTileX + x, _llTileY + y );
                deleteTile( _llTileX + x, _llTileY + y );
            }
        }
    }


    /*int px = _centerTileX;
    int py = _centerTileY;

    int minpx = mathCalcs::max( px - 1, 0 );
    int maxpx = mathCalcs::min( px + 1, 3 );
    int minx = mathCalcs::max( nx - 1, 0 );
    int maxx = mathCalcs::min( nx + 1, 3 );

    int minpy = mathCalcs::max( py - 1, 0 );
    int maxpy = mathCalcs::min( py + 1, 3 );
    int miny = mathCalcs::max( ny - 1, 0 );
    int maxy = mathCalcs::min( ny + 1, 3 );

    // Unload only tiles that are outside of the new region
    for ( int x = minpx; x <= maxpx; ++x ) {
        for ( int y = minpy; y <= maxpy; ++y ) {
            if ( !( ( minx <= x && x <= maxx ) && ( miny <= y && y <= maxy ) ) ) {
                if ( _tileStates[ x ][ y ] > 0 ) {
                    unloadTile( x, y );
                    std::cout << "Unloading: " << x << " " << y << std::endl;
                }
            }
        }
    }*/

    // Remove active region
    for ( int x = 0; x < 3; ++x ) {
        for ( int y = 0; y < 3; ++y ) {
            _tileStates[ _llTileX + x ][ _llTileY + y ] = -1;
        }
    }
    // Set new active region
    for ( int x = 0; x < 3; ++x ) {
        for ( int y = 0; y < 3; ++y ) {
            _tileStates[ nx + x ][ ny + y ] = x + y*3;
        }
    }

    // Shift previous active region into new active region
    int dx = nx - _llTileX, dy = ny - _llTileY;
    int startx, starty, endx, endy, stepx, stepy;
    if ( dx < 0 ) { startx = 2; endx = -1; stepx = -1; }
    else { startx = 0; endx = 3; stepx = 1; }
    if ( dy < 0 ) { starty = 2; endy = -1; stepy = -1; }
    else { starty = 0; endy = 3; stepy = 1; }

    for ( int x = startx; x != endx; x += stepx ) {
        for ( int y = starty; y != endy; y += stepy ) {
            int shiftx = x + dx, shifty = y + dy;
            // If still in active area
            if ( ( 0 <= shiftx && shiftx <= 2 ) && ( 0 <= shifty && shifty <= 2 ) ) {
                _tileOrder[ x + y * 3 ] = _tileOrder[ shiftx + shifty * 3 ];
            }
            else {
                _tileOrder[ x + y * 3 ] = -1;
            }
        }
    }

    _llTileX = nx;
    _llTileY = ny;

    // Determine which tiles need loading
    for ( int x = 0; x < 3; ++x ) {
        for ( int y = 0; y < 3; ++y ) {
            if ( _tileOrder[ _tileStates[ nx + x ][ ny + y ] ] == -1 ) {
                std::cout << "Load tile: " << nx + x << " , " << ny + y << std::endl;
                //loadTile( nx + x, ny + y, _tileStates[ nx + x ][ ny + y ] );
                prepareToLoadTile( nx + x, ny + y );
                _loadingTiles = true;
            }
        }
    }

    _loadSignal.notify_all();
    /*int px = _centerTileX;
    int py = _centerTileY;

    int minpx = mathCalcs::max( px - 1, 0 );
    int maxpx = mathCalcs::min( px + 1, 3 );
    int minx = mathCalcs::max( nx - 1, 0 );
    int maxx = mathCalcs::min( nx + 1, 3 );

    int minpy = mathCalcs::max( py - 1, 0 );
    int maxpy = mathCalcs::min( py + 1, 3 );
    int miny = mathCalcs::max( ny - 1, 0 );
    int maxy = mathCalcs::min( ny + 1, 3 );

    /*for ( int x = 0; x < 3; ++x ) {
        for ( int y = 0; y < 3; ++y ) {
        
        }
    }*/

    // Unload only tiles that are outside of the new region
    //std::cout << "pX: " << minpx << " " << maxpx << "      pY: " << minpy << " " << maxpy << std::endl;
    /*for ( int x = minpx; x <= maxpx; ++x ) {
        for ( int y = minpy; y <= maxpy; ++y ) {
            if ( !( ( minx <= x && x <= maxx ) && ( miny <= y && y <= maxy ) ) ) {
                if ( _tileStates[ x ][ y ] > 0 ) {
                    unloadTile( x, y );
                    _tileStates[ x ][ y ] = -1;
                }
            }
        }
    }*/

    // Load only tiles that are outside of the old region
    //std::cout << "X: " << minx << " " << maxx << "      Y: " << miny << " " << maxy << std::endl;
    /*for ( int x = minx; x <= maxx; ++x ) {
        for ( int y = miny; y <= maxy; ++y ) {
            if ( !( ( minpx <= x && x <= maxpx ) && ( minpy <= y && y <= maxpy ) ) ) {
                if ( _tileStates[ x ][ y ] == -1 ) {
                    loadTile( x, y );
                    _tileStates[ x ][ y ] = true;
                }
            }
        }
    }*/
}

void HeightfieldScalable16::update( float dtime ) {
    Vector2D realTileDimensions = getFinalTileDimensions();

    // if LOADING TILES
    if ( _loadingTiles ) {
        // Upload ready tiles to GPU
        bool allLoaded = true;
        for ( int i = 0; i < 9; ++i ) {
            if ( _tileOrder[ i ] == -1 ) {
                int x =  _llTileX + i % 3;
                int y =  _llTileY + i / 3;
                //std::cout << "Try load: " << x << " " << y << std::endl;
                if ( isTilePreLoaded( x, y ) ) {
                    if ( !uploadTile( x, y ) ) {
                        std::cout << "Failed to load" << std::endl;
                    }
                    break;
                }
                allLoaded = false;
            }
        }
        // if tile loading is done
        if ( allLoaded ) {
            _loadingTiles = false;
            std::cout << "Loaded all tiles" << std::endl;
        }
    }
    // if NOT LOADING TILES
    else {
        // Too close to the left border
        bool change = false;
        int newX = _llTileX, newY = _llTileY;
        if ( ( userPosition.x - _llTileX * realTileDimensions.x ) < _farz ) {// Move to the left
                    if ( _llTileX > 0 ) {
                        //shiftRegionX( false );
                        change = true;
                        newX = _llTileX - 1;
                    }
        }
        else if ( ( ( _llTileX + _regionSize ) * realTileDimensions.x - userPosition.x ) < _farz ) { // Move to the right
                    if ( _llTileX < _hNumTilesX-_regionSize ) {
                        //shiftRegionX( true );
                        change = true;
                        newX = _llTileX + 1;
                    }
        }
        if ( ( userPosition.y - _llTileY * realTileDimensions.y ) < _farz ) {// Move to the lower part
                    if ( _llTileY > 0 ) {
                        change = true;
                        newY = _llTileY - 1;
                    }
        }
        else if ( ( ( _llTileY + _regionSize ) * realTileDimensions.y - userPosition.y ) < _farz ) { // Move to the upper part
                    if ( _llTileY < _hNumTilesY - _regionSize ) {
                        change = true;
                        newY = _llTileY + 1;
                    }
        }
        if ( change ) {
            std::cout << "Updating active region. ";
            recenterRegion( newX, newY );
            boundsLL.setVector( _llTileX * _tileDimensions.x, _llTileY *_tileDimensions.y );
            std::cout << "[Complete]" << std::endl;;
        }
        //boundsDIM = boundsDIM; // Bounds do not change
    }

    // Check collision

    // If user is inside any tile we can do collision checking
    if ( ( 0 < userPosition.x  && userPosition.x < getPatchSize()*getHeightmapWidth() ) && ( 0 < userPosition.y  && userPosition.y < getPatchSize()*getHeightmapHeight() ) ) {
        int uTileX = userPosition.x / realTileDimensions.x;
        int uTileY = userPosition.y / realTileDimensions.y;
    //if ( ( 0 <= uTileX && uTileX < _hNumTilesX ) && ( 0 <= uTileY ||  uTileY < _hNumTilesY ) ) {
        if ( isTilePreLoaded( uTileX, uTileY ) ) {
            auto currentTile = getTileCache( uTileX, uTileY );

            //std::cout << uTileX << " " << uTileY << std::endl;

            float uX = ( userPosition.x - ( uTileX * realTileDimensions.x ) ) / getPatchSize();
            float uY = ( userPosition.y - ( uTileY * realTileDimensions.y ) ) / getPatchSize();
            Color4C_16b& patch = currentTile->heightTile[ 0 ].getPixel( (int)uX, (int)uY );

            float interX = uX - (int)( uX );
            float interY = uY - (int)( uY );

            //std::cout << uX << " " << uY << std::endl;
            //std::cout << interX << " " << interY << std::endl;

            float height = (float)mathCalcs::blerp( patch.alpha, patch.green, patch.blue, patch.red, interX, interY ) / (float)( 65535.0f );

            if ( ( userPosition.z - 2.0f ) < height * _mapHeight ) {
                userPosition.z = height *_mapHeight + 2.01f;
            }
        }
    }
}


Vector3D HeightfieldScalable16::genNormal16( PixelArray<Color4C_16b>* hmap, int ix, int iy, int tx, int ty ) {
    int hWidth = hmap->getWidth();
    int hHeight = hmap->getHeight();

    GLushort iHeight = 0;
    if ( validInterval( ix, 0, hmap->getWidth() - 1 ) && validInterval( iy, 0, hmap->getHeight() - 1 ) ) {
        iHeight = hmap->getPixel( ix, iy ).red;
    }
    else if ( validInterval( ix - 1, 0, hmap->getWidth() - 1 ) && validInterval( iy - 1, 0, hmap->getHeight() - 1 ) ) {
        iHeight = hmap->getPixel( ix - 1, iy - 1 ).alpha;
    }

    // 0 = up, 1 = down, 2 = right, 3 = left
    int dir = ( ty != iy ? ( ty > iy ? 0 : 1 ) : ( tx > ix ? 2 : 3 ) );

    Vector3D normal{ 0.0f, 0.0f, 1.0f };
    // Up
    if ( dir == 0 ) {
        if ( validInterval( tx, 0, hmap->getWidth() - 1 ) && validInterval( ty, 0, hmap->getHeight() - 1 ) ) {
            GLushort tHeight = hmap->getPixel( tx, ty ).blue;
            normal.setVector( 0, 1 * _patchSize, ( tHeight - iHeight ) / 65536.0f * _mapHeight );
            normal = rotate90_Xccw( normal );
        }
        else if ( validInterval( tx - 1, 0, hmap->getWidth() - 1 ) && validInterval( ty, 0, hmap->getHeight() - 1 ) ) {
            GLushort tHeight = hmap->getPixel( tx - 1, ty ).alpha;
            normal.setVector( 0, 1 * _patchSize, ( tHeight - iHeight ) / 65536.0f * _mapHeight );
            normal = rotate90_Xccw( normal );
        }
    }
    // Down
    else if ( dir == 1 ) {
        if ( validInterval( tx, 0, hmap->getWidth() - 1 ) && validInterval( ty - 1, 0, hmap->getHeight() - 1 ) ) {
            GLushort tHeight = hmap->getPixel( tx, ty - 1 ).red;
            normal.setVector( 0, -1 * _patchSize, ( tHeight - iHeight ) / 65536.0f * _mapHeight );
            normal = rotate90_Xcw( normal );
        }
        else if ( validInterval( tx - 1, 0, hmap->getWidth() - 1 ) && validInterval( ty - 1, 0, hmap->getHeight() - 1 ) ) {
            GLushort tHeight = hmap->getPixel( tx - 1, ty - 1 ).green;
            normal.setVector( 0, -1 * _patchSize, ( tHeight - iHeight ) / 65536.0f * _mapHeight );
            normal = rotate90_Xcw( normal );
        }
    }
    // Right
    else if ( dir == 2 ) {
        if ( validInterval( tx, 0, hmap->getWidth() - 1 ) && validInterval( ty, 0, hmap->getHeight() - 1 ) ) {
            GLushort tHeight = hmap->getPixel( tx, ty ).green;
            normal.setVector( 1 * _patchSize, 0, ( tHeight - iHeight ) / 65536.0f * _mapHeight );
            normal = rotate90_Yccw( normal );
        }
        else if ( validInterval( tx, 0, hmap->getWidth() - 1 ) && validInterval( ty - 1, 0, hmap->getHeight() - 1 ) ) {
            GLushort tHeight = hmap->getPixel( tx, ty - 1 ).alpha;
            normal.setVector( 1 * _patchSize, 0, ( tHeight - iHeight ) / 65536.0f * _mapHeight );
            normal = rotate90_Yccw( normal );
        }
    }
    // Left
    else if ( dir == 3 ) {
        if ( validInterval( tx - 1, 0, hmap->getWidth() - 1 ) && validInterval( ty, 0, hmap->getHeight() - 1 ) ) {
            GLushort tHeight = hmap->getPixel( tx - 1, ty ).red;
            normal.setVector( -1 * _patchSize, 0, ( tHeight - iHeight ) / 65536.0f * _mapHeight );
            normal = rotate90_Ycw( normal );
        }
        else if ( validInterval( tx - 1, 0, hmap->getWidth() - 1 ) && validInterval( ty - 1, 0, hmap->getHeight() - 1 ) ) {
            GLushort tHeight = hmap->getPixel( tx - 1, ty - 1 ).blue;
            normal.setVector( -1 * _patchSize, 0, ( tHeight - iHeight ) / 65536.0f * _mapHeight );
            normal = rotate90_Ycw( normal );
        }
    }
    normal.normalize();

    return normal;
}

int HeightfieldScalable16::findAvailableTile() {
    if ( _availableTiles.empty() ) {
        return -1;
    }
    int tile = _availableTiles.back();
    _availableTiles.pop_back();
    return tile;
}

void HeightfieldScalable16::printActiveRegion() {
    for ( int y = 0; y < 4; ++y ) {
        for ( int x = 0; x < 4; ++x ) {
            std::cout << _tileStates[ x ][ 3 - y ] << "\t";
        }
        std::cout << std::endl;
    }
    for ( int i = 0; i < 9; ++i ) {
        std::cout << _tileOrder[ i ] << " ";
    }
    std::cout << std::endl;
    std::cout << "LL: " << boundsLL.x << " " << boundsLL.y << std::endl;
    std::cout << "BD: " << boundsDIM.x << " " << boundsDIM.y << std::endl;
    std::cout << "LT: " << getLowerLeftX() << " " << getLowerLeftY() << std::endl;
}










void HeightfieldScalable16::spawnWorkerThread() {
    // if bpc == 8
    std::thread wkThread{
        [&] {
            std::mutex mtx;
            std::unique_lock < std::mutex > loadSig( mtx );

            while ( !_endThread ) {
                if ( _tilesToLoad.empty() ) {
                    _loadSignal.wait( loadSig );
                }
                std::pair<int, int> tIndices( -1, -1 );
                _loadMutex.lock();
                if ( !_tilesToLoad.empty() ) {
                    tIndices = _tilesToLoad.back();
                    _tilesToLoad.pop_back();
                }
                _loadMutex.unlock();
                if ( tIndices.first != -1 && tIndices.second != -1 ) {
                    preLoadTile( tIndices.first, tIndices.second );
                }
            }
            std::cout << "Thread over." << std::endl;
        }
    };
    // else if bpc == 16
    wkThread.detach();
}
void HeightfieldScalable16::closeHeightfield() {
    std::cout << "Closing loading threads.." << std::endl;
    _tilesToLoad.clear();
    _endThread = true;
    _loadSignal.notify_all();
    using namespace std::chrono_literals;
    std::this_thread::sleep_for( 5s );
    
}