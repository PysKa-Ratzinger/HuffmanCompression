#include <string.h>
#include <stdio.h>
#include "CharacterEncoding.hpp"

#include <stdexcept>
#include <sstream>

CharacterEncoding::CharacterEncoding() : numBits( 0 )
{

}

CharacterEncoding::CharacterEncoding( CharacterEncoding const& original )
		: bytes( original.bytes )
		, numBits( original.numBits )
{

}

CharacterEncoding::~CharacterEncoding()
{

}

CharacterEncoding&
CharacterEncoding::operator=( const CharacterEncoding& other )
{
	this->bytes = other.bytes;
	this->numBits = other.numBits;
	return *this;
}

const uint8_t* CharacterEncoding::GetRaw() const
{
	return &this->bytes[0];
}

bool CharacterEncoding::GetBit( size_t index ) const
{
	if ( index >= this->numBits )
		throw std::range_error( "Bit out of range" );

	size_t charIdx = index / 8;
	uint8_t c = this->bytes.at( charIdx );

	//
	// 0b0123 4567

	uint8_t rem = index % 8;
	return ( c >> (7 - rem) ) & 0x1;
}

size_t CharacterEncoding::GetNumBits() const
{
	return this->numBits;
}

bool CharacterEncoding::IsEmpty() const
{
	return this->bytes.size() == 0;
}

void CharacterEncoding::AddBit( bool bit )
{
	int rem = this->numBits % 8;
	if ( rem == 0 ) {
		this->bytes.push_back( ((uint8_t) bit) << 7 );
	} else {
		this->bytes[ this->bytes.size() - 1 ] |= ((uint8_t) bit) << (7 - rem);
	}
	this->numBits++;
}

void CharacterEncoding::RemoveBit()
{
	// numbits: 0
	// 0b1111 1111
	//
	// 0x80 >> ( numbits - 2 )
	// 0b0000 0001
	//
	// _ - 1
	// 0b0011 1111
	//
	// ~ _
	// 0b1100 0000

	int rem = this->numBits % 8;
	if ( rem == 1 ) {
		this->bytes.pop_back();
	} else {
		if ( rem == 0 ) {
			rem = 8;
		}
		uint8_t mask = ~ ((0x80 >> ( rem - 2 ) ) - 1);
		this->bytes[ this->bytes.size() - 1 ] &= mask;
	}
	this->numBits--;
}

bool CharacterEncoding::operator==( const CharacterEncoding& other ) const
{
	return bytes == other.bytes;
}

std::string CharacterEncoding::GetRepr() const
{
	std::stringstream ss;
	for ( size_t i = 0; i < this->GetNumBits(); i++ )
	{
		bool bit = this->GetBit( i );
		ss << (bit ? "1" : "0");
	}
	return ss.str();
}

