#include <cppunit/TestCase.h>
#include <cppunit/TestCaller.h>
#include <cppunit/TestSuite.h>

using namespace CppUnit;

class HuffmanEncodingTest : public TestCase
{
public:
	static Test* Suite();

	void EncodeByteTest();
	void DecodeByteTest();
	void FrequencyAnalysisTest();
	void HuffmanTreeEncodingTest();
	void LeafPersistTest();
	void ParentPersistTest();
};


