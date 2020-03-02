#include "BitStreamTest.hpp"

#include "BitStream.hpp"

#include <unistd.h>

Test* BitStreamTest::Suite()
{
	TestSuite* res = new TestSuite( "BitStreamTest" );
	res->addTest( new TestCaller< BitStreamTest > (
				"WriteTest",
				&BitStreamTest::WriteTest ) );
	res->addTest( new TestCaller< BitStreamTest > (
				"ReadTest",
				&BitStreamTest::ReadTest ) );
	return res;
}

void BitStreamTest::WriteTest()
{
	int p[ 2 ];
	pipe( p );

	BitStream b ( p[1] );

	for ( int i = 0; i < 8; i++ ) {
		b.WriteBit( i % 2 == 0 );
	}

	uint8_t buf[64];
	ssize_t nbytes = read( p[0], buf, 64 );

	CPPUNIT_ASSERT_EQUAL( nbytes, ( ssize_t ) 1 );
	CPPUNIT_ASSERT_EQUAL( buf[0], ( uint8_t ) 0xaa );

	close( p[0] );
	close( p[1] );
}

void BitStreamTest::ReadTest()
{
	int p[ 2 ];
	pipe( p );

	write( p[1], "\xaa", 1 );

	BitStream b ( p[0] );

	for ( int i = 0; i < 8; i++ ) {
		bool bit = b.ReadBit();
		CPPUNIT_ASSERT_EQUAL( bit, i % 2 == 0 );
	}

	close( p[0] );
	close( p[1] );
}

