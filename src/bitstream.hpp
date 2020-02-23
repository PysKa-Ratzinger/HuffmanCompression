#pragma once

#include <stdio.h>

/*
=============================================
A BitStream is a stream that reads or writes bits from or to a file.
Since the minimum size the CPU can talk to the filesystem is a char, some
trickery must be employed in order to write bits.
So, the FIRST written bits end up on the bit of highest order, while the
LAST bits are written on the bits of lower order. So, if we were writing:

  11111010101

to the filesystem, the result (in chars) would look like this:

  01011111 00000101

Notice that the last char has some space left where we could save more
bits. Those are not used and it is expected that, on reading, the user
knows when to stop reading for bits, in order to not read this padded
bits.

The reading operation is exactly the opposite, but maintains the same
structure. The FIRST bit written will be the FIRST bit read.
=============================================
 */
class BitStream{
public:
	BitStream( FILE* file );
	void writeNBits( const unsigned char* buffer, size_t numBits );
	void writeBit( unsigned char bit );
	unsigned char readBit();
	void readNBits( unsigned char* buffer, size_t numBits );
	void flush();

private:
	FILE* _file;
	char _charInBuffer;
	char _charOutBuffer;
	size_t _charInBufferSize;
	size_t _charOutBufferSize;
};

