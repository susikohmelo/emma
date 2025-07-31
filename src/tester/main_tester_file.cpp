
/* [ MAIN TESTING FILE ]
 *
 *   Please note that the testers are intended to be only used on Linux!
 *   
 *   The documentation of the tester is also much less rigerous.
*/

#include <EMMA.hpp>
#include <iostream>
#include <stdlib.h>
#include <stdint.h>
#include <cassert>
#include <chrono>
#include <vector>

#include "color_codes.hpp"

class LargeClass // Used to verify alignment. It's more obvious with bigger sizes
{
	public:
		LargeClass(int i) : m_number(i) {}
		~LargeClass() {}
		int getNumber() { return m_number; }
	private:
		int		m_number; // Used to verify function constructed succesfully
		void*	filler_data[42];
};
class SmallClass
{
	public:
		SmallClass(int i) : m_number(i) {}
		~SmallClass() {}
		int getNumber() { return m_number; }
	private:
		int		m_number; // Used to verify function constructed succesfully
};


#define	MEMSIZE 262144
void*	g_emmas_memory;

// Simplifies our compilation and inclusions
#include "determinism_test.cpp"
#include "alignment_test.cpp"
#include "benchmarks.cpp"

int main()
{
	// Get a piece of memory from the system.
	// In real applications you would of course directly ask for memory from
	// the system and not malloc, or have a specific location in the memory map.
	g_emmas_memory = malloc(MEMSIZE);
	assert(g_emmas_memory != NULL);

	// Throw the title + description in the terminal
	std::cout << FG_BLACK << BG_CYAN << " [ Determinism tests ] " << C_END << std::endl;
	static std::string description_determinism = \
	"This tests how deterministic emma is. Given the same starting state\n"
	"and same set of function calls, emma should always reproduce the same state/output.\n";
	std::cout << C_CYAN << description_determinism << C_END << std::endl;

	determinism_tests();

	// Throw the title + description in the terminal
	std::cout << "\n" << std::endl;
	std::cout << FG_BLACK << BG_CYAN << " [ Alignment tests ] " << C_END << std::endl;
	static std::string description_alignment = \
	"This tests the alignment of the variables. They should all be naturally aligned.\n";
	std::cout << C_CYAN << description_determinism << C_END << std::endl;

	alignment_tests();

	// Throw the title + description in the terminal
	std::cout << "\n" << std::endl;
	std::cout << FG_BLACK << BG_CYAN << " [ Benchmarking test ] " << C_END << std::endl;
	static std::string description_benchmark= \
	"This tests the performance of EMMA('s free list).\n"
	"Most importantly, this helps in verifying the time complexity.\n"
	"\nThese tests deallocate & reallocate classes between measurements to simulate a realistic environment\n"
	"Each section of the test is run tens thousands of times for more accurate results.\n"
	"Please be patient...";
	std::cout << C_CYAN << description_benchmark << C_END << std::endl;

	benchmark_tests();

	free (g_emmas_memory);
}
