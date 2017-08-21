#pragma once

#include <string>


#include "Vector.h"

#include "Texture.h"

#include "HeightTileManagement.h"

class HeightfieldScalableInterface {
public:

    static Color3C convertNormalToPixel( Vector3D norm );
    // Check if minV <= v <= maxV
    static bool validInterval( int v, int minV, int maxV );

    virtual bool initializeHeightmap( std::string hmapFileFormat, std::string hmapFileExtension,
        std::string ptexFileFormat, std::string ptexFileExtension,
        int numTilesX, int numTilesY, int tileSize,
        float loadFar, float mapHeight, float patchSize,
        int regionSize = 3 ) = 0;

    int getHeightmapWidth();
    int getHeightmapHeight();

    float getPatchSize();

    //Texture *getHeightmapTexture();
    Texture *getCompactHeightmap();
    Texture *getMaximumMipmap();
    Texture *getPhotoTexture();
    Texture *getNormalTexture();

    // Max mip level is the number of mipmaps that can be generated for a single tile
    int getMaxMipLevel() const;
    // The max mip map level that can be generated for the entire heightmap
    int getLastMipLevel() const;

    Vector3D& getUserPosition();
    virtual void update( float dtime ) = 0;

    int getTileOrder( int index );
    int* getTileOrderArray();

    int getLowerLeftX();
    int getLowerLeftY();

    Vector2D getFinalTileDimensions();
    Vector2D getTileDimensions();
    Vector2D getBounds();
    Vector2D getPointLL();

    virtual void printActiveRegion();

    virtual void closeHeightfield() = 0;

protected:
    Texture _hfCompact;
    Texture _photoTexture;
    Texture _maximumMipmaps;
    Texture _normalMap;

    int _lastLevel;
    int _maxMipmapLevel;

    Vector3D userPosition;

    // Bounds"LowerLeft" and Bounds"UpperRight"
    Vector2D boundsLL, boundsDIM;

    std::string _hFileName;
    std::string _hFileExtension;

    // Number of tiles in either direction
    int _hNumTilesX, _hNumTilesY;
    int _hTileSize;
    int _llTileX, _llTileY;
    // How many tiles surround the center tile
    int _regionSize; // 0 = 1x1, 1 = 3x3, 2 = 5x5, etc..

    float _patchSize;
    float _mapHeight;
    float _farz;

    int _normalW, _normalH;

    Vector2D _tileDimensions;
    std::vector<std::vector< int >> _tileStates;
    std::vector<int> _tileOrder;
    int _nextTile;

    // Phototexture data
    std::string _pFileName;
    std::string _pFileExtension;

    std::string getHeighTileFilename( int tileX, int tileY );
    std::string getPhotoTileFilename( int tileX, int tileY );
    
};
