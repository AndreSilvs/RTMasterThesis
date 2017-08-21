#include "StringManipulation.h"

#include <algorithm>

#include <sstream>
#include <iomanip>

#include <iostream>

#include "Bits.h"
#include "MathCalcs.h"

std::vector< std::string > &splitString( std::vector< std::string > &elements, const std::string &str, char delimiter ) {
	std::stringstream ss( str );
    std::string item;

    while ( std::getline( ss, item, delimiter ) ) {
        elements.push_back( item );
    }

    return elements;
}

std::string quote( const std::string& str ) {
	return "\"" + str + "\"";
}

void removeCharacter( std::string &str, char c ) {
    str.erase( std::remove( str.begin(), str.end(), c ), str.end() );
}
void removeCharacters( std::string &str, const std::string &anyOf ) {
    str.erase( std::remove_if( str.begin(), str.end(), 
        [&anyOf]( char c ) {
            return anyOf.find( c ) != anyOf.npos;
        }
    ) );
}

void replaceCharacter( std::string &str, char oldChar, char newChar ) {
    std::replace( str.begin(), str.end(), oldChar, newChar );
}
void replaceCharacters( std::string &str, const std::string &anyOf, char newChar ) {
    std::replace_if( str.begin(), str.end(),
        [&anyOf]( char c ) {
            return anyOf.find( c ) != anyOf.npos;
        },
        newChar );
}

bool isHexadecimalColorRGB( const std::string &str ) {
	return str.size() == 6 && ( str.find_first_not_of( "0123456789abcdefABCDEF", 0 ) == std::string::npos );
}

bool isHexadecimalColorRGBA( const std::string &str ) {
	return str.size() == 8 && ( str.find_first_not_of( "0123456789abcdefABCDEF", 0 ) == std::string::npos );
}

/*bool isHexadecimal( const std::string &str ) {

}*/

unsigned int convertHexString( const std::string &str ) {
	std::stringstream converter( str );
	unsigned int value;
	converter >> std::hex >> value;
	
	return value;
}

std::string convertToHexRGB( unsigned int value ) {
	std::stringstream stream;
	stream << std::setfill ('0') << std::setw(sizeof(unsigned int)*2) 
			<< std::hex << value;

	std::string result = stream.str();
	result.pop_back();
	result.pop_back();

	return result;
}

std::string convertToHexRGBA( unsigned int value ) {
	std::stringstream stream;
	stream << std::setfill ('0') << std::setw(sizeof(unsigned int)*2) 
			<< std::hex << value;
	return stream.str();
}

std::string convertToHex( unsigned char value ) {
    std::stringstream stream;
    stream << std::hex << (int)value;
    return stream.str();
}

bool isHexChar( char c ) {
	std::string s = "0123456789abcdefABCDEF";
	return s.find( c ) != std::string::npos;
}



std::string boolToString( bool value ) {
    return value ? "true" : "false";
}

std::string intToString( int i ) {
    bool negative = i < 0;
    i = abs( i );

    std::string s( "" );
    do {
        s = (char)( ( i % 10 ) + '0' ) + s;
        i /= 10;
    } while ( i > 0 );

    if ( negative ) s = '-' + s;

    return s;
}

std::string floatToString( float value, int decimals ) {
    bool negative = value < 0;
    value = abs( value );

    int integerPart, fraction, decimalBase;
    decimalBase = mathCalcs::powerOf( 10, decimals );
    value = round( value * (float)decimalBase ) / (float)decimalBase;

    integerPart = (int)floor( value );
    fraction = round( ( value - (float)((int)value) ) * decimalBase );

    std::string a = intToString( integerPart );
    std::string b = intToString( fraction );

    int length = (int)b.length();

    while ( length++ < decimals ) {
        b = '0' + b;
    }

    if ( negative ) return "-" + a + "." + b;
    return a + "." + b;
}

std::string doubleToString( double value, int decimals ) {
    bool negative = value < 0;
    value = abs( value );

    int integerPart, fraction, decimalBase;
    decimalBase = mathCalcs::powerOf( 10, decimals );
    value = round( value * (double)decimalBase ) / (double)decimalBase;

    integerPart = (int)floor( value );
    fraction = round( ( value - (double)((int)value) ) * decimalBase );

    std::string a = intToString( integerPart );
    std::string b = intToString( fraction );

    int length = (int)b.length();

    while ( length++ < decimals ) {
        b = '0' + b;
    }

    if ( negative ) return "-" + a + "." + b;
    return a + "." + b;
}

std::string getRawFileName( const std::string &longFileName, bool keepExtension ) {
    auto pos = longFileName.find_last_of( '/' );
    std::string fileName;
    if ( pos != std::string::npos ) {
        fileName = longFileName.substr( pos + 1 );
    }

    if ( !keepExtension ) {
        fileName = fileName.substr( 0, fileName.find_last_of( '.' ) );
    }

    return fileName;
}

std::string addFileExtenstion( const std::string &fileName, const std::string &extension ) {
    int lengthFile = fileName.length();
    int lengthExt = extension.length();
    if ( lengthFile >= lengthExt ) {
        if ( fileName.substr( lengthFile - lengthExt ) == extension ) {
            return fileName;
        }
    }
    
    return fileName + extension;
}