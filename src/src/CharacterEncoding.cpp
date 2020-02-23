#include <string.h>
#include <stdio.h>
#include "CharacterEncoding.hpp"

CharacterEncoding::CharacterEncoding()
{
	bytes = new char[START_SIZE];
	numBytes = START_SIZE;
	bitSize = 0;
	index = 0;
}

CharacterEncoding::CharacterEncoding( CharacterEncoding const& original )
{
	bytes = new char[original.numBytes];
	numBytes = original.numBytes;
	bitSize = original.bitSize;
	index = original.index;
	memcpy( bytes, original.bytes, numBytes );
}

CharacterEncoding::~CharacterEncoding()
{
	delete bytes;
}

size_t CharacterEncoding::GetBitSize()
{
	return bitSize;
}

void CharacterEncoding::AddBit( unsigned char bit )
{
	if ( index >= numBytes ) {
		char* newBytes = new char[numBytes * 2];
		delete[] bytes;
		bytes = newBytes;
		numBytes *= 2;
	}
	bytes[index] <<= 1;
	bytes[index] |= bit & 0x1;
	bitSize++;
	if ( bitSize % 8 == 0 ) {
		index++;
	}
}

void CharacterEncoding::RemoveBit()
{
	if ( bitSize == 0 ) {
		return;
	}
	if ( bitSize % 8 == 0 ) {
		index--;
	}
	bytes[index] >>= 1;
	bitSize--;
}

unsigned char CharacterEncoding::GetBit( size_t index ) const
{
	if ( index >= bitSize ) {
		return -1;
	}
	size_t base_index = index / 8;
	unsigned char res;
	if ( base_index == index ) {
		res = bytes[base_index] >> ( bitSize % 8 - index % 8 - 1 ) & 0x1;
	} else {
		res = ( bytes[base_index] >> ( 7 - index % 8 ) ) & 0x1;
	}
	return res;
}

