#include "HuffmanEncoding.hpp"
#include "BitStream.hpp"
#include "BinaryHeap.hpp"

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <chrono>
#include <iostream>
#include <fstream>

#define BUFFER_SZ 1024 * 64   // 64 Kbs

#define COMPRESS 3
#define DECOMPRESS 2

using namespace huffman;

std::streampos SimpleCompress( const std::string& inputFile,
		const std::string& outputFile )
{
	std::ofstream outFile( outputFile );
	struct FileAnalysis info;
	char buffer[ BUFFER_SZ ];
	size_t totalBytes = 0;

	std::ifstream inFile( inputFile );

	if ( ! inFile.is_open() ) {
		printf( "Unable to open input file\n" );
		return -1;
	} else if ( ! outFile.is_open() ) {
		printf( "Unable to open output file\n" );
		return -1;
	}

	do {
		inFile.read( buffer, BUFFER_SZ );
		if ( inFile.gcount() <= 0 ) {
			break;
		}
		totalBytes += inFile.gcount();
		info.FeedText( buffer, BUFFER_SZ );
	} while ( true );
	// info.Print();

	inFile.clear();
	inFile.seekg( 0, std::ios::beg );

	// Write length of decoded data into encoded file
	OutBitStream<std::ofstream> outStream( std::move( outFile ) );
	outStream.WriteNBits( (unsigned char*) &totalBytes, sizeof( totalBytes ) * 8 );

	// Persist huffman tree
	Tree tree = Tree::CreateNewTree( info );
	size_t bitsWritten = tree.Persist( outStream );
	// tree.Print();

	printf( "Tree encoding takes %ld bits ( %ld bytes + %ld bits )\n",
			bitsWritten, bitsWritten / 8, bitsWritten % 8 );

	// Encode file
	do {
		inFile.read( buffer, BUFFER_SZ );
		std::streamsize nBytes = inFile.gcount();
		if ( nBytes <= 0 ) {
			break;
		}

		for ( int i = 0; i < nBytes; i++ ) {
			tree.EncodeByte( outStream, buffer[i] );
		}
	} while ( true );

	outStream.Flush();

	return outFile.tellp();
}

std::streampos SimpleDecompress( const std::string& inputFile,
		const std::string& outputFile )
{
	std::ifstream inFile( inputFile );
	std::ofstream outFile( outputFile );

	if ( ! inFile.is_open() ) {
		printf( "Unable to open input file\n" );
		return -1;
	} else if ( ! outFile.is_open() ) {
		printf( "Unable to open output file\n" );
		return -1;
	}

	InBitStream<std::ifstream> inStream( std::move(inFile) );

	size_t nBytesIn;
	inStream.ReadNBits( &nBytesIn, sizeof( nBytesIn ) * 8 );

	Tree t = Tree::LoadTree( inStream );
	// t.Print();

	try {
		while ( nBytesIn ) {
			outFile << (unsigned char) t.DecodeByte( inStream );
			nBytesIn--;
		}
	} catch ( ... ) {
		printf( "Caught exception with %ld bytes to go...", nBytesIn );
	}

	return outFile.tellp();
}

int main (int argc, char* argv[])
{
	(void) argc;
	(void) argv;

	using high_res_clock = std::chrono::high_resolution_clock;
	high_res_clock::time_point p1, p2, p3;
	high_res_clock::duration encodingDelta, decodingDelta;

	std::string inputFile = "testFiles/hamlet2.txt";
	std::string outFile =   "testFiles/hamlet2.txt.encrypted";
	std::string outFile2 =  "testFiles/hamlet2.txt.decrypted";

	p1 = high_res_clock::now();
	size_t encLen = SimpleCompress( inputFile, outFile );
	p2 = high_res_clock::now();
	size_t decLen = SimpleDecompress( outFile, outFile2 );
	p3 = high_res_clock::now();

	encodingDelta = p2 - p1;
	decodingDelta = p3 - p2;

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

