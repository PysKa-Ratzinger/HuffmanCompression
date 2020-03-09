#pragma once

#include <algorithm>
#include <vector>
#include <stdexcept>

#include <stdlib.h>

template < typename T, class Compare = std::greater< T > >
class BinaryHeap
{
public:
	BinaryHeap( size_t startingHeapSize );
	BinaryHeap( const std::vector< T >& initialElements );
	~BinaryHeap();

	size_t Size() const;
	void   PrintArr() const;
	bool   Insert( const T& elem );
	T      Pop();

private:
	std::vector< T > heap;
};

template < typename T, class Compare >
BinaryHeap<T, Compare>::BinaryHeap( size_t startingHeapSize )
{
	heap.reserve( startingHeapSize );
}

template < typename T, class Compare >
BinaryHeap<T, Compare>::BinaryHeap( const std::vector< T >& initialElements )
		: heap( initialElements )
{
	std::make_heap( heap.begin(), heap.end(), Compare{} );
}

template < typename T, class Compare >
BinaryHeap<T, Compare>::~BinaryHeap<T, Compare>() { }

template < typename T, class Compare >
bool BinaryHeap<T, Compare>::Insert( const T& elem)
{
	heap.push_back( elem );
	std::push_heap( heap.begin(), heap.end(), Compare{} );
	return true;
}

template < typename T, class Compare >
T BinaryHeap<T, Compare>::Pop()
{
	if ( heap.empty() ) {
		throw std::range_error( "Heap is empty" );
	}

	std::pop_heap( heap.begin(), heap.end(), Compare{} );

	T res = heap.back();
	heap.pop_back();
	return res;
}

template < typename T, class Compare >
size_t BinaryHeap<T, Compare>::Size() const
{
	return heap.size();
}

template < typename T, class Compare >
void BinaryHeap<T, Compare>::PrintArr() const
{
	printf( "[" );
	bool first = true;
	for ( size_t i = 0; i < Size(); i++ )
	{
		int n = heap.at( i );
		printf( "%s%d", first ? "" : ", ", n );
		first = false;
	}
	printf( "]\n" );
}

