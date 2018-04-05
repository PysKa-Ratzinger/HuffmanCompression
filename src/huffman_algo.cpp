#include <stdio.h>
#include <string.h>
#include "huffman_algo.hpp"
#include "binary_heap.hpp"
#include "bitstream.hpp"
#include "character_encoding.hpp"

#define BUFFER_SZ 512
#define TRUE 1
#define FALSE 0

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
	(byte & 0x80 ? '1' : '0'), \
	(byte & 0x40 ? '1' : '0'), \
	(byte & 0x20 ? '1' : '0'), \
	(byte & 0x10 ? '1' : '0'), \
	(byte & 0x08 ? '1' : '0'), \
	(byte & 0x04 ? '1' : '0'), \
	(byte & 0x02 ? '1' : '0'), \
	(byte & 0x01 ? '1' : '0')

HuffmanNode::HuffmanNode(unsigned long weight, bool isLeaf)
	: _weight(weight), _isLeaf(isLeaf) {}

	unsigned long HuffmanNode::weight() const{
		return _weight;
	}

bool HuffmanNode::operator<(const HuffmanNode &node) const{
	return _weight < node._weight;
}

HuffmanLeafNode::HuffmanLeafNode(unsigned long weight, unsigned char elem)
	: HuffmanNode(weight, true),  _elem(elem) {}

	void HuffmanLeafNode::saveEncoding(CharacterEncoding **arr,
			CharacterEncoding *curr) const{
		arr[_elem] = new CharacterEncoding(*curr);
	}

HuffmanParentNode::HuffmanParentNode(HuffmanNode* left, HuffmanNode* right)
	: HuffmanNode(left->weight() + right->weight(), false),
	_left(left), _right(right) {}

	HuffmanParentNode::~HuffmanParentNode() {
		delete _left;
		delete _right;
	}

void HuffmanParentNode::saveEncoding(CharacterEncoding **arr,
		CharacterEncoding *curr) const{
	curr->addBit(0);
	_left->saveEncoding(arr, curr);
	curr->removeBit();
	curr->addBit(1);
	_right->saveEncoding(arr, curr);
	curr->removeBit();
}

HuffmanTree::HuffmanTree(const HuffmanNode* head) : _head(head){
	_encoding = new CharacterEncoding*[256];
	for(unsigned i=0; i<256; i++){
		_encoding[i] = NULL;
	}
	CharacterEncoding* curr = new CharacterEncoding();
	if(_head != NULL) _head->saveEncoding(_encoding, curr);
	delete curr;
}

HuffmanTree::~HuffmanTree(){
	for(unsigned i=0; i<256; i++){
		if(_encoding[i]) delete _encoding[i];
	}
	delete[] _encoding;
	delete _head;
}

void HuffmanNode::print(unsigned const depth) const{
	if(_isLeaf){
		((HuffmanLeafNode*)this)->print(depth);
	}else{
		((HuffmanParentNode*)this)->print(depth);
	}
}

void HuffmanLeafNode::print(unsigned const depth) const{
	for(unsigned i=0; i<depth; i++){
		printf(" | ");
	}
	printf("\"%d\" - %lu\n", _elem, weight());
}

void HuffmanParentNode::print(unsigned const depth) const{
	_left->print(depth+1);
	for(unsigned i=0; i<depth; i++){
		printf(" | ");
	}
	printf("%lu\n", weight());
	_right->print(depth+1);
}

void HuffmanTree::print() const{
	_head->print(0);
	for(unsigned i=0; i<256; i++){
		if(_encoding[i] != NULL){
			printf("Encoding for %d is: ", i);
			for(unsigned j=0; j<_encoding[i]->getBitSize(); j++){
				printf("%d", _encoding[i]->getBit(j));
			}
			printf("\n");
		}
	}
}

void HuffmanLeafNode::encodeBinary(BitStream *outStream) const{
	outStream->writeBit(0);
	outStream->writeNBits(&_elem, sizeof(_elem)*8);
}

void HuffmanParentNode::encodeBinary(BitStream *outStream) const{
	outStream->writeBit(1);
	_left->encodeBinary(outStream);
	_right->encodeBinary(outStream);
}

