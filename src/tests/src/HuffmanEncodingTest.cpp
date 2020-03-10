#include "HuffmanEncodingTest.hpp"

#include "HuffmanEncoding.hpp"
#include "FileAnalysis.hpp"

#include <algorithm>

#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>

using namespace huffman;

Test* HuffmanEncodingTest::Suite()
{
	TestSuite* res = new TestSuite( "HuffmanEncodingTest" );
	res->addTest( new TestCaller< HuffmanEncodingTest > (
				"FrequencyAnalysisTest",
				&HuffmanEncodingTest::FrequencyAnalysisTest ) );
	res->addTest( new TestCaller< HuffmanEncodingTest > (
				"LeafPersistTest",
				&HuffmanEncodingTest::LeafPersistTest ) );
	res->addTest( new TestCaller< HuffmanEncodingTest > (
				"ParentPersistTest",
				&HuffmanEncodingTest::ParentPersistTest ) );
	res->addTest( new TestCaller< HuffmanEncodingTest > (
				"HuffmanTreeEncodingTest",
				&HuffmanEncodingTest::HuffmanTreeEncodingTest ) );
	res->addTest( new TestCaller< HuffmanEncodingTest > (
				"EncodeByteTest",
				&HuffmanEncodingTest::EncodeByteTest ) );
	res->addTest( new TestCaller< HuffmanEncodingTest > (
				"DecodeByteTest",
				&HuffmanEncodingTest::DecodeByteTest ) );
	return res;
}

void HuffmanEncodingTest::FrequencyAnalysisTest()
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

void HuffmanEncodingTest::LeafPersistTest()
{
	std::stringstream ss;
	bool bit;

	LeafNode node( 5, 'A' );
	BitStream stream( ss );
	node.PersistImpl( stream );
	stream.Flush();

	BitStream stream2( ss );

	// Leaf node encoding
	CPPUNIT_ASSERT_EQUAL( (bool) 0, stream2.ReadBit( bit ) && bit );

	// 'A' == 0x41 == 0b0100 0001
	for ( bool b : {0, 1, 0, 0, 0, 0, 0, 1} ) {
		CPPUNIT_ASSERT_EQUAL( b, stream2.ReadBit( bit ) && bit );
	}
}

void HuffmanEncodingTest::ParentPersistTest()
{
	std::stringstream ss;
	bool bit;

	auto node1 = std::make_shared< LeafNode >( 5, 'A' );
	auto node2 = std::make_shared< LeafNode >( 6, 'B' );
	ParentNode pnode( node1, node2 );

	{
		BitStream b( ss );
		pnode.Persist( b );
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

void HuffmanEncodingTest::HuffmanTreeEncodingTest()
{

	std::stringstream ss;
	const char* fileContents = "AAAABBCC";

	// Load FileAnalysis after reading file
	struct FileAnalysis info;
	info.FeedText( fileContents, strlen( fileContents ) );

	Tree tree1 = Tree::CreateNewTree( info );

	printf( "\n" );

	{
		BitStream b( ss );
		tree1.Persist( b );

		// This final write should not affect how the huffman tree is read
		// from the file
		b.WriteNBits( "abcd", 32 );
		b.Flush();
	}

	BitStream b( ss );
	Tree tree2 = Tree::LoadTree( b );

	CPPUNIT_ASSERT( tree1 == tree2 );
}

void HuffmanEncodingTest::EncodeByteTest()
{
	std::stringstream ss;
	const char* fileContents = "AAAABCDE";

	// Load FileAnalysis after reading file
	struct FileAnalysis info;
	info.FeedText( fileContents, strlen( fileContents ) );

	Tree tree = Tree::CreateNewTree( info );
	// tree.Print();

	std::stringstream ss2;
	BitStream b( ss2 );
	tree.EncodeByte( b, 'A' );
	tree.EncodeByte( b, 'B' );
	tree.EncodeByte( b, 'C' );
	tree.EncodeByte( b, 'D' );
	tree.EncodeByte( b, 'E' );
	tree.EncodeByte( b, 'A' );
	tree.EncodeByte( b, 'A' );
	tree.EncodeByte( b, 'A' );
	b.Flush();

	/*
	 * Encoding for 65 is: 1
	 * Encoding for 66 is: 000
	 * Encoding for 67 is: 011
	 * Encoding for 68 is: 001
	 * Encoding for 69 is: 010
	 *
	 * ABCDEAAA
	 * 1 000 011 001 010 1 1 1
	 * 1000 0110 0101 0111
	 * \x86\x57
	 */

	CPPUNIT_ASSERT_EQUAL( std::string( "\x86\x57" ), ss2.str() );
}

void HuffmanEncodingTest::DecodeByteTest()
{
	std::stringstream ss;
	const char* fileContents = "AAAABCDE";

	// Load FileAnalysis after reading file
	struct FileAnalysis info;
	info.FeedText( fileContents, strlen( fileContents ) );

	Tree tree = Tree::CreateNewTree( info );

	std::stringstream ss2;
	ss2 << "\x86\x57";
	BitStream inStream( ss2 );

	std::vector< uint8_t > expectedValues = {
		'A', 'B', 'C', 'D', 'E', 'A', 'A', 'A'
	};

	size_t charsProcessed = 0;
	while( true ) {
		try {
			uint8_t c = tree.DecodeByte( inStream );
			CPPUNIT_ASSERT( charsProcessed < expectedValues.size() );
			CPPUNIT_ASSERT_EQUAL( expectedValues[ charsProcessed ], c );
			charsProcessed++;
		} catch ( ... ) {
			break;
		}
	}

	CPPUNIT_ASSERT_EQUAL( expectedValues.size(), charsProcessed );
}

