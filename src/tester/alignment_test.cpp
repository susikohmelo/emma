
/* [ TESTS OF ALIGNMENT ]
 *   
 *   This is a very simple test that tests if an object is aligned or not.
 *   Very short one.
 *
 *   This file is included directly in the main tester file.
*/

#include <stdint.h>

void alignment_tests()
{
	emma::allocators::FreeList	EMMA(g_emmas_memory, MEMSIZE);

	LargeClass* l1 = EMMA.allocate_class<LargeClass>(42);
	LargeClass* l2 = EMMA.allocate_class<LargeClass>(42);
	LargeClass* l3 = EMMA.allocate_class<LargeClass>(42);

	assert(l1 != NULL);
	assert(l2 != NULL);
	assert(l3 != NULL);

	std::size_t align1 = sizeof(LargeClass) + (reinterpret_cast<uintptr_t>(l1) % sizeof(LargeClass));
	std::size_t align2 = sizeof(LargeClass) + (reinterpret_cast<uintptr_t>(l2) % sizeof(LargeClass));
	std::size_t align3 = sizeof(LargeClass) + (reinterpret_cast<uintptr_t>(l3) % sizeof(LargeClass));

	std::cout << "Allocated 3 classes with a size of: " << sizeof(LargeClass) <<  "\n\n";
	std::cout << "Class 1 location is   : " << l1 << std::endl;
	std::cout << "Class 1's alignment is: " << align1 << "\n" << std::endl;
	assert(align1 == sizeof(LargeClass));

	std::cout << "Class 2 location is   : " << l2 << std::endl;
	std::cout << "Class 2's alignment is: " << align2 << "\n" << std::endl;
	assert(align2 == sizeof(LargeClass));

	std::cout << "Class 3 location is   : " << l3 << std::endl;
	std::cout << "Class 3's alignment is: " << align3 << "\n" << std::endl;
	assert(align3 == sizeof(LargeClass));

	std::cout << FG_BLACK << BG_GREEN << " SUCCESS " << C_END
	<< C_GREEN << " - all classes were in alignment \n" << C_END << std::endl;
}