void HuffmanTree::encodeBinary(BitStream *outStream) const{
	if(_head) _head->encodeBinary(outStream);
}

void HuffmanTree::encodeByte(BitStream *outStream, unsigned char byte) const{
	CharacterEncoding* encoding = _encoding[byte];
	for(unsigned i=0; i<encoding->getBitSize(); i++){
		outStream->writeBit(encoding->getBit(i));
	}
}

bool huffmanEncode(FILE* inputFile, FILE* outputFile){
	fprintf(stderr, "Started analysis... ");
	fflush(stdout);
	struct analysis_info* info = analyse_file(inputFile);
	if(info == NULL){
		fprintf(stderr, "ERROR\n");
		return false;
	}

	fprintf(stderr, "DONE\nCreating Huffman Tree... ");
	HuffmanTree *tree = create_huffman_tree(info);
	if(tree == NULL){
		fprintf(stderr, "ERROR\n");
		delete info;
		return false;
	}

	fprintf(stderr, "DONE\n");
	fprintf(stderr, "Creating output bitstream... ");
	BitStream* outStream = new BitStream(outputFile);
	if(outStream == NULL){
		fprintf(stderr, "ERROR\n");
		delete tree;
		delete info;
		return false;
	}

	fprintf(stderr, "DONE\n");
	tree->print();

	/* Output file_size and huffman tree */
	fwrite(&info->total, sizeof(info->total), 1, outputFile);
	tree->encodeBinary(outStream);

	/* Set input file position to beginning */
	fseek(inputFile, 0, SEEK_SET);

	char input;
	size_t bytes_read;
	while((bytes_read = fread(&input, 1, 1, inputFile)) > 0){
		tree->encodeByte(outStream, input);
	}

	outStream->flush();

	delete outStream;
	delete tree;
	delete info;
	return true;
}

bool huffmanDecode(FILE* inputFile, FILE* outputFile){
	BitStream* inputStream = new BitStream(inputFile);
	if (inputStream == NULL) {
		return false;
	}

	HuffmanTree* tree = read_huffman_tree(inputStream);
	if (tree == NULL) {
		delete inputStream;
		return false;
	}

	BitStream* outputStream = new BitStream(outputFile);
	if (outputStream == NULL) {
		delete inputStream;
		delete tree;
		return false;
	}

	tree->translate(inputStream, outputStream);
	outputStream->flush();

	delete outputStream;
	delete tree;
	delete inputStream;
	return true;
}

struct analysis_info* analyse_file(FILE* inputFile){
	struct analysis_info* res = NULL;
	res = new struct analysis_info;
	if(res != NULL){
		memset(res, 0, sizeof(struct analysis_info));

		unsigned char buffer[BUFFER_SZ];
		size_t bytes_read = 0;
		while((bytes_read=fread(buffer, 1, BUFFER_SZ, inputFile)) > 0){
			for(unsigned i=0; i<bytes_read; i++){
				res->frequency[buffer[i]]++;
			}
		}

		unsigned long total = 0;
		for(unsigned i=0; i<256; i++){
			total += res->frequency[i];
		}
		res->total = total;
	}
	return res;
}

HuffmanTree* create_huffman_tree(struct analysis_info* info){
	BinaryHeap<HuffmanNode>* heap = new BinaryHeap<HuffmanNode>(256);
	if(heap == NULL){
		return NULL;
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
		HuffmanNode* parent = new HuffmanParentNode(heap->pop(),
				heap->pop());
		heap->insert(parent);
	}

	HuffmanNode* node = heap->pop();
	delete heap;
	return new HuffmanTree(node);
}

HuffmanNode* read_huffman_node(BitStream* inStream) {
	unsigned char bit = inStream->readBit();
	if (bit == 0) {
		unsigned char byte;
		inStream->readNBits(&byte, 8);
		return new HuffmanLeafNode(0, byte);
	} else {
		HuffmanNode* left = read_huffman_node(inStream);
		HuffmanNode* right = read_huffman_node(inStream);
		return new HuffmanParentNode(left, right);
	}
}

HuffmanTree* read_huffman_tree(BitStream* inStream) {
	HuffmanNode* head = read_huffman_node(inStream);
	return new HuffmanTree(head);
}

