#include "Heightfield.h"

#include <iostream>
#include "MathCalcs.h"
#include "SystemErrors.h"
#include "Vector.h"
#include "VectorCalcs.h"

#include "ThreadSplitter.h"
#include <cmath>
#include <memory>

#include "DebugTimer.h"

#include "Bits.h"

#include "Color_16b.h"

#include "StringManipulation.h"

#include "DebugPrint.h"

namespace {
    Color3C convertNormalToPixel( Vector3D norm ) {
        return   Color3C( (unsigned char)( (norm.x+1.0f) / 2.0f * 255.0f ),
            (unsigned char)( (norm.y+1.0f) / 2.0f * 255.0f ),
            (unsigned char)( (norm.z+1.0f) / 2.0f * 255.0f ) );
    }

    // Check if minV <= v <= maxV
    bool validInterval( int v, int minV, int maxV ) {
        return mathCalcs::isInRangeInclusive( v, minV, maxV );
    }

    int padDimensionPOT_PerPoint( int dim ) {
        if ( !mathCalcs::isPowerOfTwo( dim - 1 ) ) {
            dim = nextPowerOfTwo( dim - 1 ) + 1;
        }
        return dim;
    }
    int padDimensionPOT_PerPatch( int dim ) {
        if ( !mathCalcs::isPowerOfTwo( dim ) ) {
            dim = nextPowerOfTwo( dim );
        }
        return dim;
    }

    int padBestDimensions( int dimA, int dimB ) {
        return mathCalcs::max( dimA, dimB );
    }
}

Heightfield::Heightfield() {

}
Heightfield::~Heightfield() {

}

bool Heightfield::loadHeightmap_8b( const std::string& file ) {
    _bpc = HeightfieldBPC::HF_BPC_8;
    DebugTimer tmr;

    tmr.start();
    std::cout << "Loading heightmap image: ";
	// Load texture - assuming dimensions equal to POWER-OF-TWO+1 (ex: 257, 1025, etc..)
    if ( !_hftex.loadTexture( file ) ) {
        std::cout << "Failed." << std::endl;
        return false;
    }
    tmr.stop();
    std::cout << " Complete. " << tmr.getTimeElapsed()/1000.0 << "s" << std::endl;

    // Real dimensions are the dimensions before padding the heightfield
    // We subtract 1 unit because the original heightmap is in per point format and the engine
    // will require the per patch format.
    _realDims.setVector( _hftex.getWidth()-1, _hftex.getHeight()-1 );

    PixelArray<Color4C> hfpixels;
    hfpixels.createPixelArrayEmpty( _hftex.getWidth(), _hftex.getHeight() );
    _hftex.downloadPixels( hfpixels.getPixels(), GL_RGBA, GL_UNSIGNED_INT_8_8_8_8 );

    // Pad heightfield if not power of two
    int reWidth = ::padDimensionPOT_PerPoint( _hftex.getWidth() );
    int reHeight = ::padDimensionPOT_PerPoint( _hftex.getHeight() );

    reWidth = ::padBestDimensions( reWidth, reHeight );
    reHeight = ::padBestDimensions( reHeight, reWidth );

    if ( ( reWidth != _hftex.getWidth() ) || ( reHeight != _hftex.getHeight() ) ) {
        PixelArray<Color4C> padpix;
        padpix.createPixelArrayEmpty( reWidth, reHeight );
        padpix.fill( 0 );
        padpix.overwritePixelArray( hfpixels, 0, 0 );

        hfpixels = padpix;
    }
    else {
        std::cout << "Heightfield does not require padding." << std::endl;
    }
    _hftex.closeTexture();

    std::cout << "Heightfield done." << std::endl;

    int width = hfpixels.getWidth(); // POT+1
    int height = hfpixels.getHeight(); // POT+1

    int newWidth = width - 1; // POT
    int newHeight = height - 1; // POT

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
                    compact.getPixel( x, y ).red = hfpixels[ y* width + x ].red;
                    compact.getPixel( x, y ).green = hfpixels[ y* width + x + 1 ].red;
                    compact.getPixel( x, y ).blue = hfpixels[ ( y + 1 )* width + x ].red;
                    compact.getPixel( x, y ).alpha = hfpixels[ ( y + 1 )* width + x + 1 ].red;
                }
            }
        },
        newWidth, newHeight, 8 );
    tmr.stop();
    std::cout << "Reformat heightfield: " << tmr.getTimeElapsed() / 1000.0 << "s" << std::endl;
    
    // Keep a copy of the heightfield in RAM for collision detection
    int cpSize = compact.getWidth() * compact.getHeight();
    _compactData.reallocate( cpSize * 4 );
    Color4C* compactStoragePoint = reinterpret_cast<Color4C*>( _compactData.getPointer() );
    for ( int i = 0; i < cpSize; ++i ) {
        compactStoragePoint[ i ] = compact[ i ];
    }

    // No need to keep the heightmap texture open
    _hftex.closeTexture();

    // Create Compact heightmap texture
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
    if ( !_maximumMipmaps.loadTexture( topMipmap.getPixels(), newWidth, newHeight) ) {
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


    std::cout << "Generating maximum mipmaps: ";
    tmr.start();
    int lodWidth = newWidth;
    int lodHeight = newHeight;
    while ( lodHeight != 1 && lodWidth != 1 ) {
        lodWidth /= 2;
        lodHeight /= 2;

        //std::cout << "MipLevel("<< level << ") W:  " << newWidth << " " << " H:  " << newHeight << std::endl;

		// Create half sized buffer to hold data for the next level
        PixelArray<Color4C> mipmapData;
        mipmapData.createPixelArrayEmpty( lodWidth, lodHeight );

		// Multithreading can be applied to the following nested for loop
		// However, the last few levels have very small workloads and do not require multithreading
        for ( int x = 0; x < lodWidth; ++x ) {
            for ( int y = 0; y < lodHeight; ++y ) {
                int tx = x * 2;
                int ty = y * 2;

                    mipmapData.getPixel( x, y ).red =
                        mathCalcs::max( mathCalcs::max( (int)topMipmap.getPixel( tx, ty ).red, (int)topMipmap.getPixel( tx + 1, ty ).red ),
                            mathCalcs::max( (int)topMipmap.getPixel( tx, ty + 1 ).red, (int)topMipmap.getPixel( tx + 1, ty + 1 ).red ) );
            }
        }

		// Create next level mipmap
        //glTexImage2D( GL_TEXTURE_2D, level, GL_R8, newWidth, newHeight, 0, GL_RED, GL_UNSIGNED_BYTE, mipmapData.getPixels() );
        glTexImage2D( GL_TEXTURE_2D, level, GL_RGBA, lodWidth, lodHeight, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, mipmapData.getPixels() );

        topMipmap = mipmapData;
        level++;
    }
    tmr.stop();
    std::cout << tmr.getTimeElapsed() / 1000.0 << "s" << std::endl;
    _maximumMipmaps.unbindTexture();

	// Maximum mipmap structure will be accessed per index. Interpolation is not needed.
    _maximumMipmaps.setMinMagFilter( GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST );
    _maximumMipmaps.setWrapMode( GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE );

	// #level is incremented one last time by the end of the previous loop
    _lastLevel = level-1;

    // Create compact mipmaps
    std::cout << "Generating heightfield mipmaps: ";
    _hfCompact.bindTexture();

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0 );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, maxLevel );

    tmr.start();
    level = 1;
    lodWidth = newWidth;
    lodHeight = newHeight;
    while ( lodHeight != 1 && lodWidth != 1 ) {
        lodWidth /= 2;
        lodHeight /= 2;

        //std::cout << "MipLevel(" << level << ") W:  " << newWidth << " " << " H:  " << newHeight << std::endl;

        // Create half sized buffer to hold data for the next level
        PixelArray<Color4C> compactData;
        compactData.createPixelArrayEmpty( lodWidth, lodHeight );

        // Multithreading can be applied to the following nested for loop
        // However, the last few levels have very small workloads and do not require multithreading
        for ( int x = 0; x < lodWidth; ++x ) {
            for ( int y = 0; y < lodHeight; ++y ) {
                int tx = x * 2;
                int ty = y * 2;

                compactData.getPixel( x, y ).setColor(
                    compact.getPixel( tx, ty ).red,
                    compact.getPixel( tx + 1, ty ).green,
                    compact.getPixel( tx, ty + 1 ).blue,
                    compact.getPixel( tx + 1, ty + 1 ).alpha );
            }
        }

        // Create next level mipmap
        //glTexImage2D( GL_TEXTURE_2D, level, GL_R8, newWidth, newHeight, 0, GL_RED, GL_UNSIGNED_BYTE, mipmapData.getPixels() );
        glTexImage2D( GL_TEXTURE_2D, level, GL_RGBA, lodWidth, lodHeight, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, compactData.getPixels() );

        compact = compactData;
        level++;
    }
    tmr.stop();
    std::cout << tmr.getTimeElapsed() / 1000.0 << "s" << std::endl;
    _hfCompact.unbindTexture();

    return true;
}

