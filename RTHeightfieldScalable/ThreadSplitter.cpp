#include "ThreadSplitter.h"

#include <thread>
#include <vector>

void ThreadSplitter::threadize( std::function< void( int, int ) > threadableFunc, int iterations, int numThreads ) {
    std::vector< std::thread > vThreads;

    for ( int i = 0; i < numThreads; ++i ) {
        vThreads.emplace_back( threadableFunc, i * iterations / numThreads, ( i + 1 ) * iterations / numThreads );
    }

    for ( int i = 0; i < numThreads; ++i ) {
        vThreads[ i ].join();
    }
}

void ThreadSplitter::threadizeSimple( std::function< void( int ) > threadableFunc, int iterations, int numThreads ) {
    std::vector< std::thread > vThreads;

    //int iterationsPerThread = ( iterations / numThreads );
    for ( int i = 0; i < numThreads; ++i ) {
        vThreads.emplace_back( &ThreadSplitter::threadizeSimpleLoop, std::ref( threadableFunc ), i * iterations / numThreads, ( i + 1 ) * iterations / numThreads );
    }

    for ( int i = 0; i < numThreads; ++i ) {
        vThreads[ i ].join();
    }
}

void ThreadSplitter::threadizeDual( std::function< void( int, int, int, int ) > threadableFunc, int iterationsX, int iterationsY, int numThreads ) {
    std::vector< std::thread > vThreads;

    for ( int i = 0; i < numThreads; ++i ) {
        vThreads.emplace_back( threadableFunc,
            i * iterationsX / numThreads, ( i + 1 ) * iterationsX / numThreads,
            0, iterationsY );
    }

    for ( int i = 0; i < numThreads; ++i ) {
        vThreads[ i ].join();
    }
}

void ThreadSplitter::threadizeSimpleLoop( std::function< void( int ) > threadableFunc, int a, int b ) {
    for ( int i = a; i < b; ++i ) {
        threadableFunc( i );
    }
}


void ThreadSplitter::threadizeDualProgress( std::function< void( int, int ) > threadableFunc, int iterationsX, int iterationsY, int numThreads ) {
    std::vector< std::thread > vThreads;

    for ( int i = 0; i < numThreads; ++i ) {
        vThreads.emplace_back( &ThreadSplitter::threadizeDualLoop, std::ref( threadableFunc ),
            i * iterationsX / numThreads, ( i + 1 ) * iterationsX / numThreads,
            0, iterationsY );
    }

    for ( int i = 0; i < numThreads; ++i ) {
        vThreads[ i ].join();
    }
}

void ThreadSplitter::threadizeDualLoop( std::function< void( int, int ) > threadableFunc, int xa, int xb, int ya, int yb ) {
    for ( int x = xa; x < xb; ++x ) {
        for ( int y = ya; y < yb; ++y ) {
            threadableFunc( x, y );
        }
    }
}