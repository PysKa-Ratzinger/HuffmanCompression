#pragma once

#include <vector>
#include <stdlib.h>

/*
================================
Represents the encoding of a character

  For example, A might be 101, B - 11, C - 011
  This will be used in compression, but not on decompression, as it will be
  more efficient to use a tree.

TODO: Consider if this class is even necessary...
================================
*/
class CharacterEncoding
{
public:
	CharacterEncoding();
	CharacterEncoding( const CharacterEncoding& original );
	~CharacterEncoding();

	bool     GetBit( size_t index ) const;
	size_t   GetBitSize() const;
	bool     IsEmpty() const;
	void     AddBit( bool bit );
	void     RemoveBit();

	CharacterEncoding& operator=( const CharacterEncoding& other );

private:
	std::vector<bool> bytes;
};

