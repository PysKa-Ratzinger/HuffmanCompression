#include <cppunit/ui/text/TestRunner.h>

#include "BinaryHeapTest.hpp"
#include "BitStreamTest.hpp"
#include "FileAnalysisTest.hpp"
#include "HuffmanEncodingTest.hpp"
#include "CharacterEncodingTest.hpp"

int main () {
	CppUnit::TextUi::TestRunner runner;

	runner.addTest( BinaryHeapTest::Suite() );
	runner.addTest( BitStreamTest::Suite() );
	runner.addTest( FileAnalysisTest::Suite() );
	runner.addTest( HuffmanEncodingTest::Suite() );
	runner.addTest( CharacterEncodingTest::Suite() );

	runner.run();
	return 0;
}

