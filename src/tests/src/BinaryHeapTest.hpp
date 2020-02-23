#include <cppunit/TestCase.h>
#include <cppunit/TestCaller.h>
#include <cppunit/TestSuite.h>

using namespace CppUnit;

class BinaryHeapTest : public TestCase
{
public:
	static Test* Suite();

	void SimpleTest();
};

