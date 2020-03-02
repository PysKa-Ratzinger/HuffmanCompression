#pragma once

#include <stdlib.h>
#include <memory>

#include "BitStream.hpp"
#include "CharacterEncoding.hpp"

#define TOTAL_CHARS 0x100

/*
================================
Used to store all the information regarding the frequency of every byte
found in the analysed file
================================
 */
struct AnalysisInfo
{
	/**
	 * Array with the frequency of every byte. The index of each frequency
	 * corresponds to the byte analysed, and the value to the frequency of
	 * said byte.
	 */
	unsigned long frequency[TOTAL_CHARS] { 0 };

	/**
	 * The total number of bytes in the file
	 */
	u_int64_t total = 0;
};

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
	virtual void  SaveEncoding( CharacterEncoding* arr[256], CharacterEncoding* curr ) const = 0;

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
	void SaveEncoding( CharacterEncoding* arr[256], CharacterEncoding* curr ) const;

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
	void SaveEncoding( CharacterEncoding* arr[256], CharacterEncoding* curr ) const;

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
	void EncodeBinary( BitStream& outStream ) const;
	void EncodeByte( BitStream& outStream, unsigned char byte ) const;

private:
	std::shared_ptr<const HuffmanNode> head;
	CharacterEncoding**                encoding;
};

bool HuffmanEncode( int inFD, int outFD );
bool HuffmanDecode( int inFD, int outFD );

/**
 *  Given a file name, an AnalysisInfo structure is created with the frequency
 * of every byte present in that file. If the file cannot be opened or memory
 * allocation for the analysis information is not possible, the operation fails.
 *  If the operation fails, NULL is returned and @{errno} is left with a value
 * that describes the problem that occurred.
 */
struct AnalysisInfo AnalyseFile( int inFD );

/**
 * Given the analysis information of a file, a huffman tree is generated.
 * @param  info Analysis info of a file that was analysed
 * @return      Pointer to a new huffman_tree structure
 */
HuffmanTree CreateHuffmanTree( const struct AnalysisInfo& info );

/**
 * Given an input stream, reads the encoded huffman tree
 */
HuffmanTree ReadHuffmanTree( BitStream& inStream );

