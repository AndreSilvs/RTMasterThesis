#ifndef FILE_READING_HEADER
#define FILE_READING_HEADER

/** GLOBAL	HEADERS **/
#include <fstream>
#include <string>

bool readFileIntoString( const std::string &fileName, std::string &destination );

// Read the file and return true if the expected word is found, false otherwise
bool readExactString( std::ifstream &file, const std::string &expected );
bool readExactChar( std::ifstream &file, char c );

// Check if next char matches expected, does not remove characters from the file
bool isNextChar( std::ifstream &file, char c );

// Peek the nth character from the current position
char peekNextChar( std::ifstream &file, int positions = 1 );

// Read a single word from the file ( word chars: alphanumeric, hiphen and underscore )
std::string readWord( std::ifstream &file );

// Read until whitespace
std::string readSegment( std::ifstream &file );

// Read any type from the file. Returns the value.
template< typename T > T readBinaryValue( std::ifstream &file ) {
	T obj;
	file.read( reinterpret_cast< char* >( &obj ), sizeof( T ) );
	return obj;
}

// Read any type from the file. Reference must be passed. Returns true on success, false on failure.
template< typename T > bool readBinaryValue( std::ifstream &file, T &obj ) {
	return file.read( reinterpret_cast< char* >( &obj ), sizeof( T ) ).good();
}

void ignoreWordCharacters( std::ifstream &file );
void ignoreAllWords( std::ifstream &file );
void ignoreNonWordCharacters( std::ifstream &file );
void ignoreUntilNextWord( std::ifstream &file );
void ignoreFileWhitespace( std::ifstream &file );
void ignoreFileLine( std::ifstream &file );

#endif