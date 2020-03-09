#pragma once

#include <stdlib.h>
#include <memory>
#include <array>

#include "BitStream.hpp"
#include "CharacterEncoding.hpp"
#include "FileAnalysis.hpp"

namespace huffman
{

/*
================================
Node interface
================================
 */
class INode
{
public:
	using Ptr = std::shared_ptr< INode >;

	virtual ~INode() {}

	virtual uint64_t Weight() const = 0;
	virtual void     Print( unsigned depth ) const = 0;
	virtual void     EncodeBinary( BitStream& outStream ) const = 0;
	virtual void     SaveEncoding( 
			std::array< CharacterEncoding, 256>& arr,
			CharacterEncoding& curr ) const = 0;

	bool operator < ( const INode& other ) const
	{
		return Weight() < other.Weight();
	}

	virtual bool operator==( const INode& other ) const;
};

/*
================================
Abstract base class of a Huffman Tree Node. The two subclasses of this
base class represent the Leaf Node and the Parent Node.
================================
 */
template < class Deriv >
class Node : public INode
{
public:
	Node( unsigned long weight ) : weight( weight ) { }

	virtual ~Node() {}

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

	bool operator==( const INode& other ) const override {
		const Deriv* o = dynamic_cast<const Deriv*>(&other);
		return o != nullptr && static_cast<const Deriv&>(*this).equals(*o);
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
class LeafNode : public Node< LeafNode >
{
public:
	LeafNode( unsigned long weight, unsigned char elem )
			: Node< LeafNode >( weight )
			, elem( elem )
	{

	}

	~LeafNode() { }

	void PrintImpl( unsigned depth ) const;
	void EncodeBinaryImpl( BitStream& outStream ) const;
	void SaveEncodingImpl( std::array< CharacterEncoding, 256>& arr,
			CharacterEncoding& curr ) const;

	bool equals( const LeafNode& other ) const;

private:
	unsigned char elem;
};

/*
================================
Parent node from a Huffman tree. Represents a node with no value but
two children.
================================
 */
class ParentNode : public Node< ParentNode >
{
public:
	ParentNode( INode::Ptr left, INode::Ptr right )
			: Node< ParentNode >( left->Weight() + right->Weight() )
			, left(left)
			, right(right)
	{

	}

	~ParentNode() { }

	void PrintImpl( unsigned depth ) const;
	void EncodeBinaryImpl( BitStream& outStream ) const;
	void SaveEncodingImpl( std::array< CharacterEncoding, 256>& arr,
			CharacterEncoding& curr ) const;

	bool equals( const ParentNode& other ) const;

private:
	INode::Ptr left;
	INode::Ptr right;
};

/*
================================
Huffman tree. Contains every information necessary in order to encode and
decode information
================================
 */
class Tree
{
public:
	Tree( const std::shared_ptr<INode> head );
	~Tree();

	static Tree CreateNewTree( const struct FileAnalysis& info );

	/*
	 * Outputs the Huffman Encoding tree in a binary form
	 */
	void        EncodeBinary( BitStream& outStream ) const;

	/*
	 * Given a byte, whose encoding we know, output it's binary correspondent
	 */
	void        EncodeByte( BitStream& outStream, unsigned char byte ) const;

	static Tree LoadTree( BitStream& inStream );
	void        Print() const;

	const std::array< CharacterEncoding, 256 > GetEncoding() const;

	bool operator==( const Tree& other ) const;

protected:
	std::shared_ptr< const INode >   head;
	std::array< CharacterEncoding, 256 >    encoding;
};

}

using namespace huffman;

// Specialization of std::greator< INode::Ptr >
template <>
struct std::greater< INode::Ptr >
{
	bool operator() ( const INode::Ptr a, const INode::Ptr b ) {
		if ( a && b ) {
			return *b < *a;
		}
		return b < a;

		// return a && b ? *b < *a : b < a;   // Not even readable
	}
};

