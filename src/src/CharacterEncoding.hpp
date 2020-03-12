#pragma once

#include <vector>
#include <stdlib.h>
#include <stdint.h>
#include <string>

/*
================================
Represents the encoding of a character

  For example, A might be 101, B - 11, C - 011
  This will be used in compression, but not on decompression, as it will be
  more efficient to use a tree.
================================
*/
class CharacterEncoding
{
public:
	CharacterEncoding();
	CharacterEncoding( const CharacterEncoding& original );
	~CharacterEncoding();

	CharacterEncoding& operator=( const CharacterEncoding& other );

	const uint8_t* GetRaw() const;
	bool           GetBit( size_t index ) const;
	size_t         GetNumBits() const;
	bool           IsEmpty() const;
	void           AddBit( bool bit );
	void           RemoveBit();
	std::string    GetRepr() const;

	// Utility function
	void AddBits( const std::vector< bool > bits ) {
		for ( bool bit : bits ) {
			AddBit( bit );
		}
	}

	bool operator==( const CharacterEncoding& other ) const;

private:
	std::vector< uint8_t > bytes;
	size_t                 numBits;
};

