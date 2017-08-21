#pragma once

#include "PixelArray.h"

template <class T > void invertImageY( T * pixels, int width, int height ) {
    int size = width * height;
    int targetIndex = 0;

    for ( int i = 0; i < size / 2; ++i ) {
        int px = i % width;
        int py = i / width;
        T temp = pixels[ i ];

        targetIndex = size - ( py + 1 ) * width + px;

        pixels[ i ] = pixels[ targetIndex ];
        pixels[ targetIndex ] = temp;
    }
}

// Helper functions
//GLuint * enlargeImage( GLuint *oldPixels, int width, int height, int newWidth, int newHeight );
