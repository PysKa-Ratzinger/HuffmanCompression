#include "CharacterEncodingTest.hpp"

#include "CharacterEncoding.hpp"

Test* CharacterEncodingTest::Suite()
{
	TestSuite* res = new TestSuite( "HuffmanAlgorithmTest" );
	res->addTest( new TestCaller< CharacterEncodingTest > (
				"SimpleTest",
				&CharacterEncodingTest::SimpleTest ) );
	return res;
}

void CharacterEncodingTest::SimpleTest()
{
	CharacterEncoding c;

	for ( int i = 0; i < 7; i++ ) {
		c.AddBit( i < 6 );
	}

	CPPUNIT_ASSERT_EQUAL( (size_t) 7, c.GetNumBits() );
	CPPUNIT_ASSERT_EQUAL( (uint8_t) '\xfc', *c.GetRaw() );

	c.RemoveBit();
	c.RemoveBit();
	CPPUNIT_ASSERT_EQUAL( (size_t) 5, c.GetNumBits() );
	CPPUNIT_ASSERT_EQUAL( (uint8_t) '\xf8', *c.GetRaw() );
	c.RemoveBit();
	CPPUNIT_ASSERT_EQUAL( (size_t) 4, c.GetNumBits() );
	CPPUNIT_ASSERT_EQUAL( (uint8_t) '\xf0', *c.GetRaw() );
	c.RemoveBit();
	CPPUNIT_ASSERT_EQUAL( (size_t) 3, c.GetNumBits() );
	CPPUNIT_ASSERT_EQUAL( (uint8_t) '\xe0', *c.GetRaw() );
	c.RemoveBit();
	CPPUNIT_ASSERT_EQUAL( (size_t) 2, c.GetNumBits() );
	CPPUNIT_ASSERT_EQUAL( (uint8_t) '\xc0', *c.GetRaw() );
	c.RemoveBit();
	CPPUNIT_ASSERT_EQUAL( (size_t) 1, c.GetNumBits() );
	CPPUNIT_ASSERT_EQUAL( (uint8_t) '\x80', *c.GetRaw() );
	c.RemoveBit();
	CPPUNIT_ASSERT_EQUAL( (size_t) 0, c.GetNumBits() );
}