bool Heightfield::loadCompactHeightmap_8b( const std::string& file ) {
    _bpc = HeightfieldBPC::HF_BPC_8;

    DebugTimer tmr;
    
    tmr.start();
    if ( !_hfCompact.loadTexture( file  ) ) {
        std::cout << "Error loading image to GPU." << std::endl;
        getchar();
        return false;
    }
    tmr.stop();
    std::cout << "Loaded heightfield: " << tmr.getTimeElapsed() / 1000.0 << "s" << std::endl;

    // Real dimensions are the dimensions before padding the heightfield
    _realDims.setVector( _hfCompact.getWidth(), _hfCompact.getHeight() );

    // Pad heightfield if not power of two
    PixelArray<Color4C> compactPix;

    int reWidth = ::padDimensionPOT_PerPatch( _hfCompact.getWidth() );
    int reHeight = ::padDimensionPOT_PerPatch( _hfCompact.getHeight() );
    reWidth = ::padBestDimensions( reWidth, reHeight );
    reHeight = ::padBestDimensions( reHeight, reWidth );
    if ( ( reWidth != _hfCompact.getWidth() ) || ( reHeight != _hfCompact.getHeight() ) ) {
        std::cout << "Padding heightfield: (" << _hfCompact.getWidth() << " x " << _hfCompact.getHeight() << ") -> (" << reWidth << " x " << reHeight << ")" << std::endl;

        PixelArray<Color4C> compactTemp;
        compactTemp.createPixelArrayEmpty( _hfCompact.getWidth(), _hfCompact.getHeight() );
        _hfCompact.downloadPixels( compactTemp.getPixels(), GL_RGBA, GL_UNSIGNED_INT_8_8_8_8 );

        compactPix.createPixelArrayEmpty( reWidth, reHeight );
        compactPix.fill( 0 );

        compactPix.overwritePixelArray( compactTemp, 0, 0 );

        _hfCompact.closeTexture();
        if ( !_hfCompact.loadTexture( compactPix.getPixels(), reWidth, reHeight ) ) {
            std::cout << "Could not load heightfield after padding!" << std::endl;
        }
    }
    else {
        std::cout << "Padding not necessary: 0.0s" << std::endl;
        compactPix.createPixelArrayEmpty( reWidth, reHeight );
        _hfCompact.downloadPixels( compactPix.getPixels(), GL_RGBA, GL_UNSIGNED_INT_8_8_8_8 );
    }

    // Keep a copy of the heightfield in RAM for collision detection
    int cpSize = compactPix.getWidth() * compactPix.getHeight();
    _compactData.reallocate( cpSize * 4 );
    Color4C* compactStoragePoint = reinterpret_cast<Color4C*>( _compactData.getPointer() );
    for ( int i = 0; i < cpSize; ++i ) {
        compactStoragePoint[ i ] = compactPix[ i ];
    }

    int width = _hfCompact.getWidth();
    int height = _hfCompact.getHeight();

    // Create finest mipmap level
	// Where each pixel contains the maximum height value from the corresponding patch in the compact format
    PixelArray<Color4C> topMipmap;
    topMipmap.createPixelArrayEmpty( width, height );

    tmr.start();
    ThreadSplitter::threadizeDual(
        [&]( int xa, int xb, int ya, int yb ) {
            for ( int x = xa; x < xb; ++x ) {
                for ( int y = ya; y < yb; ++y ) {
                    Color4C sample = compactPix[ y * width + x ];
                    topMipmap.getPixel( x, y ).red = 
                        (GLubyte)mathCalcs::max( mathCalcs::max( (int)sample.green, (int)sample.red ),
                                                 mathCalcs::max( (int)sample.blue, (int)sample.alpha ) );
                }
            }
        },
        width, height, 8 );
    tmr.stop();
    std::cout << "Create mipmap toplevel: " << tmr.getTimeElapsed() / 1000.0 << "s" << std::endl;
    //_hfCompact.discardPixels();

	// Create finest mipmap level with POT dimensions (ex: 1024x1024)
    /*if ( !_maximumMipmaps.createSpecialTexture( newWidth, newHeight, GL_R8, GL_RED, topMipmap.getPixels() ) ) {
        return false;
    }*/
    if ( !_maximumMipmaps.loadTexture( topMipmap.getPixels(), width, height ) ) {
        std::cout << "Failed to create maximum mipmap texture." << std::endl;
        return false;
    }

    // Create remaining mipmap levels starting at level 1
	// Each consecutive level has half the size of the previous level (ex: L1=512x512,L2=256x256, etc)
    int level = 1;
    _maximumMipmaps.bindTexture();

    int maxLevel = (int)log2( width );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0 );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, maxLevel );
    std::cout << "Max LOD: " << maxLevel << std::endl;

    int lodWidth = width;
    int lodHeight = height;

    std::cout << "Generating maximum mipmaps: ";
    tmr.start();
    while ( lodHeight != 1 && lodWidth != 1 ) {
        lodWidth /= 2;
        lodHeight /= 2;

        //std::cout << "MipLevel("<< level << ") W:  " << width << " " << " H:  " << height << std::endl;

		// Create half sized buffer to hold data for the next level
        PixelArray<Color4C> mipmapData;
        mipmapData.createPixelArrayEmpty( lodWidth, lodHeight );

		// Multithreading can be applied to the following nested for loop
		// However, the last few levels have very small workloads and do not require multithreading
        for ( int x = 0; x < lodWidth; ++x ) {
            for ( int y = 0; y < lodHeight; ++y ) {
                int tx = x * 2;
                int ty = y * 2;

                    mipmapData.getPixel( x, y ).red =
                        mathCalcs::max( mathCalcs::max( (int)topMipmap.getPixel( tx, ty ).red, (int)topMipmap.getPixel( tx + 1, ty ).red ),
                            mathCalcs::max( (int)topMipmap.getPixel( tx, ty + 1 ).red, (int)topMipmap.getPixel( tx + 1, ty + 1 ).red ) );
            }
        }

		// Create next level mipmap
        //glTexImage2D( GL_TEXTURE_2D, level, GL_R8, newWidth, newHeight, 0, GL_RED, GL_UNSIGNED_BYTE, mipmapData.getPixels() );
        glTexImage2D( GL_TEXTURE_2D, level, GL_RGBA, lodWidth, lodHeight, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, mipmapData.getPixels() );

        topMipmap = mipmapData;
        level++;
    }
    tmr.stop();
    std::cout << tmr.getTimeElapsed() / 1000.0 << "s" << std::endl;
    _maximumMipmaps.unbindTexture();

	// Maximum mipmap structure will be accessed per index. Interpolation is not needed.
    _maximumMipmaps.setMinMagFilter( GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST );
    _maximumMipmaps.setWrapMode( GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE );

	// #level is incremented one last time by the end of the previous loop
    _lastLevel = level-1;

    // Maximum value could aid in determining a better/smaller value for the heightmap Z dimensions, as a small optimization
    /*int max = topMipmap.getPixel(0,0);*/

    // Create compact mipmaps
    std::cout << "Generating heightfield mipmaps: ";
    _hfCompact.bindTexture();

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0 );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, maxLevel );

    tmr.start();
    level = 1;
    lodWidth = width;
    lodHeight = height;
    while ( lodHeight != 1 && lodWidth != 1 ) {
        lodWidth /= 2;
        lodHeight /= 2;

        //std::cout << "MipLevel(" << level << ") W:  " << newWidth << " " << " H:  " << newHeight << std::endl;

        // Create half sized buffer to hold data for the next level
        PixelArray<Color4C> compactData;
        compactData.createPixelArrayEmpty( lodWidth, lodHeight );

        // Multithreading can be applied to the following nested for loop
        // However, the last few levels have very small workloads and do not require multithreading
        for ( int x = 0; x < lodWidth; ++x ) {
            for ( int y = 0; y < lodHeight; ++y ) {
                int tx = x * 2;
                int ty = y * 2;

                compactData.getPixel( x, y ).setColor(
                    compactPix.getPixel( tx, ty ).red,
                    compactPix.getPixel( tx + 1, ty ).green,
                    compactPix.getPixel( tx, ty + 1 ).blue,
                    compactPix.getPixel( tx + 1, ty + 1 ).alpha );
            }
        }

        // Create next level mipmap
        //glTexImage2D( GL_TEXTURE_2D, level, GL_R8, newWidth, newHeight, 0, GL_RED, GL_UNSIGNED_BYTE, mipmapData.getPixels() );
        glTexImage2D( GL_TEXTURE_2D, level, GL_RGBA, lodWidth, lodHeight, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, compactData.getPixels() );

        compactPix = compactData;
        level++;
    }
    tmr.stop();
    std::cout << tmr.getTimeElapsed() / 1000.0 << "s" << std::endl;
    _hfCompact.unbindTexture();

    return true;
}


