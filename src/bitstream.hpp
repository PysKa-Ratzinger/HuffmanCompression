#pragma once

#include <stdio.h>

class BitStream{
private:
    FILE* _file;
    char _charBuffer;
    size_t _charBufferSize;
public:
    BitStream(FILE* file);
    void writeNBits(unsigned char* buffer, size_t numBits);
    void writeBit(unsigned char bit);
    void flush();
};
