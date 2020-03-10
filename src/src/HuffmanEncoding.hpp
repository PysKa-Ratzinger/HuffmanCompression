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
	using ConstPtr = std::shared_ptr< const INode >;

	virtual ~INode() {}

	virtual uint64_t Weight() const = 0;
	virtual void     Print( unsigned depth ) const = 0;
	virtual size_t   Persist( BitStream& outStream ) const = 0;
	virtual void     SaveEncoding( 
			std::array< CharacterEncoding, 256>& arr,
			CharacterEncoding& curr ) const = 0;

	virtual bool     IsLeaf() const = 0;
	virtual uint8_t  GetElem() const = 0;
	virtual ConstPtr GetLeftChild() const = 0;
	virtual ConstPtr GetRightChild() const = 0;

	bool operator < ( const INode& other ) const
	{
		return Weight() < other.Weight();
	}

	virtual bool operator==( const INode& other ) const = 0;
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

	size_t Persist( BitStream& outStream ) const override {
		return static_cast<const Deriv&>(*this).PersistImpl( outStream );
	}

	void SaveEncoding( std::array< CharacterEncoding, 256>& arr,
			CharacterEncoding& curr ) const override {
		static_cast<const Deriv&>(*this).SaveEncodingImpl( arr, curr );
	}

	bool operator==( const INode& other ) const override {
		const Deriv* o = dynamic_cast<const Deriv*>(&other);
		return o != nullptr && static_cast<const Deriv&>(*this) == *o;
	}

	bool IsLeaf() const override {
		return static_cast<const Deriv&>(*this).IsLeafImpl();
	}

	uint8_t  GetElem() const override {
		return static_cast<const Deriv&>(*this).GetElemImpl();
	}

	ConstPtr GetLeftChild() const override {
		return static_cast<const Deriv&>(*this).GetLeftChildImpl();
	}

	ConstPtr GetRightChild() const override {
		return static_cast<const Deriv&>(*this).GetRightChildImpl();
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
	LeafNode( unsigned long weight, uint8_t elem )
			: Node< LeafNode >( weight )
			, elem( elem )
	{

	}

	~LeafNode() { }

	void   PrintImpl( unsigned depth ) const;
	size_t PersistImpl( BitStream& outStream ) const;
	void   SaveEncodingImpl( std::array< CharacterEncoding, 256>& arr,
			CharacterEncoding& curr ) const;

	bool     IsLeafImpl() const { return true; }
	uint8_t  GetElemImpl() const { return elem; }
	ConstPtr GetLeftChildImpl() const { return nullptr; }
	ConstPtr GetRightChildImpl() const { return nullptr; }

	bool operator==( const LeafNode& other ) const;

private:
	uint8_t elem;
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

	void   PrintImpl( unsigned depth ) const;
	size_t PersistImpl( BitStream& outStream ) const;
	void   SaveEncodingImpl( std::array< CharacterEncoding, 256>& arr,
			CharacterEncoding& curr ) const;

	bool     IsLeafImpl() const { return false; }
	uint8_t  GetElemImpl() const { return 0x00; }
	ConstPtr GetLeftChildImpl() const { return this->left; }
	ConstPtr GetRightChildImpl() const { return this->right; }

	bool operator==( const ParentNode& other ) const;

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

	static Tree   CreateNewTree( const struct FileAnalysis& info );
	static Tree   LoadTree( BitStream& inStream );

	size_t        Persist( BitStream& outStream ) const;
	void          EncodeByte( BitStream& outStream, uint8_t byte ) const;
	uint8_t       DecodeByte( BitStream& inStream ) const;
	void          Print() const;

	bool          operator==( const Tree& other ) const;

	const std::array< CharacterEncoding, 256 > GetEncoding() const;

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

