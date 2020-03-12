#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <sstream>

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
class IBitStream
{
public:
	virtual void    WriteNBits( const unsigned char* buffer, size_t numBits ) = 0;
	virtual void    WriteBit( bool bit ) = 0;
	virtual bool    ReadBit( bool& bit ) = 0;
	virtual size_t  ReadNBits( unsigned char* buffer, size_t numBits ) = 0;
	virtual void    Flush() = 0;

	void WriteNBits( const char* buffer, size_t numBits ) {
		return WriteNBits( (const unsigned char*) buffer, numBits );
	}

	void WriteNBits( const std::string& result ) {
		return WriteNBits( (const unsigned char*) result.c_str(),
				result.size() * 8 );
	}

	size_t ReadNBits( char* buffer, size_t numBits ) {
		return ReadNBits( (unsigned char*) buffer, numBits );
	}
};

class BitStream : public IBitStream
{
public:
	BitStream( std::stringstream& ss );

	void    WriteNBits( const unsigned char* buffer, size_t numBits ) override;
	void    WriteBit( bool bit ) override;
	bool    ReadBit( bool& bit ) override;
	size_t  ReadNBits( unsigned char* buffer, size_t numBits ) override;
	void    Flush() override;

private:
	std::stringstream&  oss;
	uint8_t             charInBuffer;
	uint8_t             charOutBuffer;
	size_t              charInBufferSize;
	size_t              charOutBufferSize;
};

class NaiveBitStream : public IBitStream
{
public:
	NaiveBitStream( std::stringstream& ss ) : oss ( ss ) { }

	void    WriteNBits( const unsigned char* buffer, size_t numBits ) override;
	void    WriteBit( bool bit ) override;
	bool    ReadBit( bool& bit ) override;
	size_t  ReadNBits( unsigned char* buffer, size_t numBits ) override;
	void    Flush() override;

private:
	std::stringstream&  oss;
};

