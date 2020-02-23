#include <stdio.h>
#include <string.h>
#include "HuffmanAlgorithm.hpp"
#include "BinaryHeap.hpp"
#include "BitStream.hpp"
#include "CharacterEncoding.hpp"

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

HuffmanNode::HuffmanNode( unsigned long weight ) : weight( weight )
{

}

unsigned long HuffmanNode::Weight() const
{
	return weight;
}

bool HuffmanNode::operator < ( const HuffmanNode& node ) const
{
	return weight < node.weight;
}

HuffmanLeafNode::HuffmanLeafNode( unsigned long weight, unsigned char elem )
	: HuffmanNode( weight )
	, elem( elem )
{

}

void HuffmanLeafNode::SaveEncoding(
		CharacterEncoding **arr, CharacterEncoding *curr ) const
{
	arr[elem] = new CharacterEncoding( *curr );
}

HuffmanParentNode::HuffmanParentNode( HuffmanNodePtr left, HuffmanNodePtr right )
	: HuffmanNode( left->Weight() + right->Weight() )
	, left(left)
	, right(right)
{

}

HuffmanParentNode::~HuffmanParentNode()
{

}

void HuffmanParentNode::SaveEncoding( CharacterEncoding **arr,
		CharacterEncoding *curr ) const
{
	curr->AddBit( 0 );
	left->SaveEncoding( arr, curr );
	curr->RemoveBit();
	curr->AddBit( 1 );
	right->SaveEncoding( arr, curr );
	curr->RemoveBit();
}

HuffmanTree::HuffmanTree( std::shared_ptr<const HuffmanNode> head )
		: head( head )
{
	encoding = new CharacterEncoding*[256];
	for ( unsigned i = 0; i < 256; i++ ) {
		encoding[i] = NULL;
	}
	CharacterEncoding* curr = new CharacterEncoding();
	if ( head != NULL ) {
		head->SaveEncoding( encoding, curr );
	}
	delete curr;
}

HuffmanTree::~HuffmanTree()
{
	for ( unsigned i = 0; i < 256; i++ ) {
		if ( encoding[i] ) {
			delete encoding[i];
		}
	}
	delete[] encoding;
}

void HuffmanLeafNode::Print( unsigned const depth ) const
{
	for ( unsigned i = 0; i < depth; i++ ) {
		printf(" | ");
	}
	printf( "\"%d\" - %lu\n", elem, Weight() );
}

void HuffmanParentNode::Print( unsigned const depth ) const
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
		if ( encoding[i] != NULL ) {
			printf( "Encoding for %d is: ", i );
			for ( unsigned j = 0; j < encoding[i]->GetBitSize(); j++ ) {
				printf( "%d", encoding[i]->GetBit(j) );
			}
			printf( "\n" );
		}
	}
}

void HuffmanLeafNode::EncodeBinary( BitStream& outStream ) const
{
	outStream.WriteBit( 0 );
	outStream.WriteNBits( &elem, sizeof(elem) * 8 );
}

void HuffmanParentNode::EncodeBinary( BitStream& outStream ) const
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
	CharacterEncoding* currEncoding = encoding[byte];
	for ( unsigned i = 0; i < currEncoding->GetBitSize(); i++ ) {
		outStream.WriteBit( currEncoding->GetBit( i ) );
	}
}

bool HuffmanEncode( FILE* inputFile, FILE* outputFile )
{
	fprintf( stderr, "Started analysis... " );
	fflush( stdout );
	struct AnalysisInfo info = AnalyseFile( inputFile );

	fprintf( stderr, "DONE\nCreating Huffman Tree... " );
	HuffmanTree tree = CreateHuffmanTree( info );

	fprintf( stderr, "DONE\n" );
	fprintf( stderr, "Creating output bitstream... " );
	BitStream outStream( outputFile );

	fprintf( stderr, "DONE\n" );
	tree.Print();

	/* Output file_size and huffman tree */
	fwrite( &info.total, sizeof(info.total), 1, outputFile );
	tree.EncodeBinary( outStream );

	/* Set input file position to beginning */
	fseek( inputFile, 0, SEEK_SET );

	char input;
	size_t bytes_read;
	while ( ( bytes_read = fread( &input, 1, 1, inputFile ) ) > 0 ) {
		tree.EncodeByte( outStream, input );
	}

	outStream.Flush();

	return true;
}

bool HuffmanDecode( FILE* inputFile, FILE* outputFile )
{
	BitStream inputStream( inputFile );
	HuffmanTree tree = ReadHuffmanTree( inputStream );
	BitStream outputStream( outputFile );

	// tree->translate(inputStream, outputStream);
	outputStream.Flush();

	return true;
}

struct AnalysisInfo AnalyseFile( FILE* inputFile )
{
	struct AnalysisInfo res;

	unsigned char buffer[BUFFER_SZ];
	size_t bytes_read = 0;
	while ( ( bytes_read = fread( buffer, 1, BUFFER_SZ, inputFile ) ) ) {
		for ( unsigned i = 0; i < bytes_read; i++ ) {
			res.frequency[ buffer[i] ]++;
		}
	}

	unsigned long total = 0;
	for ( unsigned i = 0; i < 256; i++ ) {
		total += res.frequency[i];
	}
	res.total = total;

	return res;
}

HuffmanTree CreateHuffmanTree( const struct AnalysisInfo& info )
{
	BinaryHeap<std::shared_ptr<HuffmanNode>> heap ( 256 );

	double sum = 0;
	for ( unsigned i = 0; i < 0x100; i++ ) {
		if ( info.frequency[i] > 0 ) {
			unsigned long weight = info.frequency[i];
			auto leaf = std::make_shared< HuffmanLeafNode >( weight, i );
			sum += weight;
			heap.Insert( leaf );
		}
	}

	while ( heap.Size() >= 2 ) {
		auto parent = std::make_shared< HuffmanParentNode >(
				heap.Pop(), heap.Pop() );
		heap.Insert( parent );
	}

	std::shared_ptr< HuffmanNode > node( heap.Pop() );
	return HuffmanTree( node );
}

static std::shared_ptr< HuffmanNode > ReadHuffmanNode( BitStream& inStream )
{
	unsigned char bit = inStream.ReadBit();
	if ( bit == 0 ) {
		unsigned char byte;
		inStream.ReadNBits( &byte, 8 );
		return std::make_shared< HuffmanLeafNode >( 0, byte );
	} else {
		std::shared_ptr< HuffmanNode > left = ReadHuffmanNode( inStream );
		std::shared_ptr< HuffmanNode > right = ReadHuffmanNode( inStream );
		return std::make_shared< HuffmanParentNode >( left, right );
	}
}

HuffmanTree ReadHuffmanTree( BitStream& inStream )
{
	std::shared_ptr< HuffmanNode > head = ReadHuffmanNode( inStream );
	return HuffmanTree( head );
}

