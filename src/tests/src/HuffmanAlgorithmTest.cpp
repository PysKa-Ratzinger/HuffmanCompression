#include "HuffmanAlgorithmTest.hpp"

#include "HuffmanAlgorithm.hpp"
#include "FileAnalysis.hpp"

#include <algorithm>

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
	HuffmanTree tree = CreateHuffmanTree( info );

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

	HuffmanLeafNode node( 5, 'A' );
	BitStream stream( ss );
	node.EncodeBinaryImpl( stream );
	stream.Flush();

	BitStream stream2( ss );

	// Leaf node encoding
	CPPUNIT_ASSERT_EQUAL( (bool) 0, stream2.ReadBit() );

	// 'A' == 0x41 == 0b0010 0001
	for ( bool b : {0, 0, 1, 0, 0, 0, 0, 1} ) {
		// CPPUNIT_ASSERT_EQUAL( b, stream.ReadBit() );
		(void) b;
		printf( "Bit: %s\n", stream2.ReadBit() ? "1" : "0" );
	}
}

void HuffmanAlgorithmTest::ParentEncodeBinaryTest()
{
	CPPUNIT_ASSERT( false );
}