bool Heightfield::loadHeightmap_16b( const std::string& file ) {
    _bpc = HeightfieldBPC::HF_BPC_16;

    DebugTimer tmr;

    tmr.start();
    std::cout << "Loading 16-bit heightmap image: ";
    // Load texture - assuming dimensions equal to POWER-OF-TWO+1 (ex: 257, 1025, etc..)
    if ( !_hftex.loadTexture_16bit( file ) ) {
        std::cout << "Failed to load 16bit heightfield." << std::endl;
        return false;
    }
    tmr.stop();
    std::cout << " Complete. " << tmr.getTimeElapsed() / 1000.0 << "s" << std::endl;

    // Real dimensions are the dimensions before padding the heightfield
    // We subtract 1 unit because the original heightmap is in per point format and the engine
    // will require the per patch format.
    _realDims.setVector( _hftex.getWidth() - 1, _hftex.getHeight() - 1 );

    PixelArray<GLushort> hfpixels;

    // Pad heightfield if not power of two
    int reWidth = ::padDimensionPOT_PerPoint( _hftex.getWidth() );
    int reHeight = ::padDimensionPOT_PerPoint( _hftex.getHeight() );
    reWidth = ::padBestDimensions( reWidth, reHeight );
    reHeight = ::padBestDimensions( reHeight, reWidth );

    if ( (reWidth != _hftex.getWidth()) || (reHeight != _hftex.getHeight()) ) {
        std::cout << "Padding heightfield: (" << _hftex.getWidth() << " x " << _hftex.getHeight() << ") -> (" << reWidth << " x " << reHeight << ")" << std::endl;
        hfpixels.createPixelArray( _hftex.getWidth(), _hftex.getHeight() );
        hfpixels.fill( 0 );

        _hftex.downloadPixels( hfpixels.getPixels(), GL_RED, GL_UNSIGNED_SHORT );

        PixelArray<GLushort> padpix;
        padpix.createPixelArray( reWidth, reHeight );
        padpix.overwritePixelArray( hfpixels, 0, 0 );

        hfpixels = padpix;
    }
    else {
        std::cout << "Padding not necessary." << std::endl;
        hfpixels.createPixelArrayEmpty( _hftex.getWidth(), _hftex.getHeight() );

        _hftex.downloadPixels( hfpixels.getPixels(), GL_RED, GL_UNSIGNED_SHORT );
    }
    _hftex.closeTexture();
    std::cout << "Done processing heightfield." << std::endl;

    int width = hfpixels.getWidth(); // POT+1
    int height = hfpixels.getHeight(); // POT+1

    // Compact format must be HFDIM-1: Per Point -> Per Patch
    int newWidth = width - 1; // POT
    int newHeight = height - 1; // POT

    PixelArray<Color4C_16b> compact;
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
                compact.getPixel( x, y ).alpha = hfpixels[ y* width + x ];
                compact.getPixel( x, y ).blue = hfpixels[ y* width + x + 1 ];
                compact.getPixel( x, y ).green = hfpixels[ ( y + 1 )* width + x ];
                compact.getPixel( x, y ).red = hfpixels[ ( y + 1 )* width + x + 1 ];
            }
        }
    },
        newWidth, newHeight, 8 );
    tmr.stop();
    std::cout << "Reformat heightfield: " << tmr.getTimeElapsed() / 1000.0 << "s" << std::endl;

    // Keep a copy of the heightfield in RAM for collision detection
    int cpSize = compact.getWidth() * compact.getHeight();
    _compactData.reallocate( cpSize *8 );
    Color4C_16b* compactStoragePoint = reinterpret_cast<Color4C_16b*>( _compactData.getPointer() );
    for ( int i = 0; i < cpSize; ++i ) {
        compactStoragePoint[ i ] = compact[ i ];
    }

    if ( getOpenGLError( "Something went wrong here" ) ) {
        std::cout << "Something went wrong here." << std::endl;
        getchar();
    }

    tmr.start();
    if ( !_hfCompact.loadTexture( compact.getPixels(), newWidth, newHeight, GL_RGBA16, GL_RGBA, GL_UNSIGNED_SHORT ) ) {
        std::cout << "Error loading compact format image to GPU." << std::endl;
        getchar();
        return false;
    }
    tmr.stop();
    std::cout << "Upload heightfield: " << tmr.getTimeElapsed() / 1000.0 << "s" << std::endl;

    // Create finest mipmap level
    // Where each pixel contains the maximum height value from the corresponding patch in the compact format
    PixelArray<unsigned short> topMipmap;
    topMipmap.createPixelArrayEmpty( newWidth, newHeight );

    tmr.start();
    ThreadSplitter::threadizeDual(
        [&]( int xa, int xb, int ya, int yb ) {
        for ( int x = xa; x < xb; ++x ) {
            for ( int y = ya; y < yb; ++y ) {
                Color4C_16b sample = compact.getPixel( x, y );
                topMipmap.getPixel( x, y ) =
                    mathCalcs::max( mathCalcs::max( sample.green, sample.red ),
                        mathCalcs::max( sample.blue, sample.alpha ) );
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
    if ( !_maximumMipmaps.loadTexture( topMipmap.getPixels(), newWidth, newHeight, GL_R16, GL_RED, GL_UNSIGNED_SHORT ) ) {
        std::cout << "Failed to create maximum mipmap texture." << std::endl;
        return false;
    }

    // Create remaining mipmap levels starting at level 1
    // Each consecutive level has half the size of the previous level (ex: L1=512x512,L2=256x256, etc)
    int level = 1;
    int lodWidth = newWidth;
    int lodHeight = newHeight;
    int maxLevel = (int)log2( newWidth );

    _maximumMipmaps.bindTexture();
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0 );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, maxLevel );
    std::cout << "Max LOD: " << maxLevel << std::endl;

    std::cout << "Generating maximum mipmaps: ";
    tmr.start();
    while ( lodHeight != 1 && lodWidth != 1 ) {
        lodWidth /= 2;
        lodHeight /= 2;

        std::cout << "MipLevel(" << level << ") W:  " << lodWidth << " " << " H:  " << lodHeight << std::endl;

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
                        mathCalcs::max( topMipmap.getPixel( tx, ty + 1 ), (topMipmap.getPixel( tx + 1, ty + 1 ) ) ) );
            }
        }

        // Create next level mipmap
        //glTexImage2D( GL_TEXTURE_2D, level, GL_R8, newWidth, newHeight, 0, GL_RED, GL_UNSIGNED_BYTE, mipmapData.getPixels() );
        glTexImage2D( GL_TEXTURE_2D, level, GL_R16, lodWidth, lodHeight, 0, GL_RED, GL_UNSIGNED_SHORT, mipmapData.getPixels() );

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

    // Create compact mipmaps
    std::cout << "Generating heightfield mipmaps: ";

    _hfCompact.bindTexture();

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0 );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, maxLevel );

    tmr.start();
    level = 1;
    lodWidth = newWidth;
    lodHeight = newHeight;
    while ( lodHeight != 1 && lodWidth != 1 ) {
        lodWidth /= 2;
        lodHeight /= 2;

        std::cout << "HF MipLevel(" << level << ") W:  " << lodWidth << " " << " H:  " << lodHeight << std::endl;

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
                    compact.getPixel( tx + 1, ty + 1 ).red,
                    compact.getPixel( tx, ty +1 ).green,
                    compact.getPixel( tx+1, ty  ).blue,
                    compact.getPixel( tx, ty).alpha );
            }
        }

        // Create next level mipmap
        glTexImage2D( GL_TEXTURE_2D, level, GL_RGBA16, lodWidth, lodHeight, 0, GL_RGBA, GL_UNSIGNED_SHORT, compactData.getPixels() );

        compact = compactData;
        level++;
    }
    tmr.stop();
    std::cout << tmr.getTimeElapsed() / 1000.0 << "s" << std::endl;
    _hfCompact.unbindTexture();

    return true;
}

