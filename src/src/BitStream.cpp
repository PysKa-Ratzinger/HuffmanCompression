#include "BitStream.hpp"

BitStream::BitStream( FILE* file )
		: file(file)
		, charOutBuffer(0)
		, charOutBufferSize(0)
{

}

void BitStream::WriteNBits( const unsigned char *buffer, size_t numBits )
{
	while ( numBits >= 8 ) {
		const unsigned char c = *buffer;
		for ( size_t i = 0; i < 8; i++ ) {
			WriteBit( c >> ( 7 - i ) );
		}
		buffer++;
		numBits -= 8;
	}

	const unsigned char c = *buffer;
	for ( size_t i = 0; i < numBits; i++ ) {
		WriteBit( c >> ( 7 - i ) );
	}
}

void BitStream::WriteBit( unsigned char bit )
{
	size_t shift = 8 - charOutBufferSize - 1;
	charOutBuffer >>= shift;
	charOutBuffer |= bit & 0x1;
	charOutBuffer <<= shift;
	charOutBufferSize++;
	if ( charOutBufferSize == 8 ) {
		Flush();
	}
}

unsigned char BitStream::ReadBit()
{
	if ( charInBufferSize == 0 ) {
		fread( &charInBuffer, 1, 1, file );
		charInBufferSize = 8;
	}
	unsigned char res = charInBuffer & 0x80;
	charInBuffer <<= 0x1;
	charInBufferSize--;
	return res;
}

void BitStream::ReadNBits( unsigned char* buffer, size_t numBits )
{
	while ( numBits >= 8 ) {
		for ( size_t i = 0; i < 8; i++ ) {
			char bit = this->ReadBit();
			*buffer |= ( bit << ( 7 - i ) );
		}
		buffer++;
		numBits -= 8;
	}
	for ( size_t i = 0; i < numBits; i++ ) {
		char bit = this->ReadBit();
		*buffer |= ( bit << ( 7 - i ) );
	}
}

void BitStream::Flush()
{
	if ( charOutBufferSize > 0 ) {
		charOutBuffer <<= ( 8 - charOutBufferSize );
		fwrite( &charOutBuffer, 1, 1, file );
		charOutBufferSize = 0;
	}
}

