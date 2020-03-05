#include "BitStreamTest.hpp"

#include "BitStream.hpp"

#include <unistd.h>

Test* BitStreamTest::Suite()
{
	TestSuite* res = new TestSuite( "BitStreamTest" );
	res->addTest( new TestCaller< BitStreamTest > (
				"WriteBitTest",
				&BitStreamTest::WriteBitTest ) );
	res->addTest( new TestCaller< BitStreamTest > (
				"WriteBitsTest",
				&BitStreamTest::WriteBitsTest ) );
	res->addTest( new TestCaller< BitStreamTest > (
				"ReadTest",
				&BitStreamTest::ReadTest ) );
	return res;
}

void BitStreamTest::WriteBitTest()
{
	std::stringstream ss;

	BitStream b ( ss );

	for ( int i = 0; i < 8; i++ ) {
		b.WriteBit( i % 2 == 0 );
	}

	const std::string result = ss.str();

	CPPUNIT_ASSERT_EQUAL( result.size(), ( size_t ) 1 );
	CPPUNIT_ASSERT_EQUAL( result[0], ( char ) 0xaa );
}

void BitStreamTest::WriteBitsTest()
{
	CPPUNIT_ASSERT( false );
}

void BitStreamTest::MixedWriteTest()
{
	CPPUNIT_ASSERT( false );
}

void BitStreamTest::ReadTest()
{
	std::stringstream ss;

	ss << "\xaa";

	BitStream b ( ss );

	for ( int i = 0; i < 8; i++ ) {
		bool bit = b.ReadBit();
		CPPUNIT_ASSERT_EQUAL( bit, i % 2 == 0 );
	}
}

