/*
** MathCalcs.h
** Auxiliary Math Functions
** version 1.0 - 08/06/2014 Este header foi gerado a mao
*/

#ifndef MATHCALCS_H
#define MATHCALCS_H

#define CALC_PI 3.14159265358979323846
#define DEGREES_TO_RADIANS CALC_PI/180.0 //degrees to radians
#define RADIANS_TO_DEGREES 180.0/CALC_PI //radians to degrees

namespace mathCalcs {

	bool isEven( int value );
	bool isOdd( int value );

	// Powerof for integers
	int powerOf( int base, int exponent );

	// Round to the nearest integer value
	int round( float value );
	int round( double value );

	// Return -1 if negative, 1 if positive, 0 if zero
	int sign( int value );
	int sign( float value );
	int sign( double value );

	// Maximum value passed
	int max( int value_a, int value_b );
    unsigned short max( unsigned short value_a, unsigned short value_b );
	float max( float value_a, float value_b );
	double max( double value_a, double value_b );
    template <typename T>
    T max( T a, T b ) {
        return ( a > b ? a : b );
    }

	// Minimum value passed
	int min( int value_a, int value_b );
	float min( float value_a, float value_b );
	double min( double value_a, double value_b );
    template <typename T>
    T min( T a, T b ) {
        return ( a < b ? a : b );
    }

	// Check if value value is within range ]lower,upper[
	bool isInRange( int value, int lower, int upper );
	bool isInRange( float value, float lower, float upper );
	bool isInRange( double value, double lower, double upper );
	
	// Check if value value is within range [lower,upper]
	bool isInRangeInclusive( int value, int lower, int upper );
	bool isInRangeInclusive( float value, float lower, float upper );
	bool isInRangeInclusive( double value, double lower, double upper );

    bool withinBox( float tx, float ty, float cx, float cy, float width, float height );
    bool withinRadius( float tx, float ty, float cx, float cy, float radius );

	// Generate random values using rand()
	int randomInteger( int lower, int upper );
	float randomFloat( float lower, float upper );

	// Module for floats
	float floatMod( float value, int div );
    float floatMod( float value, float div );
	// Loop value in the range [lowerBound,upperBound]
	float loopInRange( float value, int lower, int upper );
    float loopInRange( float value, float lower, float upper );

	// Loop value in the range [lowerBound,upperBound], mirroring the values at every endpoint
	float loopMirrorInRange( float value, int lower, int upper );
	// Clamp value to the range [min,max]
	float clampToRange( float value, float lower, float upper );
	int clampToRange( int value, int lower, int upper );

	// Returns the next multiple of multiplier higher than value
	float nextMultiple( float value, float multiplier );
	// Returns the previous multiple of multiplier lower than value
	float previousMultiple(float value, float multiplier );

    // Returns whether or not a is equal to b in the neighbourhood of epsilon
    bool isEqual( float a, float b, float epsilon = 0.001f );

	float degreesToRadians( float angleDegrees );
	float radiansToDegrees( float angleRadians );

    // Linearly interpolate between 'a' and 'b'
    // rate 0.0 = a, rate 1.0 = b
    float lerp( float a, float b, float rate );

    // Linearly interpolate between 'a' and 'b' inversely
    // rate 0.0 = b, rate 1.0 = a
    float lerpi( float a, float b, float rate );

    // Approximate value of target by amount
    float approximate( float value, float target, float amount );
    float approximateAngle( float value, float target, float amount );

    float loopSignedDifference( float value, float target, float min, float max );
    float loopShortestDifference( float value, float target, float min, float max );
    float loopApproximate( float value, float target, float min, float max, float delta );
    float loopLerp( float value, float target, float min, float max, float rate );

    float blerp( float p00, float p01, float p10, float p11, float x, float y );

    // Interpolate value from range [0,max] into [-max/2,max/2]
    float translateToSymmetricRange( int value, int max );

    bool isAngleGreater( float angle, float target );
    bool isAngleLower( float angle, float target );

    bool isAngleWithin( float angle, float counterClockwiseAngle, float clockwiseAngle );

    float getShortestAngleDifference( float angleA, float angleB );

    int numDigits( int value );
    bool isFractionary( float value );
    bool isFractionary( double value );

    float gaussFunction( float x, float sigmaSquare );
}

#endif