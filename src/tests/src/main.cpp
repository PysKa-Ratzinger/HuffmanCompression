#include <cppunit/ui/text/TestRunner.h>

#include "BinaryHeapTest.hpp"
#include "AnalysisInfoTest.hpp"

int main () {
	CppUnit::TextUi::TestRunner runner;

	runner.addTest( BinaryHeapTest::Suite() );
	runner.addTest( AnalysisInfoTest::Suite() );

	runner.run();
	return 0;
}

