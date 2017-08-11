#include <stdio.h>
#include <string.h>
#include "huffman_algo.hpp"
#include "binary_heap.hpp"

#define BUFFER_SZ 512
#define TRUE 1
#define FALSE 0

HuffmanNode::HuffmanNode(double weight, bool isLeaf)
    : _weight(weight), _isLeaf(isLeaf) {}

double HuffmanNode::weight(){
    return _weight;
}

bool HuffmanNode::operator<(HuffmanNode &node){
    return _weight < node._weight;
}

HuffmanLeafNode::HuffmanLeafNode(double weight, unsigned char elem)
    : HuffmanNode(weight, true),  _elem(elem) {}

HuffmanParentNode::HuffmanParentNode(HuffmanNode& left, HuffmanNode& right)
    : HuffmanNode(left.weight() + right.weight(), false),
    _left(left), _right(right) {}

HuffmanTree::HuffmanTree(HuffmanNode* head){
    _head = head;
}

void HuffmanNode::print(int depth){
    if(_isLeaf){
        ((HuffmanLeafNode*)this)->print(depth);
    }else{
        ((HuffmanParentNode*)this)->print(depth);
    }
}

void HuffmanLeafNode::print(int depth){
    for(unsigned i=0; i<depth; i++){
        printf("  ");
    }
    printf("\"%d\" - %.3f\n", _elem, weight());
}

void HuffmanParentNode::print(int depth){
    _left.print(depth+1);
    for(unsigned i=0; i<depth; i++){
        printf("  ");
    }
    printf("%.3f\n", weight());
    _right.print(depth+1);
}

void HuffmanTree::print(){
    _head->print(0);
}

struct analysis_info* analyse_file(char* filename){
    FILE* input_file;
    struct analysis_info* res = NULL;

    input_file = fopen(filename, "r");
    if(input_file != NULL){

        res = (struct analysis_info*)malloc(sizeof(struct analysis_info));
        if(res != NULL){
            memset(res, 0, sizeof(struct analysis_info));

            unsigned char buffer[BUFFER_SZ];
            size_t bytes_read = 0;
            while((bytes_read = fread(buffer, 1, BUFFER_SZ, input_file)) > 0){
                for(unsigned i=0; i<bytes_read; i++){
                    res->frequency[buffer[i]]++;
                }
            }
        }
        fclose(input_file);
    }
    return res;
}

HuffmanTree* create_huffman_tree(struct analysis_info* info){
    BinaryHeap<HuffmanNode>* heap = new BinaryHeap<HuffmanNode>(256);
    if(heap == NULL){
        return NULL;
    }
    printf("Heap created.\n");

    unsigned long total = 0;
    for(unsigned i=0; i<0x100; i++){
        total += info->frequency[i];
    }
    printf("Total frequencies analysed.\n");

    double sum = 0;
    for(unsigned i=0; i<0x100; i++){
        if(info->frequency[i] > 0){
            double weight = (double)info->frequency[i] / total;
            HuffmanNode* leaf = new HuffmanLeafNode(weight, i);
            printf("Created leaf node with args (%f, %d)\n", weight, i);
            sum += weight;
            heap->insert(leaf);
            printf("Inserted into heap.\n");
        }
    }
    printf("\nSum of leaf weights: %.2f\n\n", sum);

    while(heap->size() >= 2){
        HuffmanNode* parent = new HuffmanParentNode(*heap->pop(), *heap->pop());
        printf("Popped two elements from heap.\n");
        heap->insert(parent);
        printf("Inserted parent node into heap.\n");
    }

    HuffmanNode* node = heap->pop();
    delete heap;
    printf("HuffmanTree created.\n");
    return new HuffmanTree(node);
}
