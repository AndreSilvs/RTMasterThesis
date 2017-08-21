#include "RNG.h"

#include <iostream>

RandomIntegerGenerator::RandomIntegerGenerator() :
    _generator( std::random_device()() )
{
}
RandomIntegerGenerator::~RandomIntegerGenerator() {

}

int RandomIntegerGenerator::random( int min, int max ) {
    std::uniform_int_distribution<int> dist( min, max );
    return dist( _generator );
}


RandomFloatGenerator::RandomFloatGenerator() :
   _generator( std::random_device()() )
{

}
RandomFloatGenerator::~RandomFloatGenerator() {

}

float RandomFloatGenerator::random( float min, float max ) {
    std::uniform_real_distribution<float> dist( 0.0f, std::nextafter( 1.0f, FLT_MAX ) );
    return min + ( max - min ) * dist( _generator );
}
float RandomFloatGenerator::randomNormal( float mean, float deviation ) {
    std::normal_distribution< float > dist{ mean, deviation };
    return dist( _generator );
}




RandomNumberGenerator::RandomNumberGenerator() :
    _generator( std::random_device()( ) )
{
}
RandomNumberGenerator::~RandomNumberGenerator() {

}

int RandomNumberGenerator::randomInt( int min, int max ) {
    std::uniform_int_distribution<int> dist( min, max );
    return dist( _generator );
}
float RandomNumberGenerator::randomFloat( float min, float max ) {
    std::uniform_real_distribution<float> dist( 0.0f, std::nextafter( 1.0f, FLT_MAX ) );
    return min + ( max - min ) * dist( _generator );
}
float RandomNumberGenerator::randomFloatNormal( float mean, float stddev ) {
    std::normal_distribution< float > dist( mean, stddev );
    return dist( _generator );
}
bool RandomNumberGenerator::randomBool() {
    return _bdist( _generator );
}
bool RandomNumberGenerator::randomBool( double chanceTrue ) {
    std::bernoulli_distribution dist( chanceTrue );
    return dist( _generator );
}