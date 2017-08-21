#pragma once

#include <vector>

template < class T >
class Grid2D {
public:
    Grid2D( int sizeX, int sizeY, const T& def_value = T{} ) :
        _sizeX( sizeX ), _sizeY( sizeY ),
        _grid( sizeX * sizeY, def_value )
    {
    }
    Grid2D( const Grid2D &grid ) :
        _sizeX( grid._sizeX ), _sizeY( grid._sizeY ),
        _grid( grid._grid )
    {
    }
    Grid2D( Grid2D &&moveGrid ) :
        _sizeX( moveGrid._sizeX ), _sizeY( moveGrid._sizeY ),
        _grid( std::move( moveGrid._grid ) )
    {
        moveGrid._sizeX = 0;
        moveGrid._sizeY = 0;
        moveGrid._grid.clear();
    }
    ~Grid2D() {
        
    }

    int getSizeX() { return _sizeX; }
    int getSizeY() { return _sizeY; }

    void setCell( const T& value, int x, int y ) {
        _grid[ x * _sizeY + y ] = value;
    }
    const T& getCell( int x, int y ) {
        return _grid[ x * _sizeY + y ];
    }

    T* operator[]( int i ) {
        return &_grid[ i * _sizeY ];
    }

    int getX( int positionX, int tileSizeX ) {
        return positionX / tileSizeX;
    }
    int getY( int positionY, int tileSizeY ) {
        return positionY / tileSizeY;
    }

    int getXGrid( float positionX, float gridWidth ) {
        return (int)( positionX / ( gridWidth / _sizeX ) );
    }
    int getYGrid( float positionY, float gridHeight ) {
        return (int)( positionY / ( gridHeight / _sizeY ) );
    }

private:
    int _sizeX, _sizeY;
    std::vector< T > _grid;
};