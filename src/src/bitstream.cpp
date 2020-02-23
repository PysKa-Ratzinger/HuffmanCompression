#include "bitstream.hpp"

BitStream::BitStream( FILE* file )
		: _file(file)
		, _charOutBuffer(0)
		, _charOutBufferSize(0)
{

}

void BitStream::writeNBits( const unsigned char *buffer, size_t numBits )
{
	while ( numBits >= 8 ) {
		const unsigned char c = *buffer;
		for ( size_t i = 0; i < 8; i++ ) {
			writeBit( c >> ( 7 - i ) );
		}
		buffer++;
		numBits -= 8;
	}

	const unsigned char c = *buffer;
	for ( size_t i = 0; i < numBits; i++ ) {
		writeBit( c >> ( 7 - i ) );
	}
}

void BitStream::writeBit( unsigned char bit )
{
	size_t shift = 8 - _charOutBufferSize - 1;
	_charOutBuffer >>= shift;
	_charOutBuffer |= bit & 0x1;
	_charOutBuffer <<= shift;
	_charOutBufferSize++;
	if ( _charOutBufferSize == 8 ) {
		flush();
	}
}

unsigned char BitStream::readBit()
{
	if ( _charInBufferSize == 0 ) {
		fread( &_charInBuffer, 1, 1, _file );
		_charInBufferSize = 8;
	}
	unsigned char res = _charInBuffer & 0x80;
	_charInBuffer <<= 0x1;
	_charInBufferSize--;
	return res;
}

void BitStream::readNBits( unsigned char* buffer, size_t numBits )
{
	while ( numBits >= 8 ) {
		for ( size_t i = 0; i < 8; i++ ) {
			char bit = this->readBit();
			*buffer |= ( bit << ( 7 - i ) );
		}
		buffer++;
		numBits -= 8;
	}
	for ( size_t i = 0; i < numBits; i++ ) {
		char bit = this->readBit();
		*buffer |= ( bit << ( 7 - i ) );
	}
}

void BitStream::flush()
{
	if ( _charOutBufferSize > 0 ) {
		_charOutBuffer <<= ( 8 - _charOutBufferSize );
		fwrite( &_charOutBuffer, 1, 1, _file );
		_charOutBufferSize = 0;
	}
}

