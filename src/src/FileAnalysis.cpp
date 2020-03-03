#include "FileAnalysis.hpp"

#include <unistd.h>

#define BUFFER_SZ 256

FileAnalysis::FileAnalysis( int inFD )
{
	uint8_t buffer[BUFFER_SZ];
	ssize_t bytes_read = 0;
	while ( ( bytes_read = read( inFD, buffer, BUFFER_SZ ) ) > 0 ) {
		for ( ssize_t i = 0; i < bytes_read; i++ ) {
			frequency[ buffer[i] ]++;
		}
	}

	for ( size_t n : frequency ) {
		total += n;
	}
}

FileAnalysis::FileAnalysis(
		  std::array< size_t, TOTAL_CHARS > predeterminedFreqs )
		: frequency( predeterminedFreqs )
{
	for ( size_t n : frequency ) {
		total += n;
	}
}

