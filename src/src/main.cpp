#include "HuffmanEncoding.hpp"
#include "BitStream.hpp"
#include "BinaryHeap.hpp"

#include <stdio.h>
#include <string.h>
#include <stdint.h>

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

	std::string inputText = "HAMLET\n"
		"To be, or not to be: that is the question:\n"
		"Whether 'tis nobler in the mind to suffer\n"
		"The slings and arrows of outrageous fortune,\n"
		"Or to take arms against a sea of troubles,\n"
	/*
		"And by opposing end them? To die: to sleep;\n"
		"No more; and by a sleep to say we end\n"
		"The heart-ache and the thousand natural shocks\n"
		"That flesh is heir to, 'tis a consummation\n"
		"Devoutly to be wish'd. To die, to sleep;\n"
		"To sleep: perchance to dream: ay, there's the rub;\n"
		"For in that sleep of death what dreams may come\n"
		"When we have shuffled off this mortal coil,\n"
		"Must give us pause: there's the respect\n"
		"That makes calamity of so long life;\n"
		"For who would bear the whips and scorns of time,\n"
		"The oppressor's wrong, the proud man's contumely,\n"
		"The pangs of despised love, the law's delay,\n"
		"The insolence of office and the spurns\n"
		"That patient merit of the unworthy takes,\n"
		"When he himself might his quietus make\n"
		"With a bare bodkin? who would fardels bear,\n"
		"To grunt and sweat under a weary life,\n"
		"But that the dread of something after death,\n"
		"The undiscover'd country from whose bourn"
		"No traveller returns, puzzles the will\n"
		"And makes us rather bear those ills we have\n"
		"Than fly to others that we know not of?\n"
		"Thus conscience does make cowards of us all;\n"
		"And thus the native hue of resolution"\n"
		*/
		;

	BitStream outStream( ss_out );

	struct FileAnalysis info;
	info.FeedText( inputText.c_str(), inputText.size() );
	info.Print();

	// Write size into output first
	size_t nBytes = inputText.size();
	outStream.WriteNBits( (const char*) &nBytes, sizeof( nBytes ) * 8 );

	// Persist huffman tree
	Tree tree = Tree::CreateNewTree( info );
	size_t bitsWritten = tree.Persist( outStream );
	tree.Print();

	printf( "Tree encoding takes %ld bits ( %ld bytes + %ld bits )\n",
			bitsWritten, bitsWritten / 8, bitsWritten % 8 );

	for ( int i = 0; i < 16 * 8; i++ ) {
		outStream.WriteBit( false );
	}

	// Encode every character from the input text
	for ( char c : inputText ) {
		tree.EncodeByte( outStream, c );
	}

	outStream.Flush();


	std::string encodedResult = ss_out.str();

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


	// Now, let's decode it
	//
	BitStream inStream( ss_out );

	// Read size
	size_t nBytesIn;
	inStream.ReadNBits( (char*) &nBytesIn, sizeof( nBytesIn ) * 8 );

	Tree t = Tree::LoadTree( inStream );

	for ( int i = 0; i < 16 * 8; i++ ) {
		bool bit;
		inStream.ReadBit( bit );
	}

	try {
		while ( nBytesIn ) {
			ss_in << t.DecodeByte( inStream );
			nBytesIn--;
		}
	} catch ( ... ) {
		printf( "Caught exception with %ld bytes to go...", nBytesIn );
	}

	std::string decodedString = ss_in.str();
	printf( "Decoded string: %s\n", decodedString.c_str() );

	if ( t == tree ) {
		printf( "Both trees have the same encoding!\n" );
	} else {
		printf( "The trees are different...\n" );
	}
}

