#include "bitstream.hpp"

BitStream::BitStream(FILE* file)
	: _file(file), _charBuffer(0), _charBufferSize(0) {}

	void BitStream::writeNBits(const unsigned char *buffer, size_t numBits){
		if(_charBufferSize == 0){
			fwrite(buffer, numBits/8, 1, _file);
			if(numBits % 8 != 0){
				numBits %= 8;
				_charBuffer = buffer[numBits/8 + 1] >> (8 - numBits);
				_charBufferSize = numBits;
			}
		}else{
			size_t cycle1 = (8 - _charBufferSize);
			size_t cycle2 = _charBufferSize;
			while(numBits >= 8){
				_charBuffer <<= cycle1;
				_charBuffer |= *buffer >> cycle2;
				fwrite(&_charBuffer, 1, 1, _file);
				_charBuffer = *buffer;
				numBits -= 8;
				buffer++;
			}
			if(numBits > 0){
				if(numBits + _charBufferSize <= 8){
					_charBuffer <<= numBits;
					_charBuffer |= *buffer >> (8 - numBits);
					_charBufferSize += numBits;
					if(_charBufferSize == 8){
						fwrite(&_charBuffer, 1, 1, _file);
						_charBufferSize = 0;
					}
				}else{
					_charBuffer <<= 8 - _charBufferSize;
					_charBuffer |= *buffer >> _charBufferSize;
					fwrite(&_charBuffer, 1, 1, _file);
					_charBuffer = *buffer >> (8 - numBits);
					_charBufferSize = numBits + _charBufferSize - 8;
				}
			}
		}
	}

void BitStream::writeBit(unsigned char bit){
	_charBuffer <<= 1;
	_charBuffer |= bit & 0x1;
	_charBufferSize++;
	if(_charBufferSize == 8){
		flush();
	}
}

void BitStream::flush(){
	if(_charBufferSize > 0){
		_charBuffer <<= (8 - _charBufferSize);
		fwrite(&_charBuffer, 1, 1, _file);
		_charBufferSize = 0;
	}
}
