#include <stdio.h>
#include <string.h>
#include "huffman_algo.hpp"
#include "binary_heap.hpp"
#include "bitstream.hpp"

#define BUFFER_SZ 512
#define TRUE 1
#define FALSE 0

HuffmanNode::HuffmanNode(unsigned long weight, bool isLeaf)
    : _weight(weight), _isLeaf(isLeaf) {}

unsigned long HuffmanNode::weight(){
    return _weight;
}

bool HuffmanNode::operator<(HuffmanNode &node){
    return _weight < node._weight;
}

HuffmanLeafNode::HuffmanLeafNode(unsigned long weight, unsigned char elem)
    : HuffmanNode(weight, true),  _elem(elem) {}

HuffmanParentNode::HuffmanParentNode(HuffmanNode* left, HuffmanNode* right)
    : HuffmanNode(left->weight() + right->weight(), false),
    _left(left), _right(right) {}

HuffmanParentNode::~HuffmanParentNode() {
    delete _left;
    delete _right;
}

HuffmanTree::HuffmanTree(HuffmanNode* head){
    _head = head;
}

HuffmanTree::~HuffmanTree(){
    delete _head;
}

void HuffmanNode::print(unsigned depth){
    if(_isLeaf){
        ((HuffmanLeafNode*)this)->print(depth);
    }else{
        ((HuffmanParentNode*)this)->print(depth);
    }
}

void HuffmanLeafNode::print(unsigned depth){
    for(unsigned i=0; i<depth; i++){
        printf("  ");
    }
    printf("\"%d\" - %lu\n", _elem, weight());
}

void HuffmanParentNode::print(unsigned depth){
    _left->print(depth+1);
    for(unsigned i=0; i<depth; i++){
        printf("  ");
    }
    printf("%lu\n", weight());
    _right->print(depth+1);
}

void HuffmanTree::print(){
    _head->print(0);
}

bool huffmanEncode(FILE* inputFile, FILE* outputFile){
    bool result = false;

    fprintf(stderr, "Started analysis... ");
    fflush(stdout);
    struct analysis_info* info = analyse_file(inputFile);
    if(info != NULL){
        fprintf(stderr, "DONE\nCreating Huffman Tree... ");
        HuffmanTree *tree = create_huffman_tree(info);
        if(tree != NULL){
            fprintf(stderr, "DONE\nCreating output bitstream... ");
            BitStream* outStream = new BitStream(outputFile);
            if(outStream != NULL){
                fprintf(stderr, "DONE\n");

                tree->print();

                result = true;
                delete outStream;
            }else{
                fprintf(stderr, "ERROR\n");
            }
            delete tree;
        }else{
            fprintf(stderr, "ERROR\n");
        }
        delete info;
    }else{
        fprintf(stderr, "ERROR\n");
    }
    return result;
}

struct analysis_info* analyse_file(FILE* inputFile){
    struct analysis_info* res = NULL;
    res = new struct analysis_info;
    if(res != NULL){
        memset(res, 0, sizeof(struct analysis_info));

        unsigned char buffer[BUFFER_SZ];
        size_t bytes_read = 0;
        while((bytes_read = fread(buffer, 1, BUFFER_SZ, inputFile)) > 0){
            for(unsigned i=0; i<bytes_read; i++){
                res->frequency[buffer[i]]++;
            }
        }
    }
    return res;
}

HuffmanTree* create_huffman_tree(struct analysis_info* info){
    BinaryHeap<HuffmanNode>* heap = new BinaryHeap<HuffmanNode>(256);
    if(heap == NULL){
        return NULL;
    }

    unsigned long total = 0;
    for(unsigned i=0; i<0x100; i++){
        total += info->frequency[i];
    }

    double sum = 0;
    for(unsigned i=0; i<0x100; i++){
        if(info->frequency[i] > 0){
            unsigned long weight = info->frequency[i];
            HuffmanNode* leaf = new HuffmanLeafNode(weight, i);
            sum += weight;
            heap->insert(leaf);
        }
    }

    while(heap->size() >= 2){
        HuffmanNode* parent = new HuffmanParentNode(heap->pop(), heap->pop());
        heap->insert(parent);
    }

    HuffmanNode* node = heap->pop();
    delete heap;
    return new HuffmanTree(node);
}
