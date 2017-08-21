#include "Heightfield.h"

#include "MathCalcs.h"

Color3C HeightfieldScalableInterface::convertNormalToPixel( Vector3D norm ) {
    return   Color3C( (unsigned char)( ( norm.x + 1.0f ) / 2.0f * 255.0f ),
        (unsigned char)( ( norm.y + 1.0f ) / 2.0f * 255.0f ),
        (unsigned char)( ( norm.z + 1.0f ) / 2.0f * 255.0f ) );
}
// Check if minV <= v <= maxV
bool HeightfieldScalableInterface::validInterval( int v, int minV, int maxV ) {
    return mathCalcs::isInRangeInclusive( v, minV, maxV );
}

int HeightfieldScalableInterface::getHeightmapWidth() {
    return _hNumTilesX * _hTileSize;
}
int HeightfieldScalableInterface::getHeightmapHeight() {
    return _hNumTilesY * _hTileSize;
}

float HeightfieldScalableInterface::getPatchSize() {
    return _patchSize;
}

//Texture *getHeightmapTexture();
Texture *HeightfieldScalableInterface::getCompactHeightmap() {
    return &_hfCompact;
}
Texture *HeightfieldScalableInterface::getMaximumMipmap() {
    return &_maximumMipmaps;
}
Texture *HeightfieldScalableInterface::getPhotoTexture() {
    return &_photoTexture;
}
Texture *HeightfieldScalableInterface::getNormalTexture() {
    return &_normalMap;
}

// Max mip level is the number of mipmaps that can be generated for a single tile
int HeightfieldScalableInterface::getMaxMipLevel() const {
    return _maxMipmapLevel;
}
// The max mip map level that can be generated for the entire heightmap
int HeightfieldScalableInterface::getLastMipLevel() const {
    return _lastLevel;
}

Vector3D& HeightfieldScalableInterface::getUserPosition() {
    return userPosition;
}

/*void HeightfieldScalableInterface::update( float dtime ) {

}*/

int HeightfieldScalableInterface::getTileOrder( int i ) {
    return _tileOrder[ i ];
}
int* HeightfieldScalableInterface::getTileOrderArray() {
    return &_tileOrder[ 0 ];
}

int HeightfieldScalableInterface::getLowerLeftX() {
    return _llTileX;
}
int HeightfieldScalableInterface::getLowerLeftY() {
    return _llTileY;
}

Vector2D HeightfieldScalableInterface::getTileDimensions() {
    return _tileDimensions;
}
Vector2D HeightfieldScalableInterface::getFinalTileDimensions() {
    return _tileDimensions * _patchSize;
}
Vector2D HeightfieldScalableInterface::getBounds() {
    return boundsDIM;
}
Vector2D HeightfieldScalableInterface::getPointLL() {
    return boundsLL;
}

void HeightfieldScalableInterface::printActiveRegion() {

}



std::string HeightfieldScalableInterface::getHeighTileFilename( int tileX, int tileY ) {
    return _hFileName + "_t" + std::to_string( tileX ) + "_" + std::to_string( tileY ) + _hFileExtension;
}
std::string HeightfieldScalableInterface::getPhotoTileFilename( int tileX, int tileY ) {
    return _pFileName + "_t" + std::to_string( tileX ) + "_" + std::to_string( tileY ) + _pFileExtension;
}