#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <iostream>
#include <istream>
#include <ostream>

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
class IOutBitStream
{
public:
	virtual void    WriteNBits( const unsigned char* buffer, size_t numBits ) = 0;
	virtual void    WriteBit( bool bit ) = 0;
	virtual void    Flush() = 0;

	void WriteNBits( const char* buffer, size_t numBits ) {
		return WriteNBits( (const unsigned char*) buffer, numBits );
	}

	void WriteNBits( const std::string& result ) {
		return WriteNBits( (const unsigned char*) result.c_str(),
				result.size() * 8 );
	}
};

class IInBitStream
{
public:
	virtual bool    ReadBit( bool& bit ) = 0;
	virtual size_t  ReadNBits( void* buffer, size_t numBits ) = 0;
};

template < typename outStream_T >
class OutBitStream : public IOutBitStream
{
public:
	OutBitStream( outStream_T&& oss )
			: oss( std::forward<outStream_T>(oss) )
			, charOutBuffer( 0 )
			, charOutBufferSize( 0 )
	{ }

	void WriteNBits( const unsigned char* buffer, size_t numBits ) override
	{
		while ( numBits >= 8 ) {
			const unsigned char c = *buffer;
			for ( size_t i = 0; i < 8; i++ ) {
				bool bit = ( c >> ( 7 - i ) ) & 0x1;
				WriteBit( bit );
			}
			buffer++;
			numBits -= 8;
		}

		if ( numBits > 0 ) {
			const unsigned char c = *buffer;
			for ( size_t i = 0; i < numBits; i++ ) {
				bool bit = ( c >> ( 7 - i ) ) & 0x1;
				WriteBit( bit );
			}
		}
	}

	void WriteBit( bool bit ) override
	{
		size_t shift = 8 - charOutBufferSize - 1;
		charOutBuffer >>= shift;
		charOutBuffer |= bit;
		charOutBuffer <<= shift;
		charOutBufferSize++;
		if ( charOutBufferSize == 8 ) {
			Flush();
			charOutBuffer = '\0'; // Clean charOutBuffer
		}
	}

	void Flush() override
	{
		if ( charOutBufferSize > 0 ) {
			oss << charOutBuffer;
			charOutBufferSize = 0;
		}
	}

private:
	outStream_T&& oss;
	uint8_t       charOutBuffer;
	size_t        charOutBufferSize;
};

template < typename inStream_T >
class InBitStream : public IInBitStream
{
public:
	InBitStream( inStream_T&& iss )
		: iss( std::forward<inStream_T>(iss) )
		, charInBuffer( 0 )
		, charInBufferSize( 0 )
	{ }

	bool ReadBit( bool& bit ) override
	{
		if ( charInBufferSize == 0 ) {
			int c = iss.get();
			if ( c == EOF ) {
				return false;
			}
			charInBuffer = c;
			charInBufferSize = 8;
		}

		bit = charInBuffer & 0x80;
		charInBuffer <<= 1;
		charInBufferSize--;
		return true;
	}

	size_t  ReadNBits( void* b, size_t numBits ) override
	{
		unsigned char* buffer = static_cast<unsigned char*>( b );
		size_t bitsRead = 0;
		bool bit;

		// TODO: Optimize this code
		while ( numBits >= 8 ) {
			*buffer = '\0';
			for ( size_t i = 0; i < 8; i++ ) {
				if ( ! this->ReadBit( bit ) ) {
					return bitsRead;
				}
				*buffer |= ( static_cast<unsigned char>( bit ) << ( 7 - i ) );
				bitsRead++;
				numBits--;
			}
			buffer++;
		}

		if ( numBits > 0 ) {
			*buffer = '\0';
			for ( size_t i = 0; i < numBits; i++ ) {
				if ( ! this->ReadBit( bit ) ) {
					return bitsRead;
				}
				*buffer |= ( bit << ( 7 - i ) );
				bitsRead++;
			}
		}

		return bitsRead;
	}

private:
	inStream_T&& iss;
	uint8_t      charInBuffer;
	size_t       charInBufferSize;
};

#if 0

class NaiveInBitStream : public IInBitStream
{
public:
	NaiveInBitStream( std::istream* ss ) : iss ( ss ) { }

	bool    ReadBit( bool& bit ) override;
	size_t  ReadNBits( unsigned char* buffer, size_t numBits ) override;

private:
	std::istream* iss;
};

class NaiveOutBitStream : public IOutBitStream
{
public:
	NaiveOutBitStream( std::ostream* ss ) : oss ( ss ) { }

	void    WriteNBits( const unsigned char* buffer, size_t numBits ) override;
	void    WriteBit( bool bit ) override;
	void    Flush() override;

private:
	std::ostream* oss;
};

#endif

