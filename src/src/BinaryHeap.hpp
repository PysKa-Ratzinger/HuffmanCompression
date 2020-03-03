#pragma once

#include <algorithm>
#include <vector>
#include <stdexcept>

#include <stdlib.h>

template < typename T >
class BinaryHeap
{
public:
	BinaryHeap( size_t startingHeapSize );
	~BinaryHeap();

	size_t Size() const;
	void   PrintArr() const;
	bool   Insert( const T& elem );
	T      Pop();

private:
	std::vector< T > heap;
};

template < typename T >
BinaryHeap<T>::BinaryHeap( size_t startingHeapSize )
{
	heap.reserve( startingHeapSize );
}

template < typename T >
BinaryHeap<T>::~BinaryHeap<T>() { }

template < typename T >
bool BinaryHeap<T>::Insert( const T& elem)
{
	heap.push_back( elem );
	std::push_heap( heap.begin(), heap.end(), std::greater<>{} );
	return true;
}

template < typename T >
T BinaryHeap<T>::Pop()
{
	if ( heap.empty() ) {
		throw std::range_error( "Heap is empty" );
	}

	std::pop_heap( heap.begin(), heap.end(), std::greater<>{} );

	T res = heap.back();
	heap.pop_back();
	return res;
}

template < typename T >
size_t BinaryHeap<T>::Size() const
{
	return heap.size();
}

template < typename T >
void BinaryHeap<T>::PrintArr() const
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

