#include <stdio.h>
#include <string.h>
#include "huffman_algo.hpp"

#define BUFFER_SZ 512

int main(){
    char buffer[BUFFER_SZ];

    printf("Name of the file to be analysed:");
    fflush(stdout);

    fgets(buffer, BUFFER_SZ, stdin);
    buffer[strlen(buffer)-1] = '\0';

    printf("Started analysis... ");
    fflush(stdout);
    struct analysis_info* info = analyse_file(buffer);
    if(info != NULL){
        printf("DONE\nCreating Huffman Tree... \n");
        HuffmanTree *tree = create_huffman_tree(info);
        if(tree != NULL){
            tree->print();
            delete tree;
        }
        free(info);
    }else{
        printf("ERROR\n");
    }

    return 0;
}