bool Heightfield::loadCompactHeightmap_16b( const std::string& file ) {
    _bpc = HeightfieldBPC::HF_BPC_16;

    DebugTimer tmr;

    tmr.start();
    std::cout << "Loading 16-bit heightmap image: ";
    // Load texture - assuming dimensions equal to POWER-OF-TWO+1 (ex: 257, 1025, etc..)
    if ( !_hfCompact.loadTexture_16bitRGBA( file ) ) {
        std::cout << "Failed to load 16-bit per patch heightfield." << std::endl;
        return false;
    }
    tmr.stop();
    std::cout << tmr.getTimeElapsed() / 1000.0 << "s" << std::endl;

    // Real dimensions are the dimensions before padding the heightfield
    _realDims.setVector( _hfCompact.getWidth(), _hfCompact.getHeight() );

    PixelArray<Color4C_16b> cpPixels;

    // Pad heightfield if not power of two
    int reWidth = ::padDimensionPOT_PerPatch( _hfCompact.getWidth() );
    int reHeight = ::padDimensionPOT_PerPatch( _hfCompact.getHeight() );
    reWidth = ::padBestDimensions( reWidth, reHeight );
    reHeight = ::padBestDimensions( reHeight, reWidth );

    if ( ( reWidth != _hfCompact.getWidth() ) || ( reHeight != _hfCompact.getHeight() ) ) {
        std::cout << "Padding heightfield: (" << _hfCompact.getWidth() << " x " << _hfCompact.getHeight() << ") -> (" << reWidth << " x " << reHeight << ")" << std::endl;
        cpPixels.createPixelArrayEmpty( _hftex.getWidth(), _hftex.getHeight() );

        _hfCompact.downloadPixels( cpPixels.getPixels(), GL_RGBA, GL_UNSIGNED_SHORT );

        PixelArray<Color4C_16b> padpix;
        padpix.createPixelArray( reWidth, reHeight );
        padpix.overwritePixelArray( cpPixels, 0, 0 );

        cpPixels = padpix;
    }
    else {
        std::cout << "Padding not necessary." << std::endl;
        cpPixels.createPixelArrayEmpty( _hfCompact.getWidth(), _hfCompact.getHeight() );

        _hfCompact.downloadPixels( cpPixels.getPixels(), GL_RGBA, GL_UNSIGNED_SHORT );
    }
    std::cout << "Done reading heightfield." << std::endl;

    // Keep a copy of the heightfield in RAM for collision detection
    int cpSize = cpPixels.getWidth() * cpPixels.getHeight();
    _compactData.reallocate( cpSize * 8 );
    Color4C_16b* compactStoragePoint = reinterpret_cast<Color4C_16b*>( _compactData.getPointer() );
    for ( int i = 0; i < cpSize; ++i ) {
        compactStoragePoint[ i ] = cpPixels[ i ];
    }

    int width = cpPixels.getWidth();
    int height = cpPixels.getHeight();

    // Create finest mipmap level
    // Where each pixel contains the maximum height value from the corresponding patch in the compact format
    PixelArray<unsigned short> topMipmap;
    topMipmap.createPixelArrayEmpty( width, height );

    tmr.start();
    ThreadSplitter::threadizeDual(
        [&]( int xa, int xb, int ya, int yb ) {
        for ( int x = xa; x < xb; ++x ) {
            for ( int y = ya; y < yb; ++y ) {
                Color4C_16b sample = cpPixels.getPixel( x, y );
                topMipmap.getPixel( x, y ) =
                    mathCalcs::max( mathCalcs::max( sample.green, sample.red ),
                        mathCalcs::max( sample.blue, sample.alpha ) );
            }
        }
    },
        width, height, 8 );
    tmr.stop();
    std::cout << "Create mipmap toplevel: " << tmr.getTimeElapsed() / 1000.0 << "s" << std::endl;

    // Create finest mipmap level with POT dimensions (ex: 1024x1024)
    /*if ( !_maximumMipmaps.createSpecialTexture( newWidth, newHeight, GL_R8, GL_RED, topMipmap.getPixels() ) ) {
    return false;
    }*/
    if ( !_maximumMipmaps.loadTexture( topMipmap.getPixels(), width, height, GL_R16, GL_RED, GL_UNSIGNED_SHORT ) ) {
        std::cout << "Failed to create maximum mipmap texture." << std::endl;
        return false;
    }

    // Create remaining mipmap levels starting at level 1
    // Each consecutive level has half the size of the previous level (ex: L1=512x512,L2=256x256, etc)
    int level = 1;
    int lodWidth = width;
    int lodHeight = height;
    int maxLevel = (int)log2( width ); // Image will always be square - either width or heigth will work

    _maximumMipmaps.bindTexture();
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0 );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, maxLevel );
    std::cout << "Max LOD: " << maxLevel << std::endl;

    std::cout << "Generating maximum mipmaps: ";
    tmr.start();
    while ( lodHeight != 1 && lodWidth != 1 ) {
        lodWidth /= 2;
        lodHeight /= 2;

        std::cout << "MipLevel(" << level << ") W:  " << lodWidth << " " << " H:  " << lodHeight << std::endl;

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
                        mathCalcs::max( topMipmap.getPixel( tx, ty + 1 ), ( topMipmap.getPixel( tx + 1, ty + 1 ) ) ) );
            }
        }

        // Create next level mipmap
        //glTexImage2D( GL_TEXTURE_2D, level, GL_R8, newWidth, newHeight, 0, GL_RED, GL_UNSIGNED_BYTE, mipmapData.getPixels() );
        glTexImage2D( GL_TEXTURE_2D, level, GL_R16, lodWidth, lodHeight, 0, GL_RED, GL_UNSIGNED_SHORT, mipmapData.getPixels() );

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

    // Create compact mipmaps
    std::cout << "Generating heightfield mipmaps: ";

    _hfCompact.bindTexture();

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0 );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, maxLevel );

    tmr.start();
    level = 1;
    lodWidth = width;
    lodHeight = height;
    while ( lodHeight != 1 && lodWidth != 1 ) {
        lodWidth /= 2;
        lodHeight /= 2;

        std::cout << "HF MipLevel(" << level << ") W:  " << lodWidth << " " << " H:  " << lodHeight << std::endl;

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
                    cpPixels.getPixel( tx + 1, ty + 1 ).red,
                    cpPixels.getPixel( tx, ty + 1 ).green,
                    cpPixels.getPixel( tx + 1, ty ).blue,
                    cpPixels.getPixel( tx, ty ).alpha );
            }
        }

        // Create next level mipmap
        glTexImage2D( GL_TEXTURE_2D, level, GL_RGBA16, lodWidth, lodHeight, 0, GL_RGBA, GL_UNSIGNED_SHORT, compactData.getPixels() );

        cpPixels = compactData;
        level++;
    }
    tmr.stop();
    std::cout << tmr.getTimeElapsed() / 1000.0 << "s" << std::endl;
    _hfCompact.unbindTexture();

    return true;
}


