#include "HuffmanAlgorithmTest.hpp"

#include "HuffmanAlgorithm.hpp"
#include "FileAnalysis.hpp"

#include <algorithm>

Test* HuffmanAlgorithmTest::Suite()
{
	TestSuite* res = new TestSuite( "HuffmanAlgorithmTest" );
	res->addTest( new TestCaller< HuffmanAlgorithmTest > (
				"simpleTest",
				&HuffmanAlgorithmTest::SimpleTest ) );
	return res;
}

void HuffmanAlgorithmTest::SimpleTest()
{
	std::array< size_t, 256 > freqs { 0 };

	freqs[0] = 16;
	freqs[1] = 4;
	freqs[2] = 4;
	freqs[3] = 4;
	freqs[4] = 4;

	FileAnalysis info( freqs );

	HuffmanTree tree = CreateHuffmanTree( info );

	tree.Print();
}

