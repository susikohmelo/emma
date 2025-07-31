
/* [ BENCHMARKING ]
 *
 *   Relatively self explanatory.
 *   Runs the tests many times for more accurate results.
*/

// Returns time PER ALLOCATION taken to allocate
// Simulates a worst case scenario
// Stores pointers to be freed later in a given array
// Failed allocations are only checked for after the benchmark, for a more true time.
// N HAS TO BE a multiple of 5
// Most classes aren't freed but it's fine we just overwrite them anyway
static std::chrono::duration<double> allocate_N_classes(int N)
{
	emma::allocators::FreeList	EMMA(g_emmas_memory, MEMSIZE);
	static	SmallClass* ptr_array[1000];

	std::chrono::duration<double> total_time = std::chrono::duration<double>::zero();

	// Start timer
	for (int i = 0; i < N; i += 5)
	{
		auto begin = std::chrono::high_resolution_clock::now();
		ptr_array[i + 0] = EMMA.allocate_class<SmallClass>(42);
		ptr_array[i + 1] = EMMA.allocate_class<SmallClass>(42);
		ptr_array[i + 2] = EMMA.allocate_class<SmallClass>(42);
		ptr_array[i + 3] = EMMA.allocate_class<SmallClass>(42);
		ptr_array[i + 4] = EMMA.allocate_class<SmallClass>(42);
		auto end = std::chrono::high_resolution_clock::now();
		total_time += std::chrono::duration_cast<std::chrono::duration<double>>(end - begin);

		// Deallocating 2 of the 5 we just allocated,
		// then reallocating 2. Does not count towards time.
		EMMA.free_class<SmallClass>(ptr_array[i + 2]);
		EMMA.free_class<SmallClass>(ptr_array[i + 4]);
		ptr_array[i + 2] = EMMA.allocate_class<SmallClass>(42);
		ptr_array[i + 4] = EMMA.allocate_class<SmallClass>(42);
	}

	// Verifying all allocations were succesful
	for (int i = 0; i < N; ++i)
		{ assert(ptr_array[i] != NULL); }

	return (total_time / static_cast<double>(N / 2)); // Div by 2 makes it 10
}

// Returns time PER ALLOCATION taken to allocate
// Simulates a worst case scenario
// Stores pointers to be freed later in a given array
// N HAS TO BE a multiple of 5
static std::chrono::duration<double> deallocate_N_classes(int N)
{
	emma::allocators::FreeList	EMMA(g_emmas_memory, MEMSIZE);
	static	SmallClass* ptr_array[1000];

	std::chrono::duration<double> total_time = std::chrono::duration<double>::zero();

	// First we need to allocate all our stuff. We again do it in the worst case scenario way
	for (int i = 0; i < N; i += 5)
	{
		ptr_array[i + 0] = EMMA.allocate_class<SmallClass>(42);
		ptr_array[i + 1] = EMMA.allocate_class<SmallClass>(42);
		ptr_array[i + 2] = EMMA.allocate_class<SmallClass>(42);
		ptr_array[i + 3] = EMMA.allocate_class<SmallClass>(42);
		ptr_array[i + 4] = EMMA.allocate_class<SmallClass>(42);

		// Deallocating 2 of the 5 we just allocated, then reallocating 2
		EMMA.free_class<SmallClass>(ptr_array[i + 3]);
		EMMA.free_class<SmallClass>(ptr_array[i + 4]);
		ptr_array[i + 3] = EMMA.allocate_class<SmallClass>(42);
		ptr_array[i + 4] = EMMA.allocate_class<SmallClass>(42);
	}
	for (int i = 0; i < N; ++i)
		{ assert(ptr_array[i] != NULL); }

	// Now we can deallocate them all
	for (int i = 0; i < N; i += 5)
	{
		auto begin = std::chrono::high_resolution_clock::now();
		EMMA.free_class<SmallClass>(ptr_array[i + 0]);
		EMMA.free_class<SmallClass>(ptr_array[i + 1]);
		EMMA.free_class<SmallClass>(ptr_array[i + 2]);
		EMMA.free_class<SmallClass>(ptr_array[i + 3]);
		EMMA.free_class<SmallClass>(ptr_array[i + 4]);
		auto end = std::chrono::high_resolution_clock::now();
		total_time += std::chrono::duration_cast<std::chrono::duration<double>>(end - begin);
	}
	return (total_time / static_cast<double>(N / 2)); // Div by 2 makes it 10
}

static void print_time(std::chrono::duration<double> &time)
{
	std::cout << std::chrono::duration_cast<std::chrono::nanoseconds>(time).count() << " nanoseconds" << std::endl;
}

// To deallocate is how from how many allocations we want to use
static void run_one_deallocation_test(int to_deallocate, int iterations)
{
	std::chrono::duration<double> total_time = std::chrono::duration<double>::zero();
	for (int i = 0; i < iterations; ++i)
	{
		total_time += deallocate_N_classes(to_deallocate);
	}
	total_time /= static_cast<double>(iterations);
	print_time(total_time);
}

// toskip is the amount we want to skip, to run is the time we actually count.
// For example, allocations N=10 to N=100 would have skip=10, run=90
static void run_one_allocation_test(int to_skip, int to_run, int iterations)
{
	std::chrono::duration<double> total_time = std::chrono::duration<double>::zero();
	for (int i = 0; i < iterations; ++i)
	{
		if (to_skip != 0)
			allocate_N_classes(to_skip);
		total_time += allocate_N_classes(to_run);
	}
	total_time /= static_cast<double>(iterations);
	print_time(total_time);
}

void benchmark_tests()
{

	std::cout << FG_YELLOW << " - Realistic Allocations - " << C_END << std::endl;
	std::cout << "N represents the amount of existing allocations\n" << std::endl;

	std::cout << " - Time per 10 allocations between N=0 to N=10 -" << std::endl;
	run_one_allocation_test(0, 10, 1000000);

	std::cout << " - Time per 10 allocations between N=10 to N=100 -" << std::endl;
	run_one_allocation_test(10, 90, 200000);

	std::cout << " - Time per 10 allocations between N=100 to N=200 -" << std::endl;
	run_one_allocation_test(100, 100, 150000);

	std::cout << " - Time per 10 allocations between N=200 to N=300 -" << std::endl;
	run_one_allocation_test(200, 100, 100000);

	std::cout << " - Time per 10 allocations between N=300 to N=400 -" << std::endl;
	run_one_allocation_test(300, 100, 50000);

	std::cout << " - Time per 10 allocations between N=400 to N=500 -" << std::endl;
	run_one_allocation_test(300, 100, 30000);


	std::cout << FG_YELLOW << "\n - Realistic Deallocations - " << C_END << std::endl;
	std::cout << "N represents the amount of existing allocations\n" << std::endl;

	std::cout << " - Time per 10 deallocations from N=10 to N=0 -" << std::endl;
	run_one_deallocation_test(10, 1000000);

	std::cout << " - Time per 10 deallocations from N=100 to N=0 -" << std::endl;
	run_one_deallocation_test(100, 150000);

	std::cout << " - Time per 10 deallocations from N=200 to N=0 -" << std::endl;
	run_one_deallocation_test(200, 150000);

	std::cout << " - Time per 10 deallocations from N=300 to N=0 -" << std::endl;
	run_one_deallocation_test(300, 100000);

	std::cout << " - Time per 10 deallocations from N=400 to N=0 -" << std::endl;
	run_one_deallocation_test(400, 50000);

	std::cout << " - Time per 10 deallocations from N=500 to N=0 -" << std::endl;
	run_one_deallocation_test(500, 50000);

}
