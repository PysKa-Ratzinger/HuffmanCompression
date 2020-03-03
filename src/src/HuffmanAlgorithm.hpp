#pragma once

#include <stdlib.h>
#include <memory>
#include <array>

#include "BitStream.hpp"
#include "CharacterEncoding.hpp"
#include "FileAnalysis.hpp"

/*
================================
Abstract base class of a Huffman Tree Node. The two subclasses of this
base class represent the Leaf Node and the Parent Node.
================================
 */
class HuffmanNode
{
public:
	HuffmanNode( unsigned long weight );
	virtual ~HuffmanNode() {}

	unsigned long Weight() const;
	bool          operator < ( const HuffmanNode& node ) const;
	virtual void  Print( unsigned const depth ) const = 0;
	virtual void  EncodeBinary( BitStream& outStream ) const = 0;
	virtual void  SaveEncoding( 
			std::array< CharacterEncoding, 256>& arr,
			CharacterEncoding& curr ) const = 0;

private:
	unsigned long weight;
};

/*
================================
Leaf node from a Huffman tree. Represents a node with no childs,
but a value.
================================
 */
class HuffmanLeafNode : public HuffmanNode
{
public:
	HuffmanLeafNode( unsigned long weight, unsigned char elem );
	~HuffmanLeafNode() {}

	void Print( unsigned const depth ) const;
	void EncodeBinary( BitStream& outStream ) const;
	void SaveEncoding(
			std::array< CharacterEncoding, 256>& arr,
			CharacterEncoding& curr ) const;

private:
	unsigned char elem;
};

/*
================================
Parent node from a Huffman tree. Represents a node with no value but
two children.
================================
 */
class HuffmanParentNode : public HuffmanNode
{
public:
	using HuffmanNodePtr = std::shared_ptr< HuffmanNode >;

	HuffmanParentNode( HuffmanNodePtr left, HuffmanNodePtr right );
	~HuffmanParentNode();

	void Print( unsigned const depth ) const;
	void EncodeBinary( BitStream& outStream ) const;
	void SaveEncoding( 
			std::array< CharacterEncoding, 256>& arr,
			CharacterEncoding& curr ) const;

private:
	HuffmanNodePtr left;
	HuffmanNodePtr right;
};

/*
================================
Huffman tree. Contains every information necessary in order to encode and
decode information
================================
 */
class HuffmanTree
{
public:
	HuffmanTree( std::shared_ptr<const HuffmanNode> head );
	~HuffmanTree();

	void Print() const;

	/*
	 * Outputs the Huffman Encoding tree in a binary form
	 */
	void EncodeBinary( BitStream& outStream ) const;

	/*
	 * Given a byte, whose encoding we know, output it's binary correspondent
	 */
	void EncodeByte( BitStream& outStream, unsigned char byte ) const;

private:
	std::shared_ptr< const HuffmanNode >   head;
	std::array< CharacterEncoding, 256 >   encoding;
};

bool HuffmanEncode( int inFD, int outFD );
bool HuffmanDecode( int inFD, int outFD );

/**
 * Given the analysis information of a file, a huffman tree is generated.
 * @param  info Analysis info of a file that was analysed
 * @return      Pointer to a new huffman_tree structure
 */
HuffmanTree CreateHuffmanTree( const struct FileAnalysis& info );

/**
 * Given an input stream, reads the encoded huffman tree
 */
HuffmanTree ReadHuffmanTree( BitStream& inStream );

