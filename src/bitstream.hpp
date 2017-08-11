#pragma once

#include <stdio.h>

class BitStream{
private:
    FILE* _file;
public:
    BitStream(FILE* file);
};
