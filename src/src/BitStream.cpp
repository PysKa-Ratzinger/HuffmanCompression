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

void BitStream::WriteNBits( const unsigned char *buffer, size_t numBits )
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
		WriteBit( c >> ( 7 - i ) );
	}
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

