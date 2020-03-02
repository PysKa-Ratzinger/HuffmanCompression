#include <cppunit/ui/text/TestRunner.h>

#include "BinaryHeapTest.hpp"
#include "AnalysisInfoTest.hpp"
#include "BitStreamTest.hpp"

int main () {
	CppUnit::TextUi::TestRunner runner;

	runner.addTest( BinaryHeapTest::Suite() );
	runner.addTest( AnalysisInfoTest::Suite() );
	runner.addTest( BitStreamTest::Suite() );

	runner.run();
	return 0;
}

