#pragma once

#define START_SIZE 2

#include <stdlib.h>

/*
================================
Represents the encoding of a character
================================
 */
class CharacterEncoding
{
public:
	CharacterEncoding();
	CharacterEncoding(CharacterEncoding const& original);
	~CharacterEncoding();
	size_t getBitSize();
	void addBit( unsigned char bit );
	void removeBit();
	unsigned char getBit( size_t index ) const;

private:
	char* _bytes;
	size_t _index;
	size_t _num_bytes;
	size_t _bitSize;
};

