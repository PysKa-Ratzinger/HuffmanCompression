#include "BinaryHeapTest.hpp"

#include "BinaryHeap.hpp"

#include <algorithm>

Test* BinaryHeapTest::Suite()
{
	TestSuite* res = new TestSuite( "BinaryHeapTest" );
	res->addTest( new TestCaller< BinaryHeapTest > (
				"simpleTest",
				&BinaryHeapTest::SimpleTest ) );
	return res;
}

void BinaryHeapTest::SimpleTest()
{
	BinaryHeap< int > heap( 5 );

	std::vector< int > randArr;
	for ( int i = 0; i < 20; i++ ) {
		randArr.push_back( i+1 );
	}
	std::random_shuffle( randArr.begin(), randArr.end() );

	for ( int n : randArr ) {
		heap.Insert( n );
	}

	CPPUNIT_ASSERT_EQUAL( (size_t) 20, heap.Size() );

	for ( int i = 0; i < 20; i++ ) {
		int n = heap.Pop();
		CPPUNIT_ASSERT_EQUAL( i + 1, n );
		CPPUNIT_ASSERT_EQUAL( (size_t) ( 20 - i - 1 ), heap.Size() );
	}

	CPPUNIT_ASSERT_EQUAL( (size_t) 0, heap.Size() );
}

