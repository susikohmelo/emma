
/* [ TESTS OF DETERMINISM ]
 *
 *   This tests that EMMA is deterministic.
 *
 *   The location of allocated memory, failures etc. should all be
 *   fully deterministic.
 *
 *   In other words, if doing a call 'A' from state 'B' makes emma respond with 'C'.
 *   EVERY time you make call 'B' from state 'A', emma must return 'C'.
 *
 *   This file is included directly in the main tester file.
*/

// Returns the iterations it took until failure
static std::size_t	EMMA_allocate_until_failure(\
emma::allocators::FreeList &EMMA, std::vector<SmallClass*> &ptrs_list)
{
	SmallClass*	allocated_ptr;
	std::size_t	iterations = 0;

	while (1)
	{
		allocated_ptr = EMMA.allocate_class<SmallClass>(42);
		if (allocated_ptr == NULL)
			break;
		
		// Verifying class allocated succesfully
		assert(allocated_ptr->getNumber() == 42);
		ptrs_list.push_back(allocated_ptr);
		++iterations;
	}
	return iterations;
}

// Runs one test
// The test involves allocating classes until we run out of memory.
// Then we deallocate one class, and allocate it. It should succeed.
// Return the amount of iterations it took to run out of memory.
// This is compared against the same test again later
static std::size_t run_one_allocation_test(emma::allocators::FreeList &EMMA)
{
	std::cout << "1. Allocating with EMMA until it runs out of memory" << std::endl;

	std::vector<SmallClass*>	ptrs_list;

	std::size_t iterations =	EMMA_allocate_until_failure(EMMA, ptrs_list);
	std::cout << "-  Out of memory after allocation no. " << iterations << std::endl;

	std::cout << "2. Attempting the same allocation again" << std::endl;
	SmallClass* new_allocation = EMMA.allocate_class<SmallClass>(42);
	assert(new_allocation == NULL);
	std::cout << "-  Allocation failed (as it should)" << std::endl;


	std::cout << "3. Deallocating one class and attempting one new allocation" << std::endl;
	EMMA.free_class(*ptrs_list.begin());
	ptrs_list.erase(ptrs_list.begin());

	new_allocation = EMMA.allocate_class<SmallClass>(42);
	assert(new_allocation != NULL); // Verifying class allocated succesfully
	assert(new_allocation->getNumber() == 42); 
	std::cout << "-  Allocation succesful" << std::endl;
	EMMA.free_class(new_allocation);

	std::cout << "4. Dellacoting and reallocating one more time" << std::endl;
	new_allocation = EMMA.allocate_class<SmallClass>(42);
	assert(new_allocation != NULL); // Verifying class allocated succesfully
	assert(new_allocation->getNumber() == 42); 
	std::cout << "-  Allocation succesful" << std::endl;
	EMMA.free_class(new_allocation);

	std::cout << "5. Deallocating all classes" << std::endl;
	for (SmallClass* ptr : ptrs_list)
		EMMA.free_class(ptr);

	return iterations;
}

void determinism_tests()
{
	emma::allocators::FreeList	EMMA(g_emmas_memory, MEMSIZE);
	std::size_t	iterations_1;
	std::size_t	iterations_2;

	std::cout << "Sequence (1)" << std::endl;
	iterations_1 = run_one_allocation_test(EMMA);
	std::cout << std::endl; //Newline

	std::cout << "- Redoing the same calls again on the same instance of EMMA -\n" << std::endl;
	std::cout << "Sequence (2)" << std::endl;
	iterations_2 = run_one_allocation_test(EMMA);
	std::cout << std::endl; //Newline

	assert(iterations_1 == iterations_2);
	std::cout << FG_BLACK << BG_GREEN << " SUCCESS " << C_END
	<< C_GREEN << " - both tests had the same output.\n"
	"EMMA started from a given state, performed a specific sequence of instructions,\n"
	"the calls were reverted via deallocation, then EMMA reproduced the same sequence again."
	<< C_END << std::endl;
}
