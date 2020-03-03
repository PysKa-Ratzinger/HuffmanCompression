#pragma once

#include <stdlib.h>
#include <stdint.h>

#include <array>

#define TOTAL_CHARS 0x100

/*
================================
Used to store all the information regarding the frequency of every byte
found in the analysed file
================================
 */
struct FileAnalysis
{
	/**
	 * Constructor given a file descriptor
	 */
	FileAnalysis( int fd );

	FileAnalysis( std::array< size_t, TOTAL_CHARS > predeterminedFreqs );

	/**
	 * Array with the frequency of every byte. The index of each frequency
	 * corresponds to the byte analysed, and the value to the frequency of
	 * said byte.
	 */
	std::array< size_t, TOTAL_CHARS > frequency { 0 };

	/**
	 * The total number of bytes in the file
	 */
	uint64_t total = 0;
};

