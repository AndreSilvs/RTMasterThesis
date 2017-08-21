#pragma once

#include "API_Headers.h"

#include "Texture.h"

#include <vector>

class TileManager {
public:
    TileManager( std::string tileNameFormat, std::string fileExtension, int nTilesX, int nTilesY, int tileSize, int regionSize );
    ~TileManager();

    void unloadTile( int x, int y );
    void loadTile( int x, int y );

    void smartLoadTile( int x, int y );

    int getHeightmapWidth();
    int getHeightmapHeight();


public:
    // Remember which tiles are loaded (WxH: [true,false])
    std::vector<std::vector< bool >> _tileStates;
    int tileOrder[ 9 ];

    std::string _fileName;
    std::string _fileExtension;

    // Number of tiles in either direction
    int _numTilesX, _numTilesY;
    int _tileSize;
    int _centerTileX, _centerTileY;
    // How many tiles surround the center tile
    int _regionSize; // 0 = 1x1, 1 = 3x3, 2 = 5x5, etc..

    Texture _sparsePhotoTex;
    Texture _sparseHeightTex;
};

//void unloadTile( unsigned int texture, int x, int y, int numTilesX, int numTiles );