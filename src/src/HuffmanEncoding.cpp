#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <algorithm>

#include "BinaryHeap.hpp"
#include "BitStream.hpp"
#include "CharacterEncoding.hpp"
#include "HuffmanEncoding.hpp"

#define BUFFER_SZ 512
#define TRUE 1
#define FALSE 0

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
	(byte & 0x80 ? '1' : '0'), \
	(byte & 0x40 ? '1' : '0'), \
	(byte & 0x20 ? '1' : '0'), \
	(byte & 0x10 ? '1' : '0'), \
	(byte & 0x08 ? '1' : '0'), \
	(byte & 0x04 ? '1' : '0'), \
	(byte & 0x02 ? '1' : '0'), \
	(byte & 0x01 ? '1' : '0')

using namespace huffman;

void
LeafNode::PrintImpl( unsigned depth ) const
{
	for ( unsigned i = 0; i < depth; i++ ) {
		printf(" | ");
	}
	printf( "\"%d\" - %lu\n", elem, Weight() );
}

void
LeafNode::EncodeBinaryImpl( BitStream& outStream ) const
{
	outStream.WriteBit( 0 );
	outStream.WriteNBits( &elem, sizeof(elem) * 8 );
}

void
LeafNode::SaveEncodingImpl(
		std::array< CharacterEncoding, 256>& arr,
		CharacterEncoding& curr ) const
{
	arr[elem] = CharacterEncoding( curr );
}

bool
LeafNode::equals( const LeafNode& other ) const
{
	return this->elem == other.elem;
}

void
ParentNode::SaveEncodingImpl(
		std::array< CharacterEncoding, 256>& arr,
		CharacterEncoding& curr ) const
{
	curr.AddBit( 0 );
	left->SaveEncoding( arr, curr );
	curr.RemoveBit();
	curr.AddBit( 1 );
	right->SaveEncoding( arr, curr );
	curr.RemoveBit();
}

Tree::Tree( const INode::Ptr head )
		: head( head )
{
	CharacterEncoding curr;
	if ( head != NULL ) {
		head->SaveEncoding( encoding, curr );
	}
}

Tree::~Tree()
{

}

static INode::Ptr
ReadHuffmanNode( BitStream& inStream )
{
	bool bit;

	if ( ! inStream.ReadBit( bit ) ) {
		return nullptr;
	}

	if ( bit == 0 ) {
		unsigned char byte;
		inStream.ReadNBits( &byte, 8 );
		return std::make_shared< LeafNode >( 0, byte );
	} else {
		INode::Ptr left = ReadHuffmanNode( inStream );
		INode::Ptr right = ReadHuffmanNode( inStream );
		return std::make_shared< ParentNode >( left, right );
	}
}

Tree
Tree::LoadTree( BitStream& inStream )
{
	INode::Ptr head = ReadHuffmanNode( inStream );
	return Tree( head );
}

void
ParentNode::PrintImpl( unsigned const depth ) const
{
	left->Print( depth + 1 );
	for ( unsigned i = 0; i < depth; i++ ) {
		printf( " | " );
	}
	printf( "%lu\n", Weight() );
	right->Print( depth + 1 );
}

void
Tree::Print() const
{
	head->Print( 0 );
	for ( unsigned i = 0; i < 256; i++ ) {
		const CharacterEncoding& c = encoding.at( i );
		if ( ! c.IsEmpty() ) {
			printf( "Encoding for %d is: ", i );
			for ( unsigned j = 0; j < c.GetBitSize(); j++ ) {
				printf( "%d", c.GetBit( j ) );
			}
			printf( "\n" );
		}
	}
}

void
ParentNode::EncodeBinaryImpl( BitStream& outStream ) const
{
	outStream.WriteBit( 1 );
	left->EncodeBinary( outStream );
	right->EncodeBinary( outStream );
}

bool
ParentNode::equals( const ParentNode& other ) const
{
	return *this->left == *other.left &&
		*this->right == *other.right;
}

void
Tree::EncodeBinary( BitStream& outStream ) const
{
	if ( head ) {
		head->EncodeBinary( outStream );
	}
}

void
Tree::EncodeByte( BitStream& outStream , unsigned char byte ) const
{
	const CharacterEncoding& currEncoding = encoding[byte];
	for ( unsigned i = 0; i < currEncoding.GetBitSize(); i++ ) {
		outStream.WriteBit( currEncoding.GetBit( i ) );
	}
}

const
std::array< CharacterEncoding, 256 > Tree::GetEncoding() const
{
	return encoding;
}

Tree
Tree::CreateNewTree( const struct FileAnalysis& info )
{
	std::vector< INode::Ptr > elems;

	double sum = 0;
	for ( unsigned i = 0; i < 0x100; i++ ) {
		if ( info.frequency[i] > 0 ) {
			unsigned long weight = info.frequency[i];
			auto leaf = std::make_shared<LeafNode>( weight, i );
			sum += weight;
			elems.push_back( leaf );
		}
	}

	// Create heap with character frequencies
	BinaryHeap<INode::Ptr> heap ( elems );

	// Keep joining the two largest frequencies under a parent node (whose
	// frequency will be their sum), until only 1 node remains
	while ( heap.Size() >= 2 ) {
		auto parent = std::make_shared< ParentNode >(
				heap.Pop(), heap.Pop() );
		heap.Insert( parent );
	}

	// Done
	INode::Ptr node( heap.Pop() );
	return Tree( node );
}

bool
Tree::operator==( const Tree& other ) const
{
	return *this->head == *other.head;
}