bool Heightfield::preComputeNormals() {
    _hfCompact.downloadPixels();

    auto compactHF = _hfCompact.getPixels();

    int maxW = _hfCompact.getWidth();
    int maxH = _hfCompact.getHeight();

    PixelArray< Color3C > normals;
    normals.createPixelArray( _hfCompact.getWidth() + 1, _hfCompact.getHeight() + 1 );

    // Lower left normal
    {
        Vector3D nLL_right = Vector3D( 1.0, 0.0, compactHF[ 0 ].green - compactHF[ 0 ].red );
        Vector3D nLL_front = Vector3D( 0.0, 1.0, compactHF[ 0 ].blue - compactHF[ 0 ].red );
        Vector3D nLL_left = Vector3D( 0, 0, 1 );
        Vector3D nLL_back = Vector3D( 0, 0, 1 );
        nLL_right = rotate90_Yccw( nLL_right ); nLL_right.normalize();
        nLL_front = rotate90_Xccw( nLL_front ); nLL_front.normalize();
        Vector3D normLL = nLL_right * 0.25f + nLL_front * 0.25f + nLL_left * 0.25f * nLL_back * 0.25f;
        normLL.normalize();
        normals.getPixel( 0, 0 ) = ::convertNormalToPixel( normLL );
    }

    // Lower right normal
    {
        Vector3D nLR_right = Vector3D( 0, 0, 1 );
        Vector3D nLR_front = Vector3D( 0.0, 1.0, compactHF[ maxW ].alpha - compactHF[ maxW ].green );
        Vector3D nLR_left = Vector3D( -1.0, 0.0, compactHF[ maxW ].red - compactHF[ maxW ].green );
        Vector3D nLR_back = Vector3D( 0, 0, 1 );
        nLR_left = rotate90_Ycw( nLR_left ); nLR_left.normalize();
        nLR_front = rotate90_Xccw( nLR_front ); nLR_front.normalize();
        Vector3D normLR = nLR_right * 0.25f + nLR_front * 0.25f + nLR_left * 0.25f * nLR_back * 0.25f;
        normLR.normalize();
        normals.getPixel( maxW, 0 ) = ::convertNormalToPixel( normLR );
    }

    // Upper left normal
    {
        Vector3D nUL_right = Vector3D( 1.0, 0.0, compactHF[ maxW ].alpha - compactHF[ maxW ].blue );
        Vector3D nUL_front = Vector3D( 0, 0, 1 );
        Vector3D nUL_left = Vector3D( 0, 0, 1 );
        Vector3D nUL_back = Vector3D( 0.0, -1.0, compactHF[ maxW ].red - compactHF[ maxW ].blue );
        nUL_right = rotate90_Ycw( nUL_right ); nUL_right.normalize();
        nUL_back = rotate90_Xcw( nUL_back ); nUL_back.normalize();
        Vector3D normUL = nUL_right * 0.25f + nUL_front * 0.25f + nUL_left * 0.25f * nUL_back * 0.25f;
        normUL.normalize();
        normals.getPixel( 0, maxH ) = ::convertNormalToPixel( normUL );
    }

    // Upper right normal
    {
        Vector3D nUR_right = Vector3D( 0, 0, 1 );
        Vector3D nUR_front = Vector3D( 0, 0, 1 );
        Vector3D nUR_left = Vector3D( -1.0, 0.0, compactHF[ maxW ].blue - compactHF[ maxW ].alpha );
        Vector3D nUR_back = Vector3D( 0.0, -1.0, compactHF[ maxW ].green - compactHF[ maxW ].alpha );
        nUR_left = rotate90_Ycw( nUR_left ); nUR_left.normalize();
        nUR_back = rotate90_Xcw( nUR_back ); nUR_back.normalize();
        Vector3D normUR = nUR_right * 0.25f + nUR_front * 0.25f + nUR_left * 0.25f * nUR_back * 0.25f;
        normUR.normalize();
        normals.getPixel( maxW, maxH ) = ::convertNormalToPixel( normUR );
    }

    // Lower row normals
    for ( int x = 0; x < maxW-1; ++x ) {
        Vector3D n_right = Vector3D( 1.0, 0.0, compactHF[ x+1 ].green - compactHF[ x+1 ].red );
        Vector3D n_front = Vector3D( 0.0, 1.0, compactHF[ x ].alpha - compactHF[ x ].green );
        Vector3D n_left = Vector3D( -1.0, 0.0, compactHF[ x ].red - compactHF[ x ].green );
        Vector3D n_back = Vector3D( 0, 0, 1 );
        n_left = rotate90_Ycw( n_left ); n_left.normalize();
        n_back = rotate90_Xcw( n_back ); n_back.normalize();
        Vector3D norm = n_right * 0.25f + n_front * 0.25f + n_left * 0.25f * n_back * 0.25f;
        norm.normalize();
        normals.getPixel( x+1, 0 ) = ::convertNormalToPixel( norm );
    }
    // Upper row normals
    /*for ( int x = 0; x < maxW - 1; ++x ) {
        Vector3D n_right = Vector3D( 1.0, 0.0, compactHF[ x + 1 ].green - compactHF[ x + 1 ].red );
        Vector3D n_front = Vector3D( 0.0, 1.0, compactHF[ x ].alpha - compactHF[ x ].green );
        Vector3D n_left = Vector3D( -1.0, 0.0, compactHF[ x ].red - compactHF[ x ].green );
        Vector3D n_back = Vector3D( 0, 0, 1 );
        n_left = rotate90_Ycw( n_left ); n_left.normalize();
        n_back = rotate90_Xcw( n_back ); n_back.normalize();
        Vector3D norm = n_right * 0.25f + n_front * 0.25f + n_left * 0.25f * n_back * 0.25f;
        norm.normalize();
        normals.getPixel( x + 1, 0 ) = ::convertNormalToPixel( norm );
    }*/
    // Left column normals
    /*for ( int x = 0; x < maxW - 1; ++x ) {
        Vector3D n_right = Vector3D( 1.0, 0.0, compactHF[ x + 1 ].green - compactHF[ x + 1 ].red );
        Vector3D n_front = Vector3D( 0.0, 1.0, compactHF[ x ].alpha - compactHF[ x ].green );
        Vector3D n_left = Vector3D( -1.0, 0.0, compactHF[ x ].red - compactHF[ x ].green );
        Vector3D n_back = Vector3D( 0, 0, 1 );
        n_left = rotate90_Ycw( n_left ); n_left.normalize();
        n_back = rotate90_Xcw( n_back ); n_back.normalize();
        Vector3D norm = n_right * 0.25f + n_front * 0.25f + n_left * 0.25f * n_back * 0.25f;
        norm.normalize();
        normals.getPixel( x + 1, 0 ) = ::convertNormalToPixel( norm );
    }*/
    // Right column normals
    /*for ( int x = 0; x < maxW - 1; ++x ) {
        Vector3D n_right = Vector3D( 1.0, 0.0, compactHF[ x + 1 ].green - compactHF[ x + 1 ].red );
        Vector3D n_front = Vector3D( 0.0, 1.0, compactHF[ x ].alpha - compactHF[ x ].green );
        Vector3D n_left = Vector3D( -1.0, 0.0, compactHF[ x ].red - compactHF[ x ].green );
        Vector3D n_back = Vector3D( 0, 0, 1 );
        n_left = rotate90_Ycw( n_left ); n_left.normalize();
        n_back = rotate90_Xcw( n_back ); n_back.normalize();
        Vector3D norm = n_right * 0.25f + n_front * 0.25f + n_left * 0.25f * n_back * 0.25f;
        norm.normalize();
        normals.getPixel( x + 1, 0 ) = ::convertNormalToPixel( norm );
    }*/

    // Middle normals
    for ( int x = 1; x < maxW - 1; ++x ) {
        for ( int y = 1; y < maxH - 1; ++y ) {
            Vector3D n_right = Vector3D( 1.0, 0.0, compactHF[ x + y*maxW ].green - compactHF[ x + y*maxW ].red );
            Vector3D n_front = Vector3D( 0.0, 1.0, compactHF[ x + y*maxW ].blue - compactHF[ x + y*maxW ].red );
            Vector3D n_left = Vector3D( -1.0, 0.0, compactHF[ (x-1)+(y-1)*maxW ].blue - compactHF[ ( x - 1 ) + ( y - 1 )*maxW ].alpha );
            Vector3D n_back = Vector3D( 0.0, -1.0, compactHF[ ( x - 1 ) + ( y - 1 )*maxW ].green - compactHF[ ( x - 1 ) + ( y - 1 )*maxW ].alpha );
            n_front = rotate90_Xccw( n_front ); n_front.normalize();
            n_right = rotate90_Yccw( n_right ); n_right.normalize();
            n_left  = rotate90_Ycw( n_left ); n_left.normalize();
            n_back  = rotate90_Xcw( n_back ); n_back.normalize();
            Vector3D norm = n_right * 0.25f + n_front * 0.25f + n_left * 0.25f * n_back * 0.25f;
            norm.normalize();
            normals.getPixel( x, y ) = ::convertNormalToPixel( norm );
        }
    }

    _hfCompact.discardPixels();

    return _normalMap.loadTexture( normals.getPixels(), normals.getWidth(), normals.getHeight(), GL_RGB, GL_BGR, GL_UNSIGNED_BYTE );
}

