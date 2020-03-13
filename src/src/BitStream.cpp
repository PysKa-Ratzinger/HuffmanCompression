#include "BitStream.hpp"

#include <unistd.h>

#include <stdexcept>

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
#if 0
void OutBitStream::WriteNBits( const unsigned char *buffer, size_t numBits )
{
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

void NaiveOutBitStream::WriteNBits( const unsigned char *buffer, size_t numBits )
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

void NaiveOutBitStream::WriteBit( bool bit )
{
	*this->oss << ( bit ? "1" : "0" );
}

bool NaiveInBitStream::ReadBit( bool& bit )
{
	int c = iss->get();
	if ( c == EOF ) {
		return false;
	}

	if ( c != '0' && c != '1' ) {
		return false;
	}

	bit = c == '1';
	return true;
}

size_t NaiveInBitStream::ReadNBits( unsigned char* buffer, size_t numBits )
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

void NaiveOutBitStream::Flush()
{

}

#endif

