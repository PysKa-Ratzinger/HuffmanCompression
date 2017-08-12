#pragma once

#include <stdio.h>
#include <stdlib.h>

#include "bitstream.hpp"
#include "character_encoding.hpp"

#define TOTAL_CHARS 0x100

/**  Used to store all the information regarding the frequency of every byte
 * found in the analysed file
 */
struct analysis_info{
    /**  Array with the frequency of every byte. The index of each frequency
     * corresponds to the byte analysed, and the value to the frequency of
     * said byte.
     */
    unsigned long frequency[TOTAL_CHARS];

    /**
     * The total number of bytes in the file
     */
    u_int64_t total;
};

/**  Abstract base class of a Huffman Tree Node. The two subclasses of this
 * base class represent the Leaf Node and the Parent Node.
 */
class HuffmanNode{
private:
    unsigned long _weight;
    bool _isLeaf;
public:
    HuffmanNode(unsigned long weight, bool isLeaf);
    virtual ~HuffmanNode() {}
    unsigned long weight() const;
    bool operator < (const HuffmanNode& node) const;
    virtual void print(unsigned const depth) const;
    virtual void encodeBinary(BitStream* outStream) const = 0;
    virtual void saveEncoding(CharacterEncoding* arr[256],
                                CharacterEncoding* curr) const = 0;
};

/**  Leaf node from a Huffman tree. Represents a node with no childs,
 * but a value.
 */
class HuffmanLeafNode : public HuffmanNode{
private:
    unsigned char _elem;
public:
    HuffmanLeafNode(unsigned long weight, unsigned char elem);
    ~HuffmanLeafNode() {}
    void print(unsigned const depth) const;
    void encodeBinary(BitStream* outStream) const;
    void saveEncoding(CharacterEncoding* arr[256],
                        CharacterEncoding* curr) const;
};

/**  Parent node from a Huffman tree. Represents a node with no value but
 * two children.
 */
class HuffmanParentNode : public HuffmanNode{
private:
    HuffmanNode* _left;
    HuffmanNode* _right;
public:
    HuffmanParentNode(HuffmanNode* left, HuffmanNode* right);
    ~HuffmanParentNode();
    void print(unsigned const depth) const;
    void encodeBinary(BitStream* outStream) const;
    void saveEncoding(CharacterEncoding* arr[256],
                        CharacterEncoding* curr) const;
};

/**  Huffman tree. Contains every information necessary in order to encode and
 * decode information
 */
class HuffmanTree{
private:
    const HuffmanNode* _head;
    CharacterEncoding** _encoding;
public:
    HuffmanTree(const HuffmanNode* head);
    ~HuffmanTree();
    void print() const;
    void encodeBinary(BitStream* outStream) const;
    void encodeByte(BitStream* outStream, unsigned char byte) const;
};

bool huffmanEncode(FILE* inputFile, FILE* outputFile);

/**
 *  Given a file name, an analysis_info structure is created with the frequency
 * of every byte present in that file. If the file cannot be opened or memory
 * allocation for the analysis information is not possible, the operation fails.
 *  If the operation fails, NULL is returned and @{errno} is left with a value
 * that describes the problem that occurred.
 */
struct analysis_info* analyse_file(FILE* inputFile);

/**
 * Given the analysis information of a file, a huffman tree is generated.
 * @param  info Analysis info of a file that was analysed
 * @return      Pointer to a new huffman_tree structure
 */
HuffmanTree* create_huffman_tree(struct analysis_info *info);