bool Heightfield::preComputeNormals2() {
    int maxW = _hfCompact.getWidth();
    int maxH = _hfCompact.getHeight();

    PixelArray< Color4C > compactData;
    compactData.createPixelArrayEmpty( _hfCompact.getWidth(), _hfCompact.getHeight() );
    _hfCompact.downloadPixels( compactData.getPixels(), GL_RGBA, GL_UNSIGNED_INT_8_8_8_8 );

    PixelArray< Color4C > normals;
    normals.createPixelArray( maxW + 1, maxH + 1 );

    ThreadSplitter::threadizeDual(
        [&]( int xa, int xb, int ya, int yb ) {
        for ( int x = xa; x < xb; ++x ) {
            for ( int y = ya; y < yb; ++y ) {
                Vector3D nUp = genNormal( &compactData, x, y, x, y + 1 );
                Vector3D nDown = genNormal( &compactData, x, y, x, y - 1 );
                Vector3D nLeft = genNormal( &compactData, x, y, x - 1, y );
                Vector3D nRight = genNormal( &compactData, x, y, x + 1, y );

                Vector3D norm = nUp + nDown + nLeft + nRight; norm.normalize();
                normals.getPixel( x, y ) = Color4C( ::convertNormalToPixel( norm ) );
            }
        }
    },
        maxW, maxH, 8 );

    /*for ( int x = 0; x < maxW; ++x ) {
        for ( int y = 0; y < maxH; ++y ) {
            Vector3D nUp    = genNormal( &compactData, x, y, x, y + 1 );
            Vector3D nDown  = genNormal( &compactData, x, y, x, y - 1 );
            Vector3D nLeft  = genNormal( &compactData, x, y, x - 1, y );
            Vector3D nRight = genNormal( &compactData, x, y, x + 1, y );

            Vector3D norm = nUp + nDown + nLeft + nRight; norm.normalize();
            normals.getPixel( x, y ) = Color4C( ::convertNormalToPixel( norm ) );
        }
    }*/

    _normalMap.loadTexture( normals.getPixels(), normals.getWidth(), normals.getHeight(), GL_RGBA, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8 );// , GL_RGB, GL_BGR, GL_UNSIGNED_BYTE );
    _normalMap.setMinMagFilter( GL_LINEAR, GL_LINEAR );
    return _normalMap.isLoaded();
}

