#include "HuffmanEncoding.hpp"
#include "BitStream.hpp"
#include "BinaryHeap.hpp"

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <chrono>

#define BUFFER_SZ 1024

#define COMPRESS 3
#define DECOMPRESS 2

using namespace huffman;

int main (int argc, char* argv[])
{
	(void) argc;
	(void) argv;

	std::stringstream ss_in;
	std::stringstream ss_out;

	std::string inputFile = "testFiles/hamlet2.txt";

	FILE* inFile = fopen( inputFile.c_str(), "r+" );

	using high_res_clock = std::chrono::high_resolution_clock;
	high_res_clock::time_point p1, p2, p3, p4;
	high_res_clock::duration encodingDelta, decodingDelta;

	p1 = high_res_clock::now();

	BitStream outStream( ss_out );

	struct FileAnalysis info;

	size_t totalBytes = 0;
	ssize_t nbytes;
	char buffer[ BUFFER_SZ ];

	while ( ( nbytes = fread( buffer, 1, BUFFER_SZ, inFile ) ) > 0 ) {
		totalBytes += nbytes;
		info.FeedText( buffer, nbytes );
	}
	// info.Print();

	// Write size into output first
	outStream.WriteNBits( (unsigned char*) &totalBytes, sizeof( totalBytes ) * 8 );

	// Persist huffman tree
	Tree tree = Tree::CreateNewTree( info );
	size_t bitsWritten = tree.Persist( outStream );
	(void) bitsWritten;
	// tree.Print();

	/*
	printf( "Tree encoding takes %ld bits ( %ld bytes + %ld bits )\n",
			bitsWritten, bitsWritten / 8, bitsWritten % 8 );
			*/

	// Encode every character from the input text
	fseek( inFile, 0L, SEEK_SET );
	while ( ( nbytes = fread( buffer, 1, BUFFER_SZ, inFile ) ) > 0 ) {
		totalBytes += nbytes;
		info.FeedText( buffer, nbytes );

		for ( ssize_t i = 0; i < nbytes; i++ ) {
			tree.EncodeByte( outStream, buffer[i] );
		}
	}

	outStream.Flush();

	p2 = high_res_clock::now();
	encodingDelta = p2 - p1;

	std::string encodedResult = ss_out.str();

	/*
	int n = 0;
	for ( uint8_t c : encodedResult ) {
		n++;
		printf( "%02x", c );
		if ( n % 8 == 0 ) {
			if ( n % 16 == 0 ) {
				printf( "\n" );
			} else {
				printf( "   " );
			}
		} else {
			printf( " " );
		}
	}
	printf( "\n" );
	*/

	p3 = high_res_clock::now();

	// Now, let's decode it
	//
	BitStream inStream( ss_out );

	// Read size
	size_t nBytesIn;
	inStream.ReadNBits( (unsigned char*) &nBytesIn, sizeof( nBytesIn ) * 8 );

	Tree t = Tree::LoadTree( inStream );

	try {
		while ( nBytesIn ) {
			ss_in << tree.DecodeByte( inStream );
			nBytesIn--;
		}
	} catch ( ... ) {
		printf( "Caught exception with %ld bytes to go...", nBytesIn );
	}

	p4 = high_res_clock::now();
	decodingDelta = p4 - p3;

	std::string decodedString = ss_in.str();
	printf( "Decoded string: %s\n", decodedString.c_str() );

	size_t decLen = decodedString.size();
	size_t encLen = encodedResult.size();
	printf( "Decoded string length: %ld\n", decLen );
	printf( "Encoded string length: %ld\n", encLen );

	double compressionRatio = 1.0 - ((double) encLen / decLen );
	printf( "Compression ratio: %lf%%\n", (compressionRatio) * 100 );

	size_t encTimeMs = std::chrono::duration_cast< std::chrono::microseconds >(
			encodingDelta ).count();
	size_t decTimeMs = std::chrono::duration_cast< std::chrono::microseconds >(
			encodingDelta ).count();

	printf( "\n" );
	printf( "Encoding time: %ld us\n", encTimeMs );
	printf( "Decoding time: %ld us\n", decTimeMs );

	double encSpeed = ( (double) encLen / ((double) encTimeMs / 1000.0 ) );
	double decSpeed = ( (double) decLen / ((double) decTimeMs / 1000.0 ) );
	printf( "Encoding speed: %.2lf kb/s\n", encSpeed );
	printf( "Decoding speed: %.2lf kb/s\n", decSpeed );
}

