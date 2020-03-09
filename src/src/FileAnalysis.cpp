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

