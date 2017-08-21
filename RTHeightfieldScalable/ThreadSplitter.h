#pragma once

#include <functional>

class ThreadSplitter {
public:
    static void threadize( std::function< void( int, int ) > threadableFunc, int iterations, int numThreads );

    static void threadizeSimple( std::function< void( int ) > threadableFunc, int iterations, int numThreads );

    static void threadizeDual( std::function< void( int, int, int, int ) > threadableFunc, int iterationsX, int iterationsY, int numThreads );

    static void threadizeDualProgress( std::function< void( int, int ) > threadableFunc, int iterationsX, int iterationsY, int numThreads );

private:
    static void threadizeSimpleLoop( std::function< void( int ) > threadableFunc, int a, int b );
    static void threadizeDualLoop( std::function< void( int, int ) > threadableFunc, int xa, int xb, int ya, int yb );
};