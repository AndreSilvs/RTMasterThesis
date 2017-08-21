#ifndef FILE_WRITING_HEADER
#define FILE_WRITING_HEADER

/** GLOBAL	HEADERS **/
#include <fstream>

template< typename T > void writeBinaryValue( std::ofstream &file, T val ) {
	file.write( reinterpret_cast< const char* >( &val ), sizeof(T) );
}

// Formatting
void writeChar( std::ofstream &file, char c, int times = 1 );

#endif