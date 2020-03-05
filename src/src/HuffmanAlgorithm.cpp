#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <algorithm>

#include "BinaryHeap.hpp"
#include "BitStream.hpp"
#include "CharacterEncoding.hpp"
#include "HuffmanAlgorithm.hpp"

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

void HuffmanLeafNode::PrintImpl( unsigned depth ) const
{
	for ( unsigned i = 0; i < depth; i++ ) {
		printf(" | ");
	}
	printf( "\"%d\" - %lu\n", elem, Weight() );
}

void HuffmanLeafNode::EncodeBinaryImpl( BitStream& outStream ) const
{
	outStream.WriteBit( 0 );
	outStream.WriteNBits( &elem, sizeof(elem) * 8 );
}

void HuffmanLeafNode::SaveEncodingImpl(
		std::array< CharacterEncoding, 256>& arr,
		CharacterEncoding& curr ) const
{
	arr[elem] = CharacterEncoding( curr );
}

void HuffmanParentNode::SaveEncodingImpl(
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

HuffmanTree::HuffmanTree( const IHuffmanNode::Ptr head )
		: head( head )
{
	CharacterEncoding curr;
	if ( head != NULL ) {
		head->SaveEncoding( encoding, curr );
	}
}

HuffmanTree::~HuffmanTree()
{

}

void HuffmanParentNode::PrintImpl( unsigned const depth ) const
{
	left->Print( depth + 1 );
	for ( unsigned i = 0; i < depth; i++ ) {
		printf( " | " );
	}
	printf( "%lu\n", Weight() );
	right->Print( depth + 1 );
}

void HuffmanTree::Print() const
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

void HuffmanParentNode::EncodeBinaryImpl( BitStream& outStream ) const
{
	outStream.WriteBit( 1 );
	left->EncodeBinary( outStream );
	right->EncodeBinary( outStream );
}

void HuffmanTree::EncodeBinary( BitStream& outStream ) const
{
	if ( head ) {
		head->EncodeBinary( outStream );
	}
}

void HuffmanTree::EncodeByte( BitStream& outStream , unsigned char byte ) const
{
	const CharacterEncoding& currEncoding = encoding[byte];
	for ( unsigned i = 0; i < currEncoding.GetBitSize(); i++ ) {
		outStream.WriteBit( currEncoding.GetBit( i ) );
	}
}

const std::array< CharacterEncoding, 256 > HuffmanTree::GetEncoding() const
{
	return encoding;
}

#if 0
bool HuffmanEncode( int inFD, int outFD )
{
	fprintf( stderr, "Started analysis... " );
	fflush( stdout );
	struct FileAnalysis info( inFD );

	fprintf( stderr, "DONE\nCreating Huffman Tree... " );
	HuffmanTree tree = CreateHuffmanTree( info );

	fprintf( stderr, "DONE\n" );
	fprintf( stderr, "Creating output bitstream... " );

	// TODO: Create stringstream to file descriptor...?

	BitStream outStream( outFD );

	fprintf( stderr, "DONE\n" );
	tree.Print();

	/* Output file_size and huffman tree */
	write( outFD, &info.total, sizeof(info.total) );
	tree.EncodeBinary( outStream );

	/* Set input file position to beginning */
	lseek( inFD, 0, SEEK_SET );

	char input;
	size_t bytes_read;
	while ( ( bytes_read = read( inFD, &input, 1 ) ) > 0 ) {
		tree.EncodeByte( outStream, input );
	}

	outStream.Flush();

	return true;
}

bool HuffmanDecode( int inFD, int outFD )
{
	BitStream inputStream( inFD );
	HuffmanTree tree = ReadHuffmanTree( inputStream );
	BitStream outputStream( outFD );

	// tree->translate(inputStream, outputStream);
	outputStream.Flush();

	return true;
}
#endif

HuffmanTree CreateHuffmanTree( const struct FileAnalysis& info )
{
	std::vector< IHuffmanNode::Ptr > elems;

	double sum = 0;
	for ( unsigned i = 0; i < 0x100; i++ ) {
		if ( info.frequency[i] > 0 ) {
			unsigned long weight = info.frequency[i];
			auto leaf = std::make_shared<HuffmanLeafNode>( weight, i );
			sum += weight;
			elems.push_back( leaf );
		}
	}

	// Create heap with character frequencies
	BinaryHeap<IHuffmanNode::Ptr> heap ( elems );

	// Keep joining the two largest frequencies under a parent node (whose
	// frequency will be their sum), until only 1 node remains
	while ( heap.Size() >= 2 ) {
		auto parent = std::make_shared< HuffmanParentNode >(
				heap.Pop(), heap.Pop() );
		heap.Insert( parent );
	}

	// Done
	IHuffmanNode::Ptr node( heap.Pop() );
	return HuffmanTree( node );
}

static IHuffmanNode::Ptr ReadHuffmanNode( BitStream& inStream )
{
	unsigned char bit = inStream.ReadBit();
	if ( bit == 0 ) {
		unsigned char byte;
		inStream.ReadNBits( &byte, 8 );
		return std::make_shared< HuffmanLeafNode >( 0, byte );
	} else {
		IHuffmanNode::Ptr left = ReadHuffmanNode( inStream );
		IHuffmanNode::Ptr right = ReadHuffmanNode( inStream );
		return std::make_shared< HuffmanParentNode >( left, right );
	}
}

HuffmanTree ReadHuffmanTree( BitStream& inStream )
{
	IHuffmanNode::Ptr head = ReadHuffmanNode( inStream );
	return HuffmanTree( head );
}

