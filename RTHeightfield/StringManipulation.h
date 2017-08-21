#pragma once

#include <vector>
#include <string>
#include <functional>

std::vector< std::string > &splitString( std::vector< std::string > &elements, const std::string &str, char delimiter );

std::string quote( const std::string& str );

void removeCharacter( std::string &str, char c );
void removeCharacters( std::string &str, const std::string &anyOf );

void replaceCharacter( std::string &str, char oldChar, char newChar );
void replaceCharacters( std::string &str, const std::string &anyOf, char newChar );

bool isHexadecimalColorRGB( const std::string &str );
bool isHexadecimalColorRGBA( const std::string &str );

unsigned int convertHexString( const std::string &str );

std::string convertToHexRGB( unsigned int value );
std::string convertToHexRGBA( unsigned int value );
std::string convertToHex( unsigned char value );

bool isHexChar( char c );

std::string boolToString( bool value );
std::string intToString( int i );
std::string floatToString( float f, int decimals = 3 );
std::string doubleToString( double f, int decimals = 3 );


std::string getRawFileName( const std::string &fileName, bool keepExtension = false );
std::string addFileExtenstion( const std::string &fileName, const std::string &extenstion );
