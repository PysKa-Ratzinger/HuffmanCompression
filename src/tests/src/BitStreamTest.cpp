#include "BitStreamTest.hpp"

#include "BitStream.hpp"

#include <unistd.h>

Test* BitStreamTest::Suite()
{
	TestSuite* res = new TestSuite( "BitStreamTest" );
	res->addTest( new TestCaller< BitStreamTest > (
				"SimpleTest",
				&BitStreamTest::SimpleTest ) );
	return res;
}

void BitStreamTest::SimpleTest()
{
	int p[ 2 ];
	pipe( p );

	BitStream b ( p[1] );

	for ( int i = 0; i < 8; i++ ) {
		b.WriteBit( true );
	}

	// TODO: Continue here...
}