bool Heightfield::preComputeNormals_16b() {
    int maxW = _hfCompact.getWidth();
    int maxH = _hfCompact.getHeight();
    
    PixelArray< Color4C_16b > compactHF;
    compactHF.createPixelArrayEmpty( maxW, maxH );
    _hfCompact.downloadPixels( compactHF.getPixels(), GL_RGBA, GL_UNSIGNED_SHORT );

    PixelArray< Color4C > normals;
    normals.createPixelArray( maxW + 1, maxH + 1 );

    ThreadSplitter::threadizeDual(
        [&]( int xa, int xb, int ya, int yb ) {
        for ( int x = xa; x < xb; ++x ) {
            for ( int y = ya; y < yb; ++y ) {
                Vector3D nUp = genNormal16( &compactHF, x, y, x, y + 1 );
                Vector3D nDown = genNormal16( &compactHF, x, y, x, y - 1 );
                Vector3D nLeft = genNormal16( &compactHF, x, y, x - 1, y );
                Vector3D nRight = genNormal16( &compactHF, x, y, x + 1, y );

                Vector3D norm = nUp + nDown + nLeft + nRight; norm.normalize();
                normals.getPixel( x, y ) = Color4C( ::convertNormalToPixel( norm ) );
            }
        }
    },
        maxW, maxH, 8 );

    /*for ( int x = 0; x < maxW; ++x ) {
        for ( int y = 0; y < maxH; ++y ) {
            Vector3D nUp = genNormal16( &compactHF, x, y, x, y + 1 );
            Vector3D nDown = genNormal16( &compactHF, x, y, x, y - 1 );
            Vector3D nLeft = genNormal16( &compactHF, x, y, x - 1, y );
            Vector3D nRight = genNormal16( &compactHF, x, y, x + 1, y );

            Vector3D norm = nUp + nDown + nLeft + nRight; norm.normalize();
            normals.getPixel( x, y ) = Color4C( ::convertNormalToPixel( norm ) );
        }
    }*/

    _normalMap.loadTexture( normals.getPixels(), normals.getWidth(), normals.getHeight(), GL_RGBA, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8 );// , GL_RGB, GL_BGR, GL_UNSIGNED_BYTE );
    _normalMap.setMinMagFilter( GL_LINEAR, GL_LINEAR );
    return _normalMap.isLoaded();
}

bool Heightfield::loadPhotoTexture( const std::string &file ) {
    return _photoTexture.loadTexture( file, GL_COMPRESSED_RGBA_S3TC_DXT3_EXT, GL_RGBA, GL_UNSIGNED_BYTE );
}
bool Heightfield::loadDefaultPhotoTexture() {
    PixelArray< Color4C > defaultPtex;
    defaultPtex.createPixelArrayEmpty( 16, 16 );
    defaultPtex.fill( Color4C( 255, 255, 255, 255 ) );
    return _photoTexture.loadTexture( defaultPtex.getPixels(), 16, 16 );
}

Texture *Heightfield::getHeightmapTexture() {
    return &_hftex;
}
Texture *Heightfield::getCompactHeightmap() {
    return &_hfCompact;
}
Texture *Heightfield::getMaximumMipmap() {
    return &_maximumMipmaps;
}
Texture *Heightfield::getPhotoTexture() {
    return &_photoTexture;
}
Texture *Heightfield::getNormalMap() {
    return &_normalMap;
}

int Heightfield::getLastMipLevel() const {
    return _lastLevel;
}

Vector2D Heightfield::getHeightmapDimensions() {
    return Vector2D( _hfCompact.getWidth(), _hfCompact.getHeight() );
}
Vector2D Heightfield::getRealHeightmapDimensions() {
    return _realDims;
}


Vector3D Heightfield::genNormal( PixelArray<Color4C>* hmap, int ix, int iy, int tx, int ty ) {
    int hWidth = hmap->getWidth();
    int hHeight = hmap->getHeight();

    GLubyte iHeight = 0;
    if ( ::validInterval( ix, 0, hmap->getWidth() - 1 ) && ::validInterval( iy, 0, hmap->getHeight() - 1 ) ) {
        iHeight = hmap->getPixel( ix, iy ).red;
    }
    else if ( ::validInterval( ix - 1, 0, hmap->getWidth() - 1 ) && ::validInterval( iy - 1, 0, hmap->getHeight() - 1 ) ) {
        iHeight = hmap->getPixel( ix - 1, iy - 1 ).alpha;
    }

    // 0 = up, 1 = down, 2 = right, 3 = left
    int dir = ( ty != iy ? ( ty > iy ? 0 : 1 ) : ( tx > ix ? 2 : 3 ) );

    Vector3D normal{ 0.0f, 0.0f, 1.0f };
    // Up
    if ( dir == 0 ) {
        if ( ::validInterval( tx, 0, hmap->getWidth() - 1 ) && ::validInterval( ty, 0, hmap->getHeight() - 1 ) ) {
            GLubyte tHeight = hmap->getPixel( tx, ty ).blue;
            normal.setVector( 0, 1 * _patchSize, ( tHeight - iHeight ) / 255.0f * _mapHeight );
            normal = rotate90_Xccw( normal );
        }
        else if ( ::validInterval( tx - 1, 0, hmap->getWidth() - 1 ) && ::validInterval( ty, 0, hmap->getHeight() - 1 ) ) {
            GLubyte tHeight = hmap->getPixel( tx-1, ty ).alpha;
            normal.setVector( 0, 1 * _patchSize, ( tHeight - iHeight ) / 255.0f * _mapHeight );
            normal = rotate90_Xccw( normal );
        }
    }
    // Down
    else if ( dir == 1 ) {
        if ( ::validInterval( tx, 0, hmap->getWidth() - 1 ) && ::validInterval( ty-1, 0, hmap->getHeight() - 1 ) ) {
            GLubyte tHeight = hmap->getPixel( tx, ty-1 ).red;
            normal.setVector( 0, -1 * _patchSize, ( tHeight - iHeight ) / 255.0f * _mapHeight );
            normal = rotate90_Xcw( normal );
        }
        else if ( ::validInterval( tx - 1, 0, hmap->getWidth() - 1 ) && ::validInterval( ty-1, 0, hmap->getHeight() - 1 ) ) {
            GLubyte tHeight = hmap->getPixel( tx - 1, ty - 1 ).green;
            normal.setVector( 0, -1 * _patchSize, ( tHeight - iHeight ) / 255.0f * _mapHeight );
            normal = rotate90_Xcw( normal );
        }
    }
    // Right
    else if ( dir == 2 ) {
        if ( ::validInterval( tx, 0, hmap->getWidth() - 1 ) && ::validInterval( ty, 0, hmap->getHeight() - 1 ) ) {
            GLubyte tHeight = hmap->getPixel( tx, ty ).green;
            normal.setVector( 1 * _patchSize, 0, ( tHeight - iHeight ) / 255.0f * _mapHeight );
            normal = rotate90_Yccw( normal );
        }
        else if ( ::validInterval( tx, 0, hmap->getWidth() - 1 ) && ::validInterval( ty-1, 0, hmap->getHeight() - 1 ) ) {
            GLubyte tHeight = hmap->getPixel( tx, ty-1 ).alpha;
            normal.setVector( 1 * _patchSize, 0, ( tHeight - iHeight ) / 255.0f * _mapHeight );
            normal = rotate90_Yccw( normal );
        }
    }
    // Left
    else if ( dir == 3 ) {
        if ( ::validInterval( tx-1, 0, hmap->getWidth() - 1 ) && ::validInterval( ty, 0, hmap->getHeight() - 1 ) ) {
            GLubyte tHeight = hmap->getPixel( tx-1, ty ).red;
            normal.setVector( -1 * _patchSize, 0, ( tHeight - iHeight ) / 255.0f * _mapHeight );
            normal = rotate90_Ycw( normal );
        }
        else if ( ::validInterval( tx - 1, 0, hmap->getWidth() - 1 ) && ::validInterval( ty - 1, 0, hmap->getHeight() - 1 ) ) {
            GLubyte tHeight = hmap->getPixel( tx-1, ty - 1 ).blue;
            normal.setVector( -1 * _patchSize, 0, ( tHeight - iHeight ) / 255.0f * _mapHeight );
            normal = rotate90_Ycw( normal );
        }
    }
    normal.normalize();

    return normal;
}

