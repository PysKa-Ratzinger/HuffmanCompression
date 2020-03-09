#include "HuffmanAlgorithmTest.hpp"

#include "HuffmanEncoding.hpp"
#include "FileAnalysis.hpp"

#include <algorithm>

#include <fcntl.h>
#include <string.h>
#include <unistd.h>

using namespace huffman;

Test* HuffmanAlgorithmTest::Suite()
{
	TestSuite* res = new TestSuite( "HuffmanAlgorithmTest" );
	res->addTest( new TestCaller< HuffmanAlgorithmTest > (
				"FrequencyAnalysisTest",
				&HuffmanAlgorithmTest::FrequencyAnalysisTest ) );
	res->addTest( new TestCaller< HuffmanAlgorithmTest > (
				"LeafEncodeBinaryTest",
				&HuffmanAlgorithmTest::LeafEncodeBinaryTest ) );
	res->addTest( new TestCaller< HuffmanAlgorithmTest > (
				"ParentEncodeBinaryTest",
				&HuffmanAlgorithmTest::ParentEncodeBinaryTest ) );
	res->addTest( new TestCaller< HuffmanAlgorithmTest > (
				"HuffmanTreeEncodingTest",
				&HuffmanAlgorithmTest::HuffmanTreeEncodingTest ) );
	return res;
}

void HuffmanAlgorithmTest::FrequencyAnalysisTest()
{
	std::array< size_t, 256 > freqs { 0 };

	freqs[0] = 16;
	freqs[1] = 4;
	freqs[2] = 4;
	freqs[3] = 4;
	freqs[4] = 4;

	FileAnalysis info( freqs );
	Tree tree = Tree::CreateNewTree( info );

	std::array< CharacterEncoding, 256 > expectedValues;

	expectedValues.at(0).AddBit(1);
	expectedValues.at(1).AddBits({0, 0, 0});
	expectedValues.at(2).AddBits({0, 1, 1});
	expectedValues.at(3).AddBits({0, 0, 1});
	expectedValues.at(4).AddBits({0, 1, 0});

	auto encoding = tree.GetEncoding();

	for ( int i = 0; i < 256; i++ ) {
		CPPUNIT_ASSERT( expectedValues.at(i) == encoding.at(i) );
	}
}

void HuffmanAlgorithmTest::LeafEncodeBinaryTest()
{
	std::stringstream ss;
	bool bit;

	LeafNode node( 5, 'A' );
	BitStream stream( ss );
	node.EncodeBinaryImpl( stream );
	stream.Flush();

	BitStream stream2( ss );

	// Leaf node encoding
	CPPUNIT_ASSERT_EQUAL( (bool) 0, stream2.ReadBit( bit ) && bit );

	// 'A' == 0x41 == 0b0100 0001
	for ( bool b : {0, 1, 0, 0, 0, 0, 0, 1} ) {
		CPPUNIT_ASSERT_EQUAL( b, stream2.ReadBit( bit ) && bit );
	}
}

void HuffmanAlgorithmTest::ParentEncodeBinaryTest()
{
	std::stringstream ss;
	bool bit;

	auto node1 = std::make_shared< LeafNode >( 5, 'A' );
	auto node2 = std::make_shared< LeafNode >( 6, 'B' );
	ParentNode pnode( node1, node2 );

	{
		BitStream b( ss );
		pnode.EncodeBinary( b );
		b.Flush();
	}

	BitStream b2( ss );

	// Parent node encoding
	CPPUNIT_ASSERT_EQUAL( (bool) 1, b2.ReadBit( bit ) && bit );

	// Leaf node encoding
	CPPUNIT_ASSERT_EQUAL( (bool) 0, b2.ReadBit( bit ) && bit );
	// 'A' == 0x41 == 0b0100 0001
	for ( bool b : {0, 1, 0, 0, 0, 0, 0, 1} ) {
		CPPUNIT_ASSERT_EQUAL( b, b2.ReadBit( bit ) && bit );
	}

	// Leaf node encoding
	CPPUNIT_ASSERT_EQUAL( (bool) 0, b2.ReadBit( bit ) && bit );
	// 'B' == 0x42 == 0b0100 0010
	for ( bool b : {0, 1, 0, 0, 0, 0, 1, 0} ) {
		CPPUNIT_ASSERT_EQUAL( b, b2.ReadBit( bit ) && bit );
	}
}

void HuffmanAlgorithmTest::HuffmanTreeEncodingTest()
{
	std::stringstream ss;
	const char* fileContents = "AAAABBCC";

	// Load FileAnalysis after reading file
	struct FileAnalysis info;
	info.FeedText( fileContents, strlen( fileContents ) );

	Tree tree1 = Tree::CreateNewTree( info );

	printf( "\n" );
	tree1.Print();

	{
		BitStream b( ss );
		tree1.EncodeBinary( b );
		b.Flush();
	}

	BitStream b( ss );
	Tree tree2 = Tree::LoadTree( b );

	CPPUNIT_ASSERT( tree1 == tree2 );
}

