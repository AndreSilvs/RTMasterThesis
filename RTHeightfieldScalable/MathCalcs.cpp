#include "MathCalcs.h"

#include <iostream>
#include <cmath>

namespace mathCalcs {
	
	bool isEven( int value ) {
		return (value & 1) == 0;
	}
	bool isOdd( int value ) {
		return (value & 1) == 1;
	}

	int powerOf( int base, int exponent ) {
		int res = base;
		if ( exponent <= 0 ) {
			res /= base;
		}
		else {
			for( int i = 1; i < exponent; i++ )
				res *= base;
		}

		return res;
	}

	int round( float value ) {
		return (int)floor( value + 0.5f );
	}
	int round( double value ) {
		return (int)floor( value + 0.5 );
	}

	int sign( int value ) {
		if ( value == 0 ) return 0;
		return ( value < 0 ? -1 : 1 );
	}
	int sign( float value ) {
		if ( value == 0 ) return 0;
		return ( value < 0 ? -1 : 1 );
	}
	int sign( double value ) {
		if ( value == 0 ) return 0;
		return ( value < 0 ? -1 : 1 );
	}

	int max( int a, int b ) {
		return ( a > b ? a : b );
	}
    unsigned short max( unsigned short a, unsigned short b ) {
        return ( a > b ? a : b );
    }
	float max( float a, float b ) {
		return ( a > b ? a : b );
	}
	double max( double a, double b ) {
		return ( a > b ? a : b);
	}
		
	int min( int a, int b ) {
		return ( a < b ? a : b );
	}
	float min( float a, float b ) {
		return ( a < b ? a : b );
	}
	double min( double a, double b ) {
		return ( a < b ? a : b );
	}

	bool isInRange( int value, int lower, int upper ) {
		return lower < value && value < upper;
	}
	bool isInRange( float value, float lower, float upper ) {
		return lower < value && value < upper;
	}
	bool isInRange( double value, double lower, double upper ) {
		return lower < value && value < upper;
	}
	
	bool isInRangeInclusive( int value, int lower, int upper ) {
		return lower <= value && value <= upper;
	}
	bool isInRangeInclusive( float value, float lower, float upper ) {
		return lower <= value && value <= upper;
	}
	bool isInRangeInclusive( double value, double lower, double upper ) {
		return lower <= value && value <= upper;
	}

    bool withinBox( float tx, float ty, float cx, float cy, float width, float height ) {
        width = width / 2;
        height = height / 2;
        return ( ( ( cx - width ) <= tx ) && ( tx <= ( cx + width ) ) ) &&
            ( ( ( cy - height ) <= ty ) && ( ty <= ( cy + height ) ) );
    }

    bool withinRadius( float tx, float ty, float cx, float cy, float radius ) {
        float sqx = ( tx - cx ); sqx *= sqx;
        float sqy = ( ty - cy ); sqy *= sqy;
        return ( sqx + sqy ) <= radius*radius;
    }

	int randomInteger( int lower, int upper ) {
		return rand() % ( upper - lower + 1 ) + lower;
	}

	float randomFloat( float lower, float upper ) {
		return lower + (float)rand() / ( (float)RAND_MAX / ( upper - lower) );
	}

	float floatMod( float value, int div ) {
		float mod = value - (float)(( ((int)value) / div ) * div );
		return ( mod > 0 ? mod : div + mod );
	}
    float floatMod( float value, float div ) {
        float r = value / div;
        float frac = r - (float)( (int)r );

        return ( r > 0 ? frac * div : div + div * frac );
    }



	float loopInRange( float value, int lower, int upper ) {
		return floatMod( value + lower, upper - lower ) + lower;
	}
    float loopInRange( float value, float lower, float upper ) {
        return fmod( value - lower, upper - lower ) + lower;
    }

	float loopMirrorInRange( float value, int lower, int upper ) {
		float loopValue = loopInRange( value, lower, upper );
		int div = (int)(abs( value - lower ) / ( upper - lower ));

		if ( isOdd( div ) ) {
			loopValue = upper - loopValue + lower;
		}

		return loopValue;
	}
	float clampToRange( float value, float lower, float upper ) {
		if ( value < lower ) return lower;
		if ( value > upper ) return upper;
		return value;
	}
	int clampToRange( int value, int lower, int upper ) {
		if ( value < lower ) return lower;
		if ( value > upper ) return upper;
		return value;
	}

	float nextMultiple( float value, float multiplier ) {
		int m = (int)( value / multiplier ) + 1;
		return multiplier * (float)m;
	}
	float previousMultiple( float value, float multiplier ) {
		int m = (int)ceil( (int)(value - multiplier) / multiplier );
		return multiplier * (float)m;
	}

