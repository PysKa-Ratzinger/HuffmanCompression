#include <string.h>
#include <stdio.h>
#include "CharacterEncoding.hpp"

CharacterEncoding::CharacterEncoding()
{

}

CharacterEncoding::CharacterEncoding( CharacterEncoding const& original ) :
		bytes( original.bytes )
{

}

CharacterEncoding::~CharacterEncoding()
{

}

CharacterEncoding&
CharacterEncoding::operator=( const CharacterEncoding& other )
{
	this->bytes = other.bytes;
	return *this;
}

bool CharacterEncoding::GetBit( size_t index ) const
{
	return this->bytes.at( index );
}

size_t CharacterEncoding::GetBitSize() const
{
	return this->bytes.size();
}

bool CharacterEncoding::IsEmpty() const
{
	return this->bytes.size() == 0;
}

void CharacterEncoding::AddBit( bool bit )
{
	this->bytes.push_back( bit );
}

void CharacterEncoding::RemoveBit()
{
	if ( this->bytes.size() > 0 ) {
		this->bytes.pop_back();
	}
}

bool CharacterEncoding::operator==( const CharacterEncoding& other ) const
{
	return bytes == other.bytes;
}

