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

	size_t pos = Size() - 1;
	size_t topPos = (pos - 1) / 2;
	while ( pos != 0 ) {
		if ( heap[pos] < heap[topPos] ) {
			std::swap( heap[topPos], heap[pos] );
			pos = topPos;
			topPos = (pos - 1) / 2;
		} else {
			break;
		}
	}
	return true;
}

template < typename T >
T BinaryHeap<T>::Pop()
{
	if ( heap.empty() ) {
		throw std::range_error( "Heap is empty" );
	}

	T res = heap[0];
	heap[0] = heap.back();
	heap.pop_back();

	size_t pos = 0;
	while( true ){
		size_t left = pos*2 + 1;
		size_t right = left + 1;
		size_t target;
		if( right < Size() ) {
			// Both children are present
			target = heap[left] < heap[right] ? left : right;
		} else if ( left < Size() ) {
			// Only left child is present
			target = left;
		} else {
			break;
		}

		if ( heap[target] < heap[pos] ) {
			std::swap( heap[target], heap[pos] );
			pos = target;
		} else {
			break;
		}
	}
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

