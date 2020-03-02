#include "AnalysisInfoTest.hpp"

#include "HuffmanAlgorithm.hpp"

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

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
	int inputFile = open( inputFileName, O_WRONLY | O_TRUNC | O_CREAT );

	const char* fileContents = "aaaaaaaabbbbccdefghijklmnopqrstuvwxyz";
	write( inputFile, fileContents, strlen( fileContents ) );
	close( inputFile );

	inputFile = open( inputFileName, O_RDONLY );

	// Load AnalysisInfo after reading file
	struct AnalysisInfo info = AnalyseFile( inputFile );
	close( inputFile );

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

