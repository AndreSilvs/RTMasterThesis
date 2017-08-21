#include "HeightTileManagement.h"

#include "MathCalcs.h"

TileManager::TileManager( std::string tileNameFormat, std::string fileExtension, int nTilesX, int nTilesY, int tileSize, int regionSize ) : 
    _fileName( tileNameFormat ), _fileExtension( fileExtension ), _numTilesX( nTilesX ), _numTilesY( nTilesY ),
    _tileSize( tileSize ), _regionSize( regionSize )
{
    // Initialize tile-load states: Not loaded by default
    _tileStates.resize( nTilesX, std::vector<bool>( nTilesY, false ) );
}
TileManager::~TileManager() {

}

void TileManager::unloadTile( int x, int y ) {
    _sparseHeightTex.bindTexture();
    glTexPageCommitmentARB( GL_TEXTURE_2D, 0, x*_tileSize, y *_tileSize, 0, _tileSize, _tileSize, 0, GL_FALSE );
    _sparseHeightTex.unbindTexture();

    _sparsePhotoTex.bindTexture();
    glTexPageCommitmentARB( GL_TEXTURE_2D, 0, x*_tileSize, y *_tileSize, 0, _tileSize, _tileSize, 0, GL_FALSE );
    _sparsePhotoTex.unbindTexture();

    // unbind sparse texture
}
void TileManager::loadTile( int x, int y ) {
    _sparseHeightTex.bindTexture();
    //glTexSubImage2D( GL_TEXTURE_2D, 0, x*_tileSize, y*_tileSize, _tileSize, _tileSize, GL_RGBA, GL_UNSIGNED_BYTE, ??? );
    glTexPageCommitmentARB( GL_TEXTURE_2D, 0, x*_tileSize, y *_tileSize, 0, _tileSize, _tileSize, 0, GL_TRUE );

    _sparseHeightTex.unbindTexture();
}

void TileManager::smartLoadTile( int nx, int ny ) {
    int px = _centerTileX;
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
    //std::cout << "pX: " << minpx << " " << maxpx << "      pY: " << minpy << " " << maxpy << std::endl;
    for ( int x = minpx; x <= maxpx; ++x ) {
        for ( int y = minpy; y <= maxpy; ++y ) {
            if ( !( ( minx <= x && x <= maxx ) && ( miny <= y && y <= maxy ) ) ) {
                if ( _tileStates[ x ][ y ] ) {
                    unloadTile( x, y );
                    _tileStates[ x ][ y ] = false;
                }
            }
        }
    }

    // Load only tiles that are outside of the old region
    //std::cout << "X: " << minx << " " << maxx << "      Y: " << miny << " " << maxy << std::endl;
    for ( int x = minx; x <= maxx; ++x ) {
        for ( int y = miny; y <= maxy; ++y ) {
            if ( !( ( minpx <= x && x <= maxpx ) && ( minpy <= y && y <= maxpy ) ) ) {
                if ( !_tileStates[ x ][ y ] ) {
                    loadTile( x, y );
                    _tileStates[ x ][ y ] = true;
                }
            }
        }
    }
}

int TileManager::getHeightmapWidth() {
    return _numTilesX * _tileSize;
}
int TileManager::getHeightmapHeight() {
    return _numTilesY * _tileSize;
}