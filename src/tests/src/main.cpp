#include <cppunit/ui/text/TestRunner.h>

#include "BinaryHeapTest.hpp"
#include "BitStreamTest.hpp"
#include "FileAnalysisTest.hpp"
#include "HuffmanAlgorithmTest.hpp"

int main () {
	CppUnit::TextUi::TestRunner runner;

	runner.addTest( BinaryHeapTest::Suite() );
	runner.addTest( BitStreamTest::Suite() );
	runner.addTest( FileAnalysisTest::Suite() );
	runner.addTest( HuffmanAlgorithmTest::Suite() );

	runner.run();
	return 0;
}