    bool isEqual( float a, float b, float epsilon ) {
        return isInRange( a, b - epsilon, b + epsilon );
    }

	float degreesToRadians( float angleDegrees ) {
		return angleDegrees * (float)DEGREES_TO_RADIANS;
	}
	float radiansToDegrees( float angleRadians ) {
		return angleRadians * (float)RADIANS_TO_DEGREES;
	}

    float lerp( float a, float b, float percent ) {
        return a * percent + b * ( 1.0f - percent );
    }
    float lerpi( float a, float b, float percent ) {
        return a * ( 1.0f - percent ) + b * percent;
    }

    float approximate( float value, float target, float amount ) {
        if ( value < target ) {
            value = min( value + amount, target );
        }
        else {
            value = max( value - amount, target );
        }

        return value;
    }

    float approximateAngle( float value, float target, float amount ) {
        value = loopInRange( value, 0, 360 );
        target = loopInRange( target, 0, 360 );

        if ( abs( target - value ) > 180 ) {
            float difference = target - value;
            if ( difference > 0 ) {
                value = value + 360;
            }
            else {
                target = target + 360;
            }
        }

        return approximate( value, target, amount );
    }

    float loopSignedDifference( float value, float target, float min, float max ) {
        value = loopInRange( value, min, max );
        target = loopInRange( target, min, max );
        float diff = target - value;
        return diff > ( max - min ) / 2.0f ? diff - max :
            (diff < -( max - min ) /2.0f ? diff + max : diff);
    }
    float loopShortestDifference( float value, float target, float min, float max ) {
        value = loopInRange( value, min, max );
        target = loopInRange( target, min, max );
        float finalValue = abs( value - target );
        return finalValue <= (max-min)/2.0f ? finalValue : max - finalValue;
    }
    float loopApproximate( float value, float target, float min, float max, float delta ) {
        value = loopInRange( value, min, max );
        target = loopInRange( target, min, max );

        if ( abs( target - value ) > (max-min)/2.0f ) {
            float difference = target - value;
            if ( difference > 0 ) {
                value = value + max;
            }
            else {
                target = target + max;
            }
        }

        return approximate( value, target, delta );
    }
    float loopLerp( float value, float target, float min, float max, float rate ) {
        float diff = loopSignedDifference( value, target, min, max );
        return loopInRange( value + diff * rate, min, max );
    }
    float blerp( float p00, float p01, float p10, float p11, float x, float y ) {
        return  ( 1.0f - x ) * ( 1.0f - y ) * p00 +
            ( 1.0f - x ) *      y       * p01 +
            x       * ( 1.0f - y ) * p10 +
            x       *      y       * p11;
    }

    float translateToSymmetricRange( int value, int max ) {
        float fmin = -(float)max / 2.0f;
        float fValue = fmin + (float)value;
        return fValue;
    }

    bool isAngleGreater( float angle, float target ) {
        angle = loopInRange( angle, 0, 360 );
        target = loopInRange( target, 0, 360 );
        float diff = angle - target;
        return ( diff < 180 && diff > 0 ) || ( diff < -180 && diff > -360 );
    }
    bool isAngleLower( float angle, float target ) {
        angle = loopInRange( angle, 0, 360 );
        target = loopInRange( target, 0, 360 );
        float diff = angle - target;
        return ( diff > 180 && diff < 360 ) || ( diff > -180 && diff < 0 );
    }

    bool isAngleWithin( float angle, float counterClockwiseAngle, float clockwiseAngle ) {
        float left = counterClockwiseAngle;
        float right = clockwiseAngle;

        left -= right;
        angle -= right;
        right -= right;

        angle = loopInRange( angle, 0, 360 );

        return left > angle && angle > right;
    }

    float getShortestAngleDifference( float angleA, float angleB ) {
        angleA = loopInRange( angleA, 0, 360 );
        angleB = loopInRange( angleB, 0, 360 );
        float angle = abs( angleA - angleB );
        return angle <= 180 ? angle : 360 - angle;
    }

    int numDigits( int value ) {
        int digits = 1;
        while ( value > 10 ) {
            value /= 10;
            digits++;
        }
        return digits;
    }

    bool isFractionary( float value ) {
        return ( value != (float)( (int)value ) );
    }
    bool isFractionary( double value ) {
        return ( value != (double)( (int)value ) );
    }

    float gaussFunction( float x, float sigmaSquare ) {
        float a = 1 / sqrt( 2.0f *(float) CALC_PI * sigmaSquare );
        float b = exp( -( x*x / ( 2 * sigmaSquare ) ) );
        return a * b;
    }
}