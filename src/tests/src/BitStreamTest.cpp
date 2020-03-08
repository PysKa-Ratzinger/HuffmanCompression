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
				"MixedWriteTest",
				&BitStreamTest::MixedWriteTest ) );
	res->addTest( new TestCaller< BitStreamTest > (
				"ReadTest",
				&BitStreamTest::ReadTest ) );
	return res;
}

void BitStreamTest::WriteBitTest()
{
	std::stringstream ss;

	BitStream b( ss );

	// 100 100 100 100 100 1
	// 1001 0010 0100 1001 -> \x92\x49
	for ( int i = 0; i < 16; i++ ) {
		b.WriteBit( i % 3 == 0 );
	}

	const std::string result = ss.str();

	CPPUNIT_ASSERT_EQUAL( result.size(), static_cast< size_t >( 2 ) );
	CPPUNIT_ASSERT_EQUAL( (uint8_t) result[0], (uint8_t)'\x92' );
	CPPUNIT_ASSERT_EQUAL( (uint8_t) result[1], (uint8_t)'\x49' );
}

void BitStreamTest::WriteBitsTest()
{
	std::stringstream ss;

	BitStream b( ss );

	std::string testString( "Hello world!" );
	b.WriteNBits( testString );

	const std::string result = ss.str();

	CPPUNIT_ASSERT_EQUAL( static_cast< size_t >( testString.size() ), result.size() );
	CPPUNIT_ASSERT_EQUAL( testString, result );
}

void BitStreamTest::MixedWriteTest()
{
	std::stringstream ss;
	std::string testString( "A mysterious wolf" );

	{
		BitStream b( ss );

		b.WriteBit( true );
		b.WriteNBits( testString );

		b.WriteBit( true );
		b.WriteBit( true );
		b.WriteBit( false );
		b.WriteBit( false );
		b.WriteBit( true );

		b.Flush();
	}

	BitStream b2( ss );

	CPPUNIT_ASSERT_EQUAL( true, b2.ReadBit() );

	char buffer[ 64 ];
	b2.ReadNBits( buffer, testString.size() * 8 );
	CPPUNIT_ASSERT_EQUAL( testString, std::string( buffer ) );

	CPPUNIT_ASSERT_EQUAL( true, b2.ReadBit() );
	CPPUNIT_ASSERT_EQUAL( true, b2.ReadBit() );
	CPPUNIT_ASSERT_EQUAL( false, b2.ReadBit() );
	CPPUNIT_ASSERT_EQUAL( false, b2.ReadBit() );
	CPPUNIT_ASSERT_EQUAL( true, b2.ReadBit() );

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

