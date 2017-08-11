#include <stdio.h>
#include <string.h>
#include "huffman_algo.hpp"
#include "bitstream.hpp"

#define BUFFER_SZ 512

int main(){
    char buffer[BUFFER_SZ];

    printf("Name of the file to be analysed: ");
    fflush(stdout);
    fgets(buffer, BUFFER_SZ, stdin);
    buffer[strlen(buffer)-1] = '\0';

    fprintf(stderr, "Opening input file... ");
    FILE* inputFile = fopen(buffer, "r");
    if(inputFile == NULL){
        fprintf(stderr, "ERROR\n");
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    fprintf(stderr, "DONE\n");

    printf("Name of the output file: ");
    fflush(stdout);
    fgets(buffer, BUFFER_SZ, stdin);
    buffer[strlen(buffer)-1] = '\0';

    fprintf(stderr, "Opening output file... ");
    FILE* outputFile = fopen(buffer, "w+");
    if(outputFile == NULL){
        fprintf(stderr, "ERROR\n");
        perror("fopen");
        fclose(inputFile);
        exit(EXIT_FAILURE);
    }

    fprintf(stderr, "DONE\n");

    bool operationResult = huffmanEncode(inputFile, outputFile);

    if(operationResult){
        printf("Huffman enconding complete.\n");
    }else{
        printf("It seems an error has occurred or something. Sorry :/\n");
    }

    fclose(outputFile);
    fclose(inputFile);

    return 0;
}
