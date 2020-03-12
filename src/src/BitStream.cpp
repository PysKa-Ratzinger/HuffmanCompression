#include "BitStream.hpp"

#include <unistd.h>

#include <stdexcept>

BitStream::BitStream( std::stringstream& oss )
		: oss( oss )
		, charInBuffer( 0 )
		, charOutBuffer( 0 )
		, charInBufferSize( 0 )
		, charOutBufferSize( 0 )
{

}

/*
 * Current behaviour:
 *
 * When writing N bits, write every bit one by one
 *
 *
 * Optimized behaviour:
 * 
 * Calculate when one can use shifts and masks to make multiple operations in
 * one
 *
 * Case1: ( done )
 * Our buffer is empty. So, it's easy. Just copy the bits into the buffer
 *
 * Case2:
 * Our buffer is not empty, and we are:
 *
 * 2.1
 *   Trying to write less bytes than it would take to complete a full byte.
 *   0b1110 0000 + 0b1110 0000 = 0b1111 1100
 *
 * 2.2
 *   Trying to write exactly byte sized pieces
 *   0baaa0 0000 + 0b1234 5678
 *
 *   step1: 0baaa1 2345 -> Flush
 *   step2: 0b6780 0000
 */
void BitStream::WriteNBits( const unsigned char *buffer, size_t numBits )
{
#if 0
	// TODO: Fix this function

	if ( charOutBufferSize == 0 ) {
		while ( numBits >= 8 ) {
			this->charOutBuffer = *buffer;
			Flush();
			numBits -= 8;
			buffer++;
		}

		if ( numBits > 0 ) {
			this->charOutBuffer = *buffer;
			this->charOutBufferSize = numBits;
		}
	} else {
		//size_t offsetBytes = this->charOutBufferSize;
		unsigned char c;

		/*
		while ( numBits >= 8 ) {
			c = *buffer;

			this->charOutBuffer |= c >> offsetBytes;
			this->Flush();
			this->charOutBuffer = 0x0;
			this->charOutBuffer |= c << ( 8 - offsetBytes );

			buffer++;
			numBits -= 8;
		}

		this->charOutBufferSize = offsetBytes;

		if ( numBits > 0 ) {
			c = *buffer;
			this->charOutBuffer |= c >> offsetBytes;

			if ( numBits < ( 8 - offsetBytes ) ) {
				this->charOutBufferSize += numBits;
			} else {
				this->Flush();
				this->charOutBuffer = 0x0 | c << ( 8 - offsetBytes );
				this->charOutBufferSize = numBits - ( 8 - offsetBytes );
			}
		}
		*/

		c = *buffer;
		for ( size_t i = 0; i < numBits; i++ ) {
			bool bit = ( c >> ( 7 - i ) ) & 0x1;
			WriteBit( bit );
		}
	}
#else
	while ( numBits >= 8 ) {
		const unsigned char c = *buffer;
		for ( size_t i = 0; i < 8; i++ ) {
			bool bit = ( c >> ( 7 - i ) ) & 0x1;
			WriteBit( bit );
		}
		buffer++;
		numBits -= 8;
	}

	const unsigned char c = *buffer;
	for ( size_t i = 0; i < numBits; i++ ) {
		bool bit = ( c >> ( 7 - i ) ) & 0x1;
		WriteBit( bit );
	}
#endif
}

void BitStream::WriteBit( bool bit )
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

bool BitStream::ReadBit( bool& bit )
{
	if ( charInBufferSize == 0 ) {
		int c = oss.get();
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

size_t BitStream::ReadNBits( unsigned char* buffer, size_t numBits )
{
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

	*buffer = '\0';
	for ( size_t i = 0; i < numBits; i++ ) {
		if ( ! this->ReadBit( bit ) ) {
			return bitsRead;
		}
		*buffer |= ( bit << ( 7 - i ) );
		bitsRead++;
	}

	return bitsRead;
}

void BitStream::Flush()
{
	if ( charOutBufferSize > 0 ) {
		oss << charOutBuffer;
		charOutBufferSize = 0;
	}
}


void NaiveBitStream::WriteNBits( const unsigned char *buffer, size_t numBits )
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

	const unsigned char c = *buffer;
	for ( size_t i = 0; i < numBits; i++ ) {
		bool bit = ( c >> ( 7 - i ) ) & 0x1;
		WriteBit( bit );
	}
}

void NaiveBitStream::WriteBit( bool bit )
{
	this->oss << ( bit ? "1" : "0" );
}

bool NaiveBitStream::ReadBit( bool& bit )
{
	int c = oss.get();
	if ( c == EOF ) {
		return false;
	}

	if ( c != '0' && c != '1' ) {
		return false;
	}

	bit = c == '1';
	return true;
}

size_t NaiveBitStream::ReadNBits( unsigned char* buffer, size_t numBits )
{
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

	*buffer = '\0';
	for ( size_t i = 0; i < numBits; i++ ) {
		if ( ! this->ReadBit( bit ) ) {
			return bitsRead;
		}
		*buffer |= ( bit << ( 7 - i ) );
		bitsRead++;
	}

	return bitsRead;
}

void NaiveBitStream::Flush()
{

}


