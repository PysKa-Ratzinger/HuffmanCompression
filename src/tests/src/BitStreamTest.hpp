#include <cppunit/TestCase.h>
#include <cppunit/TestCaller.h>
#include <cppunit/TestSuite.h>

using namespace CppUnit;

class BitStreamTest : public TestCase
{
public:
	static Test* Suite();

	void WriteBitTest();
	void WriteBitsTest();
	void MixedWriteTest();

	void ReadTest();
};



