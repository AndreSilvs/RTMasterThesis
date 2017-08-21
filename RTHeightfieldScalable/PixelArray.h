#pragma once

template <class T> class PixelArray {
public:
    PixelArray() :
        _initialized( false ), _width( 0 ), _height( 0 ), _pixels( nullptr )
    {
    }
    ~PixelArray() {
        deletePixelArray();
    }
    PixelArray( PixelArray<T> &&mvarray ) {
        _width = mvarray._width;
        _height = mvarray._height;
        _initialized = mvarray._initialized;
        _pixels = mvarray._pixels;

        mvarray._width = mvarray._height = 0;
        mvarray._initialized = false;
        mvarray._pixels = nullptr;
    }

    bool isInitialized() const {
        return _initialized;
    }

    int sizeOfPixel() const {
        return sizeof( T );
    }

    void fill( T value ) {
        for ( int y = 0; y < _height; ++y ) {
            for ( int x = 0; x < _width; ++x ) {
                _pixels[ y * _width + x ] = value;
            }
        }
    }
    void fill( int fromX, int fromY, int sizeX, int sizeY, T value ) {
        for ( int y = fromY, yEnd = fromY + sizeY; y < yEnd; ++y ) {
            for ( int x = fromX, xEnd = fromX + sizeX; x < xEnd; ++x ) {
                _pixels[ y * _width + x ] = value;
            }
        }
    }

    void copyPixelArray( const T *pixels, int width, int height ) {
        deletePixelArray();
        createPixelArray( width, height );
        int size = width * height;
        _width = width;
        _height = height;

        //memcpy( _pixels, pixels, size * sizeOfPixel() );
        for ( int i = 0; i < size; ++i ) {
            _pixels[ i ] = pixels[ i ];
        }
    }

    void createPixelArray( int width, int height ) {
        deletePixelArray();
        int size = width * height;

        _width = width;
        _height = height;

        _pixels = new T[ size ];
        memset( _pixels, 0, size * sizeOfPixel() );
        _initialized = true;
    }
    void createPixelArrayEmpty( int width, int height ) {
        deletePixelArray();
        int size = width * height;

        _width = width;
        _height = height;

        _pixels = new T[ size ];
        _initialized = true;
    }
    void deletePixelArray() {
        if ( _initialized ) {
            _width = 0;
            _height = 0;
            delete[] _pixels;
            _pixels = nullptr;
            _initialized = false;
        }
    }
    void overwritePixelArray( const T *pixels, int width, int height, int originX, int originY ) {
        if ( isInitialized() ) {
            // Missing: Bounds check
            for ( int y = 0; y < height; ++y ) {
                for ( int x = 0; x < width; ++x ) {
                    _pixels[ ( originY + y ) * _width + ( originX + x ) ] = pixels[ y * width + x ];
                }
            }
        }
    }
    void overwritePixelArray( const PixelArray< T > &targetPixels, int originX, int originY ) {
        if ( isInitialized() && targetPixels.isInitialized() ) {
            int width = targetPixels.getWidth();
            int height = targetPixels.getHeight();
            auto pixels = targetPixels.getPixels();

            // Missing: Bounds check
            for ( int y = 0; y < height; ++y ) {
                for ( int x = 0; x < width; ++x ) {
                    _pixels[ ( originY + y ) * _width + ( originX + x ) ] = pixels[ y * width + x ];
                }
            }
        }
    }

    T *getPixels() {
        return _pixels;
    }

    const T *getPixels() const {
        return _pixels;
    }

    int getWidth() const {
        return _width;
    }

    int getHeight() const {
        return _height;
    }

    int getSize() const {
        return _width * _height;
    }

    T &getPixel( int x, int y ) {
        return _pixels[ y * _width + x ];
    }
    
    const T &getPixel( int x, int y ) const {
        return _pixels[ y * _width + x ];
    }

    T &operator[]( const unsigned int i ) {
        return _pixels[ i ];
    }

    const T &operator[]( const unsigned int i ) const {
        return _pixels[ i ];
    }

    PixelArray< T > &operator=( const PixelArray< T > &pArr ) {
        copyPixelArray( pArr.getPixels(), pArr.getWidth(), pArr.getHeight() );
        return *this;
    }

private:
    bool _initialized;
    T *_pixels;
    int _width;
    int _height;
};