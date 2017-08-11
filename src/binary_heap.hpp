#pragma once

#include <stdlib.h>

// ####  #####  #### #
// #   # #     #     #
// #   # ###   #     #
// #   # #     #     #
// ####  #####  #### ##### #

template <typename T>
class BinaryHeap{
private:
    T** _heap;
    size_t _heap_size;
    size_t _num_elems;
    void switchPos(size_t pos1, size_t pos2);
public:
    BinaryHeap(size_t starting_heap_size);
    ~BinaryHeap();
    bool insert(T* elem);
    T* pop();
    size_t size();
};

// ##### #   # ####  #
//   #   ## ## #   # #
//   #   # # # ####  #
//   #   #   # #     #
// ##### #   # #     ##### #

template <typename T>
BinaryHeap<T>::BinaryHeap(size_t starting_heap_size){
    // _heap = malloc(starting_heap_size * sizeof(T));
    _heap = new T*[starting_heap_size];
    _heap_size = starting_heap_size;
    _num_elems = 0;
}

template <typename T>
BinaryHeap<T>::~BinaryHeap<T>(){
    delete _heap;
}

template <typename T>
void BinaryHeap<T>::switchPos(size_t pos1, size_t pos2){
    T* temp = _heap[pos1];
    _heap[pos1] = _heap[pos2];
    _heap[pos2] = temp;
}

template <typename T>
bool BinaryHeap<T>::insert(T* elem){
    if(_num_elems == _heap_size) return false;
    size_t pos = _num_elems++;
    _heap[pos] = elem;
    size_t topPos = (pos - 1) / 2;
    while(pos != 0){
        if(*_heap[pos] < *_heap[topPos]){
            switchPos(topPos, pos);
            pos = topPos;
            topPos = (pos - 1) / 2;
        }else{
            break;
        }
    }
    return true;
}

template <typename T>
T* BinaryHeap<T>::pop(){
    if(_num_elems == 0) return NULL;
    T* res = _heap[0];
    _heap[0] = _heap[--_num_elems];
    size_t pos = 0;
    while(true){
        size_t left = pos*2 + 1;
        size_t right = left + 1;
        size_t target;
        if(right < _num_elems){ // Both children are present
            target = *_heap[left] < *_heap[right] ? left : right;
        }else if(left < _num_elems){ // Only left child is present
            target = left;
        }else{
            break;
        }
        if(*_heap[target] < *_heap[pos]){
            switchPos(target, pos);
            pos = target;
        }else{
            break;
        }
    }
    return res;
}

template <typename T>
size_t BinaryHeap<T>::size(){
    return _num_elems;
}
