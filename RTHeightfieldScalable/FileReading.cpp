#include "FileReading.h"

#include <cctype>
#include <ios>

namespace {
	bool iswordchar( int ch ) {
		return isalnum(ch) || ch == '-' || ch == '_';
	}
};

bool readFileIntoString( const std::string &fileName, std::string &destination ) {
    std::ifstream in( fileName, std::ios::in | std::ios::binary );
    if ( in ) {
        in.seekg( 0, std::ios::end );
        destination.resize( in.tellg() );
        in.seekg( 0, std::ios::beg );
        in.read( &destination[ 0 ], destination.size() );
        in.close();
        return true;
    }
    return false;
}

bool readExactString( std::ifstream &file, const std::string &expectation ) {
	for ( int i = 0, iEnd = (int)expectation.size(); i < iEnd; ++i ) {
		if ( ( file.peek() != expectation[i] ) || file.eof() ) {
			return false;
		}
		file.get();
	}
	return true;
}
bool readExactChar( std::ifstream &file, char c ) {
	if ( ( (char)file.peek() != c ) || file.eof() ) {
		return false;
	}
	file.get();
	return true;
}

bool isNextChar( std::ifstream &file, char c ) {
	return ( (char)file.peek() == c && !file.eof() );
}

char peekNextChar( std::ifstream &file, int positions  ) {
	auto pos = file.tellg();
	
	char ch = file.peek();
	while ( positions > 0 ) {
		positions--;
		ch = file.get();
	}

	file.seekg( pos );
	return ch;
}


std::string readWord( std::ifstream &file ) {
	std::string word;
	while ( ::iswordchar( file.peek() ) ) {
		word += (char)file.get();
	}
	return word;
}

std::string readSegment( std::ifstream &file ) {
	std::string segment;
	while ( !isspace( file.peek() ) ) {
		segment += file.get();
	}
	file.get();
	return segment;
}

void ignoreWordCharacters( std::ifstream &file ) {
	while ( ::iswordchar( file.peek() ) && !file.eof() ) {
		file.get();
	}
}
void ignoreNonWordCharacters( std::ifstream &file ) {
	while ( !::iswordchar( file.peek() ) && !file.eof() ) {
		file.get();
	}
}

void ignoreAllWords( std::ifstream &file ) {
	while ( ( ::iswordchar( file.peek() ) || file.peek() == ' ' ) && !file.eof() ) {
		file.get();
	}
}

void ignoreUntilNextWord( std::ifstream &file ) {
	// State 1 = ignore current word, if there is any
	ignoreWordCharacters( file );
	// State 2 = ignore non word chars until first word char is found
	ignoreNonWordCharacters( file );
}

void ignoreFileWhitespace( std::ifstream &file ) {
	while ( isspace( file.peek() ) ) {
		file.get();
	}
}
void ignoreFileLine( std::ifstream &file ) {
	while ( file.peek() != '\n' ) {
		file.get();
	}
	file.get();
}