#pragma once

#include <stdio.h>
#include <stdlib.h>

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
	unsigned long frequency[TOTAL_CHARS];

	/**
	 * The total number of bytes in the file
	 */
	u_int64_t total;
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
	virtual void  EncodeBinary( BitStream* outStream ) const = 0;
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
	void EncodeBinary( BitStream* outStream ) const;
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
	HuffmanParentNode( HuffmanNode* left, HuffmanNode* right );
	~HuffmanParentNode();

	void Print( unsigned const depth ) const;
	void EncodeBinary( BitStream* outStream ) const;
	void SaveEncoding( CharacterEncoding* arr[256], CharacterEncoding* curr ) const;

private:
	HuffmanNode* left;
	HuffmanNode* right;
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
	HuffmanTree( const HuffmanNode* head );
	~HuffmanTree();

	void Print() const;
	void EncodeBinary( BitStream* outStream ) const;
	void EncodeByte( BitStream* outStream, unsigned char byte ) const;

private:
	const HuffmanNode* head;
	CharacterEncoding** encoding;
};

bool HuffmanEncode( FILE* inputFile, FILE* outputFile );
bool HuffmanDecode( FILE* inputFile, FILE* outputFile );

/**
 *  Given a file name, an AnalysisInfo structure is created with the frequency
 * of every byte present in that file. If the file cannot be opened or memory
 * allocation for the analysis information is not possible, the operation fails.
 *  If the operation fails, NULL is returned and @{errno} is left with a value
 * that describes the problem that occurred.
 */
struct AnalysisInfo* AnalyseFile( FILE* inputFile );

/**
 * Given the analysis information of a file, a huffman tree is generated.
 * @param  info Analysis info of a file that was analysed
 * @return      Pointer to a new huffman_tree structure
 */
HuffmanTree* CreateHuffmanTree( struct AnalysisInfo* info );

/**
 * Given an input stream, reads the encoded huffman tree
 */
HuffmanTree* ReadHuffmanTree( BitStream* inStream );

