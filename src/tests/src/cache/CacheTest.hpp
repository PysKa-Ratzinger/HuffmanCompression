#include <cppunit/TestCase.h>
#include <cppunit/TestCaller.h>
#include <cppunit/TestSuite.h>

#include "cache/Cache.hpp"

using namespace CppUnit;

class CacheTest : public TestCase {
public:
	static Test* suite();

	void simpleTest();
};


