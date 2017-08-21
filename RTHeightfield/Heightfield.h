#pragma once

#include <string>

#include "Texture.h"

#include "Vector.h"

#include "Color_16b.h"

#include "HeightfieldParameters.h"

class Heightfield {
public:
    Heightfield();
    ~Heightfield();

    /* Load heightfield and convert to a format where each cell contains
    the height data for all 4 neighbours in each channel RGBA
    As well as a Maximum Mipmap structure, in which each pixel in the finest level
	contains the maximum height value of the corresponding cell/patch in the compact texture.
	Every subsequent mipmap level contains the maximum height value of the previous level.
    */
    bool loadHeightmap_8b( const std::string& file );
    // Skip compactation step
    bool loadCompactHeightmap_8b( const std::string& file );

    bool loadHeightmap_16b( const std::string& file );

    bool loadCompactHeightmap_16b( const std::string& file );

    bool loadPhotoTexture( const std::string &file );
	// Default to a white texture
    bool loadDefaultPhotoTexture();

    bool preComputeNormals();
    bool preComputeNormals2();

    bool preComputeNormals_16b();

    Texture *getHeightmapTexture();
    Texture *getCompactHeightmap();
    Texture *getMaximumMipmap();
    Texture *getPhotoTexture();
    Texture *getNormalMap();

    int getLastMipLevel() const;

    void update( float dtime );

    Vector2D getHeightmapDimensions();
    Vector2D getRealHeightmapDimensions();

    void setMapHeight( float maxHeight );
    float getMapHeight();

    void setPatchSize( float patchSize );
    float getPatchSize();

    Vector3D _userPosition;
private:
    HeightfieldBPC _bpc;

    Texture _hftex;
    Texture _hfCompact;
    Texture _photoTexture;
    Texture _normalMap;

    Texture _maximumMipmaps;
    int _lastLevel;

    Vector2D _realDims;

    Vector3D genNormal( PixelArray<Color4C>* hmap, int ix, int iy, int tx, int ty );
    Vector3D genNormal16( PixelArray<Color4C_16b>* hmap, int ix, int iy, int tx, int ty );

    float _mapHeight;
    float _patchSize;

    ArrayWrapper< GLubyte > _compactData;
};
