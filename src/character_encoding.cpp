#include <string.h>
#include <stdio.h>
#include "character_encoding.hpp"

CharacterEncoding::CharacterEncoding(){
	_bytes = new char[START_SIZE];
	_num_bytes = START_SIZE;
	_bitSize = 0;
	_index = 0;
}

CharacterEncoding::CharacterEncoding(CharacterEncoding const& original){
	_bytes = new char[original._num_bytes];
	_num_bytes = original._num_bytes;
	_bitSize = original._bitSize;
	_index = original._index;
	memcpy(_bytes, original._bytes, _num_bytes);
}

CharacterEncoding::~CharacterEncoding(){
	delete _bytes;
}

size_t CharacterEncoding::getBitSize(){
	return _bitSize;
}

void CharacterEncoding::addBit(unsigned char bit){
	if(_index >= _num_bytes){
		char* newBytes = new char[_num_bytes * 2];
		delete[] _bytes;
		_bytes = newBytes;
		_num_bytes *= 2;
	}
	_bytes[_index] <<= 1;
	_bytes[_index] |= bit & 0x1;
	_bitSize++;
	if(_bitSize % 8 == 0) _index++;
}

void CharacterEncoding::removeBit(){
	if(_bitSize == 0) return;
	if(_bitSize % 8 == 0) _index--;
	_bytes[_index] >>= 1;
	_bitSize--;
}

unsigned char CharacterEncoding::getBit(size_t index) const{
	if(index >= _bitSize) return -1;
	size_t base_index = index / 8;
	unsigned char res;
	if(base_index == _index){
		res = _bytes[base_index] >> (_bitSize % 8 - index % 8 - 1) & 0x1;
	}else{
		res = (_bytes[base_index] >> (7 - index % 8)) & 0x1;
	}
	return res;
}

