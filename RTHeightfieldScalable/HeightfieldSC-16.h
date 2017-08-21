#pragma once

#include <memory>
#include <string>
#include <map>
#include <thread>
#include <mutex>

#include "Texture.h"

#include "Vector.h"

#include "HeightTileManagement.h"

#include "Heightfield.h"

struct TileCache16 {
    int tileX, tileY;
    std::vector< PixelArray< Color4C_16b >> heightTile;
    PixelArray< Color4C > photoTile;
    std::vector<PixelArray<unsigned short>> mipmaps;
    PixelArray< Color3C > normalTile;
    int assignedIndex = -1;
    bool error = false;
    bool loaded = false;
};

/* Heightfield Scalable
- This class manages the storage of a potentially very large heightmap.

*/
class HeightfieldScalable16 : public HeightfieldScalableInterface {
public:
    HeightfieldScalable16();
    ~HeightfieldScalable16();

    bool initializeHeightmap( std::string hmapFileFormat, std::string hmapFileExtension,
        std::string ptexFileFormat, std::string ptexFileExtension,
        int numTilesX, int numTilesY, int tileSize,
        float loadFar, float mapHeight, float patchSize,
        int regionSize = 3 );

    /* Load heightfield and convert to a format where each cell contains
    the height data for all 4 neighbours in each channel RGBA
    As well as a Maximum Mipmap structure, in which each pixel in the finest level
    contains the maximum height value of the corresponding cell/patch in the compact texture.
    Every subsequent mipmap level contains the maximum height value of the previous level.
    */
    bool loadCompactFormat( const std::string& file );
    // Skip compactation step
    bool loadCompactFormatA2( int tileX, int tileY, int texTile );

    //bool preComputeNormals( int tileX, int tileY );

    bool loadPerPatchTile_16b( int tileX, int tileY, int texTile );

    bool loadPhotoTexture( const std::string &file );

    void prepareToLoadTile( int tileX, int tileY );
    bool preLoadTile( int tileX, int tileY );
    bool isTilePreLoaded( int tileX, int tileY );
    TileCache16* getTileCache( int tileX, int tileY );
    bool uploadTile( int tileX, int tileY );
    void deleteTile( int tileX, int tileY );

    /*TileManager( std::string tileNameFormat, std::string fileExtension, int nTilesX, int nTilesY, int tileSize, int regionSize );
    ~TileManager();*/

    bool unloadTile( int x, int y );
    bool loadTile( int x, int y, int texTile );

    void reloadRegion( int x, int y );
    void recenterRegion( int x, int y );

    void update( float dtime );

    void printActiveRegion();

    void closeHeightfield();

private:
    Vector3D genNormal16( PixelArray<Color4C_16b>* hmap, int ix, int iy, int tx, int ty );

    //std::vector< PixelArray<Color4C> > cachedTiles;
    std::mutex _loadMutex;
    std::condition_variable _loadSignal;

    void spawnWorkerThread();
    bool _endThread = false;

     // Tile Caching Data
    bool _loadingTiles;
    std::map< std::pair<int,int>, std::shared_ptr<TileCache16> > _tileCacheMap;
    std::vector< std::pair< int, int > > _tilesToLoad;
    std::vector< std::pair< int, int > > _tilesReady;

    std::vector<int> _availableTiles;

    int findAvailableTile();
};

