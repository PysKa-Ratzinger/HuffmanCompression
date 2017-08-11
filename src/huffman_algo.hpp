#pragma once

#include <stdio.h>
#include <stdlib.h>

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
};

class HuffmanNode{
private:
    unsigned long _weight;
    bool _isLeaf;
public:
    HuffmanNode(unsigned long weight, bool isLeaf);
    virtual ~HuffmanNode() {}
    unsigned long weight();
    bool operator < (HuffmanNode& node);
    void print(unsigned depth);
};

class HuffmanLeafNode : public HuffmanNode{
private:
    unsigned char _elem;
public:
    HuffmanLeafNode(unsigned long weight, unsigned char elem);
    ~HuffmanLeafNode() {}
    void print(unsigned depth);
};

class HuffmanParentNode : public HuffmanNode{
private:
    HuffmanNode* _left;
    HuffmanNode* _right;
public:
    HuffmanParentNode(HuffmanNode* left, HuffmanNode* right);
    ~HuffmanParentNode();
    void print(unsigned depth);
};

class HuffmanTree{
private:
    HuffmanNode *_head;
public:
    HuffmanTree(HuffmanNode *head);
    ~HuffmanTree();
    void print();
};

bool huffmanEncode(FILE* inputFile, FILE* outputFile);

/**
 *  Given a file name, an analysis_info structure is created with the frequency
 * of every byte present in that file. If the file cannot be opened or memory
 * allocation for the analysis information is not possible, the operation fails.
 *  If the operation fails, NULL is returned and @{errno} is left with a value
 * that describes the problem that occurred.
 *
 * @param  filename      The name of the file to be analysed
 * @param  filename_size The size of the name of the file to be analysed
 * @return               A pointer to a structure with all the information
 *                         needed from the analysis, or NULL if one could not
 *                         be created.
 */
struct analysis_info* analyse_file(FILE* inputFile);

/**
 * Given the analysis information of a file, a huffman tree is generated.
 * @param  info Analysis info of a file that was analysed
 * @return      Pointer to a new huffman_tree structure
 */
HuffmanTree* create_huffman_tree(struct analysis_info *info);
