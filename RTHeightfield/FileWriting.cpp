#include "FileWriting.h"

#include <cctype>
#include <ios>

void writeChar( std::ofstream &file, char c, int times ) {
	while ( times > 0 ) {
		file << c;
		--times;
	}
}
