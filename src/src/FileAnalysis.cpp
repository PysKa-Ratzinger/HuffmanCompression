#include "FileAnalysis.hpp"

#include <unistd.h>

#define BUFFER_SZ 256

FileAnalysis::FileAnalysis( ) { }

void
FileAnalysis::FeedText( const char* msg, size_t msgSize )
{
	for ( size_t i = 0; i < msgSize; i++ ) {
		frequency[ msg[i] ]++;
	}

	total += msgSize;
}

FileAnalysis::FileAnalysis(
		  std::array< size_t, TOTAL_CHARS > predeterminedFreqs )
		: frequency( predeterminedFreqs )
{
	for ( size_t n : frequency ) {
		total += n;
	}
}

void FileAnalysis::Print() const
{
	for ( size_t i = 0; i < 256; i++ ) {
		size_t freq = this->frequency[i];
		if ( freq > 0 ) {
			printf( "Character 0x%02x ( %c ) appears %ld times.\n",
					(uint8_t) i, (char) i, this->frequency[i] );
		}
	}
}

