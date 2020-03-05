#pragma once

#include <stdlib.h>
#include <memory>
#include <array>

#include "BitStream.hpp"
#include "CharacterEncoding.hpp"
#include "FileAnalysis.hpp"

/*
================================
HuffmanNode interface
================================
 */
class IHuffmanNode
{
public:
	using Ptr = std::shared_ptr< IHuffmanNode >;

	virtual ~IHuffmanNode() {}

	virtual uint64_t Weight() const = 0;
	virtual void     Print( unsigned depth ) const = 0;
	virtual void     EncodeBinary( BitStream& outStream ) const = 0;
	virtual void     SaveEncoding( 
			std::array< CharacterEncoding, 256>& arr,
			CharacterEncoding& curr ) const = 0;

	bool operator < ( const IHuffmanNode& other ) const
	{
		return Weight() < other.Weight();
	}
};

// Specialization of std::greator< IHuffmanNode::Ptr >
template <>
struct std::greater< IHuffmanNode::Ptr >
{
	bool operator() ( const IHuffmanNode::Ptr a, const IHuffmanNode::Ptr b ) {
		if ( a && b ) {
			return *b < *a;
		}
		return b < a;

		// return a && b ? *b < *a : b < a;   // Not even readable
	}
};

/*
================================
Abstract base class of a Huffman Tree Node. The two subclasses of this
base class represent the Leaf Node and the Parent Node.
================================
 */
template < class Deriv >
class HuffmanNode : public IHuffmanNode
{
public:
	HuffmanNode( unsigned long weight ) : weight( weight ) { }

	virtual ~HuffmanNode() {}

	unsigned long Weight() const override {
		return weight;
	}

	void Print( unsigned depth ) const override {
		static_cast<const Deriv&>(*this).PrintImpl( depth );
	}

	void EncodeBinary( BitStream& outStream ) const override {
		static_cast<const Deriv&>(*this).EncodeBinaryImpl( outStream );
	}

	void SaveEncoding( std::array< CharacterEncoding, 256>& arr,
			CharacterEncoding& curr ) const override {
		static_cast<const Deriv&>(*this).SaveEncodingImpl( arr, curr );
	}

private:
	unsigned long weight;
};

/*
================================
Leaf node from a Huffman tree. Represents a node with no childs,
but a value.
================================
 */
class HuffmanLeafNode : public HuffmanNode< HuffmanLeafNode >
{
public:
	HuffmanLeafNode( unsigned long weight, unsigned char elem )
			: HuffmanNode< HuffmanLeafNode >( weight )
			, elem( elem )
	{

	}

	~HuffmanLeafNode() { }

	void PrintImpl( unsigned depth ) const;
	void EncodeBinaryImpl( BitStream& outStream ) const;
	void SaveEncodingImpl( std::array< CharacterEncoding, 256>& arr,
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
class HuffmanParentNode : public HuffmanNode< HuffmanParentNode >
{
public:
	HuffmanParentNode( IHuffmanNode::Ptr left, IHuffmanNode::Ptr right )
			: HuffmanNode< HuffmanParentNode >( left->Weight() + right->Weight() )
			, left(left)
			, right(right)
	{

	}

	~HuffmanParentNode() { }

	void PrintImpl( unsigned depth ) const;
	void EncodeBinaryImpl( BitStream& outStream ) const;
	void SaveEncodingImpl( std::array< CharacterEncoding, 256>& arr,
			CharacterEncoding& curr ) const;

private:
	IHuffmanNode::Ptr left;
	IHuffmanNode::Ptr right;
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
	HuffmanTree( const std::shared_ptr<IHuffmanNode> head );
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

	const std::array< CharacterEncoding, 256 > GetEncoding() const;

protected:
	std::shared_ptr< const IHuffmanNode >   head;
	std::array< CharacterEncoding, 256 >    encoding;
};

#if 0
bool HuffmanEncode( int inFD, int outFD );
bool HuffmanDecode( int inFD, int outFD );
#endif

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

