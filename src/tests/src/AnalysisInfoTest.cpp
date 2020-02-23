#include "AnalysisInfoTest.hpp"

#include "HuffmanAlgorithm.hpp"

#include <unistd.h>

Test* AnalysisInfoTest::Suite()
{
	TestSuite* res = new TestSuite( "AnalysisInfoTest" );
	res->addTest( new TestCaller< AnalysisInfoTest > (
				"simpleTest",
				&AnalysisInfoTest::SimpleTest ) );
	return res;
}

void AnalysisInfoTest::SimpleTest()
{
	const char* inputFileName = "/tmp/AnalysisInfoTestFile.txt";
	FILE* inputFile = fopen( inputFileName, "w+" );

	fprintf( inputFile, "aaaaaaaabbbbccdefghijklmnopqrstuvwxyz" );
	fclose( inputFile );

	inputFile = fopen( inputFileName, "r" );
	struct AnalysisInfo info = AnalyseFile( inputFile );
	fclose( inputFile );
	unlink( inputFileName );

	unsigned long expectedFrequencies[256] { 0 };
	expectedFrequencies[(int)'a'] = 8;
	expectedFrequencies[(int)'b'] = 4;
	expectedFrequencies[(int)'c'] = 2;
	for ( char c = 'd'; c <= 'z'; c++ ) {
		expectedFrequencies[(int)c] = 1;
	}

	for ( int i = 0; i < 256; i++ ) {
		CPPUNIT_ASSERT_EQUAL( expectedFrequencies[i], info.frequency[i] );
	}

	CPPUNIT_ASSERT_EQUAL( (unsigned long) 37, info.total );
}

