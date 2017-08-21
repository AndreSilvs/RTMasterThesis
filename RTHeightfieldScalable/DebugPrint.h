#pragma once

#include <iostream>
#include <string>

#include "StringManipulation.h"

template < class T >
void printMemory( const T &arr ) {
    const unsigned char *ptr = reinterpret_cast< const unsigned char* >( &arr );
    for ( int i = 0; i < sizeof( T ); ++i ) {
        std::cout << (int)ptr[ i ] << ' ';
    }
    std::cout << std::endl;
}
template < class T >
void printMemoryHex( const T &arr ) {
    const unsigned char *ptr = reinterpret_cast< const unsigned char* >( &arr );
    for ( int i = 0; i < sizeof( T ); ++i ) {
        std::cout << convertToHex( ptr[ i ] ) << ' ';
    }
    std::cout << std::endl;
}

template < class T >
void printArray( const T *arr, int size ) {
    for ( int i = 0; i < size; ++i ) {
        std::cout << arr[ i ] << ' ';
    }
    std::cout << std::endl;
}

template < class T >
void printArrayAsTable( const T *arr, int size, int rows ) {
    int i = 0;
    for ( i = 0; i < size; ++i ) {
        int nextNumber = i + 1; // Just so you don't mix up the index
        if ( ( nextNumber % rows ) == 0 ) {
            std::cout << arr[ i ] << std::endl;
        }
        else {
            std::cout << arr[ i ] << ' ';
        }
    }
    if ( i % rows != 0 ) {
        std::cout << std::endl;
    }
}

void newLine();