Vector3D Heightfield::genNormal16( PixelArray<Color4C_16b>* hmap, int ix, int iy, int tx, int ty ) {
    int hWidth = hmap->getWidth();
    int hHeight = hmap->getHeight();

    GLushort iHeight = 0;
    if ( ::validInterval( ix, 0, hmap->getWidth() - 1 ) && ::validInterval( iy, 0, hmap->getHeight() - 1 ) ) {
        iHeight = hmap->getPixel( ix, iy ).red;
    }
    else if ( ::validInterval( ix - 1, 0, hmap->getWidth() - 1 ) && ::validInterval( iy - 1, 0, hmap->getHeight() - 1 ) ) {
        iHeight = hmap->getPixel( ix - 1, iy - 1 ).alpha;
    }

    // 0 = up, 1 = down, 2 = right, 3 = left
    int dir = ( ty != iy ? ( ty > iy ? 0 : 1 ) : ( tx > ix ? 2 : 3 ) );

    Vector3D normal{ 0.0f, 0.0f, 1.0f };
    // Up
    if ( dir == 0 ) {
        if ( ::validInterval( tx, 0, hmap->getWidth() - 1 ) && ::validInterval( ty, 0, hmap->getHeight() - 1 ) ) {
            GLushort tHeight = hmap->getPixel( tx, ty ).blue;
            normal.setVector( 0, 1 * _patchSize, ( tHeight - iHeight ) / 65536.0f * _mapHeight );
            normal = rotate90_Xccw( normal );
        }
        else if ( ::validInterval( tx - 1, 0, hmap->getWidth() - 1 ) && ::validInterval( ty, 0, hmap->getHeight() - 1 ) ) {
            GLushort tHeight = hmap->getPixel( tx - 1, ty ).alpha;
            normal.setVector( 0, 1 * _patchSize, ( tHeight - iHeight ) / 65536.0f * _mapHeight );
            normal = rotate90_Xccw( normal );
        }
    }
    // Down
    else if ( dir == 1 ) {
        if ( ::validInterval( tx, 0, hmap->getWidth() - 1 ) && ::validInterval( ty - 1, 0, hmap->getHeight() - 1 ) ) {
            GLushort tHeight = hmap->getPixel( tx, ty - 1 ).red;
            normal.setVector( 0, -1 * _patchSize, ( tHeight - iHeight ) / 65536.0f * _mapHeight );
            normal = rotate90_Xcw( normal );
        }
        else if ( ::validInterval( tx - 1, 0, hmap->getWidth() - 1 ) && ::validInterval( ty - 1, 0, hmap->getHeight() - 1 ) ) {
            GLushort tHeight = hmap->getPixel( tx - 1, ty - 1 ).green;
            normal.setVector( 0, -1 * _patchSize, ( tHeight - iHeight ) / 65536.0f * _mapHeight );
            normal = rotate90_Xcw( normal );
        }
    }
    // Right
    else if ( dir == 2 ) {
        if ( ::validInterval( tx, 0, hmap->getWidth() - 1 ) && ::validInterval( ty, 0, hmap->getHeight() - 1 ) ) {
            GLushort tHeight = hmap->getPixel( tx, ty ).green;
            normal.setVector( 1 * _patchSize, 0, ( tHeight - iHeight ) / 65536.0f * _mapHeight );
            normal = rotate90_Yccw( normal );
        }
        else if ( ::validInterval( tx, 0, hmap->getWidth() - 1 ) && ::validInterval( ty - 1, 0, hmap->getHeight() - 1 ) ) {
            GLushort tHeight = hmap->getPixel( tx, ty - 1 ).alpha;
            normal.setVector( 1 * _patchSize, 0, ( tHeight - iHeight ) / 65536.0f * _mapHeight );
            normal = rotate90_Yccw( normal );
        }
    }
    // Left
    else if ( dir == 3 ) {
        if ( ::validInterval( tx - 1, 0, hmap->getWidth() - 1 ) && ::validInterval( ty, 0, hmap->getHeight() - 1 ) ) {
            GLushort tHeight = hmap->getPixel( tx - 1, ty ).red;
            normal.setVector( -1 * _patchSize, 0, ( tHeight - iHeight ) / 65536.0f * _mapHeight );
            normal = rotate90_Ycw( normal );
        }
        else if ( ::validInterval( tx - 1, 0, hmap->getWidth() - 1 ) && ::validInterval( ty - 1, 0, hmap->getHeight() - 1 ) ) {
            GLushort tHeight = hmap->getPixel( tx - 1, ty - 1 ).blue;
            normal.setVector( -1 * _patchSize, 0, ( tHeight - iHeight ) / 65536.0f * _mapHeight );
            normal = rotate90_Ycw( normal );
        }
    }
    normal.normalize();

    return normal;
}

void Heightfield::setMapHeight( float maxHeight ) {
    _mapHeight = maxHeight;
}
float Heightfield::getMapHeight() {
    return _mapHeight;
}

void Heightfield::setPatchSize( float patchSize ) {
    _patchSize = patchSize;
}
float Heightfield::getPatchSize() {
    return _patchSize;
}

void Heightfield::update( float dtime ) {
    // If user is inside any tile we can do collision checking
    if ( ( 0 < _userPosition.x  && _userPosition.x < getHeightmapDimensions().x*_patchSize ) && ( 0 < _userPosition.y  && _userPosition.y < getHeightmapDimensions().y*_patchSize ) ) {

        float uX = _userPosition.x / _patchSize;
        float uY = _userPosition.y / _patchSize;
        int tileX = (int)( uX );
        int tileY = (int)( uY );
        //Color4C_16b& patch = currentTile->heightTile[ 0 ].getPixel( (int)uX, (int)uY );

        float interX = uX - tileX;
        float interY = uY - tileY;

        //std::cout << uX << " " << uY << std::endl;
        //std::cout << interX << " " << interY << std::endl;

        float height = 0.0f;
        // The heightfield data is stored in different formats for 8 and 16 bits
        // For 8 bits the order is the following
        // r  - b  - g  - a
        // 00 - 01 - 10 - 11
        if ( _bpc == HeightfieldBPC::HF_BPC_8 ) {
            Color4C patch = reinterpret_cast<Color4C*>( _compactData.getPointer() )[ tileX + tileY * (int)getHeightmapDimensions().x ];
            height = (float)mathCalcs::blerp( patch.red, patch.blue, patch.green, patch.alpha, interX, interY ) / (float)( 255.0f );
        }
        // For 16 bits the order is the following
        // a  - g  - b  - r
        // 00 - 01 - 10 - 11
        else if ( _bpc == HeightfieldBPC::HF_BPC_16 ) {
            Color4C_16b patch = reinterpret_cast<Color4C_16b*>( _compactData.getPointer() )[ tileX + tileY * (int)getHeightmapDimensions().x ];
            height = (float)mathCalcs::blerp( patch.alpha, patch.green, patch.blue, patch.red, interX, interY ) / (float)( 65535.0f );
        }

        if ( ( _userPosition.z - 2.0f ) < height * _mapHeight ) {
            _userPosition.z = height *_mapHeight + 2.01f;
        }
    }
}