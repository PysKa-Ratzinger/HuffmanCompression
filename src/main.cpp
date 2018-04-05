#include <stdio.h>
#include <string.h>
#include "huffman_algo.hpp"
#include "bitstream.hpp"

#define BUFFER_SZ 512

#define COMPRESS 1
#define DECOMPRESS 2

int main(){
	char buffer[BUFFER_SZ];

	printf("Options:\n\n");
	printf("\t1. Compress file\n");
	printf("\t2. Decompress file\n");
	printf("\t0. Exit\n");
	fgets(buffer, BUFFER_SZ, stdin);

	int option = 0;

	switch(buffer[0]){
		case '0':
			return 0;
		case '1':
			option = COMPRESS;
			break;
		case '2':
			option = DECOMPRESS;
			return 1;
		default:
			return 1;
	}

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

	bool operationResult;
	switch(option){
		case COMPRESS:
			operationResult = huffmanEncode(inputFile, outputFile);
			break;
		case DECOMPRESS:
			operationResult = huffmanDecode(inputFile, outputFile);
			break;
		default:
			return 1;
	}
	if(operationResult){
		printf("Huffman enconding complete.\n");
	}else{
		printf("It seems an error has occurred or something.\n");
	}

	fclose(outputFile);
	fclose(inputFile);

	return 0;
}

