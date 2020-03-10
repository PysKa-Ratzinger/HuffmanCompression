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
	 * Default constructor
	 */
	FileAnalysis();

	FileAnalysis( std::array< size_t, TOTAL_CHARS > predeterminedFreqs );

	void FeedText( const char* msg, size_t msgSize );

	/**
	 * Array with the frequency of every byte. The index of each frequency
	 * corresponds to the byte analysed, and the value to the frequency of
	 * said byte.
	 */
	std::array< size_t, TOTAL_CHARS > frequency { 0 };

	void Print() const;

	/**
	 * The total number of bytes in the file
	 */
	uint64_t total = 0;
};

