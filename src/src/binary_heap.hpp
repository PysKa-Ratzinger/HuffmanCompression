#pragma once

#include <algorithm>
#include <vector>
#include <stdexcept>

#include <stdlib.h>

template < typename T >
class BinaryHeap
{
public:
	BinaryHeap( size_t starting_heap_size );
	~BinaryHeap();
	bool Insert( const T& elem );
	T Pop();
	size_t Size() const;

	void PrintArr() const;

private:
	std::vector< T > _heap;
	size_t curr_size;
};

template < typename T >
BinaryHeap<T>::BinaryHeap( size_t starting_heap_size )
		: _heap( starting_heap_size )
		, curr_size( 0 ) { }

template < typename T >
BinaryHeap<T>::~BinaryHeap<T>() { }

template < typename T >
bool BinaryHeap<T>::Insert( const T& elem)
{
	if ( curr_size < _heap.size() ) {
		_heap[curr_size] = elem;
	} else {
		_heap.push_back( elem );
	}
	curr_size++;

	size_t pos = Size() - 1;
	size_t topPos = (pos - 1) / 2;
	while(pos != 0){
		if(_heap[pos] < _heap[topPos]){
			std::swap( _heap[topPos], _heap[pos] );
			pos = topPos;
			topPos = (pos - 1) / 2;
		}else{
			break;
		}
	}
	return true;
}

template < typename T >
T BinaryHeap<T>::Pop()
{
	if ( _heap.empty() ) {
		throw std::range_error( "Heap is empty" );
	}

	T res = _heap[0];
	curr_size--;
	_heap[0] = _heap.at( curr_size );

	size_t pos = 0;
	while( true ){
		size_t left = pos*2 + 1;
		size_t right = left + 1;
		size_t target;
		if( right < Size() ) {
			// Both children are present
			target = _heap[left] < _heap[right] ? left : right;
		} else if ( left < Size() ) {
			// Only left child is present
			target = left;
		} else {
			break;
		}

		if ( _heap[target] < _heap[pos] ) {
			std::swap( _heap[target], _heap[pos] );
			pos = target;
		}else{
			break;
		}
	}
	return res;
}

template < typename T >
size_t BinaryHeap<T>::Size() const
{
	return curr_size;
}

template < typename T >
void BinaryHeap<T>::PrintArr() const
{
	printf( "[" );
	bool first = true;
	for ( size_t i = 0; i < Size(); i++ )
	{
		int n = _heap.at( i );
		printf( "%s%d", first ? "" : ", ", n );
		first = false;
	}
	printf( "]\n" );
}

