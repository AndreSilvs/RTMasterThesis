#pragma once

#include <random>

class RandomIntegerGenerator {
public:
    RandomIntegerGenerator();
    ~RandomIntegerGenerator();

    int random( int min, int max );

private:
    std::mt19937 _generator;
};

class RandomFloatGenerator {
public:
    RandomFloatGenerator();
    ~RandomFloatGenerator();

    float random( float min, float max );
    float randomNormal( float average, float deviation );

private:
    std::mt19937 _generator;
};

class RandomNumberGenerator {
public:
    RandomNumberGenerator();
    ~RandomNumberGenerator();

    int randomInt( int min, int max );
    
    float randomFloat( float min, float max );
    float randomFloatNormal( float mean, float stddev );

    bool randomBool();
    bool randomBool( double chanceTrue );

private:
    std::mt19937 _generator;
    std::bernoulli_distribution _bdist;
};
