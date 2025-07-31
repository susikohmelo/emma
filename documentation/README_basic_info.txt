Contributors: Lassi Jylhänkangas aka. Susikohmelo                                 
Tools used  : VIM & coffee
                                                                                  
┌──────────────── EMMA - Embedded Modular Memory Allocator ────────────────────┐
│     A portable, modular memory allocator for C++17 on embedded systems.      │
└──────────────────────────────────────────────────────────────────────────────┘

┌ [ Summary ] ─────────────────────────────────────────────────────────────────┐
│                                                                              │
│ The primary purpose of EMMA is to provide functionality similar to standard  │
│ C++ 'new' and 'delete' operators, managing memory and class construction.    │
│                                                                              │
│ In addition, EMMA can allocate/deallocate raw pointers, similar to malloc(). │
│                                                                              │
│ EMMA is polymorphic, and thus supports multiple memory management algorithms.│
│ A simple framework is provided implement any number of them.                 │
│                                                                              │
│ The user is able to define the location and size of the memory available to  │
│ the allocator, as well as the specific algorithm used to manage the memory.  │
│ It is also possible to use multiple EMMA's at the same time, which may be    │
│ useful for handling a specific data type with higher efficiency.             │
│                                                                              │
│ By default, EMMA is portable and doesn't use system specific functions.      │
│ Without changes (other than the compiler), it should run on most platforms.  │
│                                                                              │
│ Some features such as exception throwing are disabled by default,            │
│ but can be enabled through a configuration file before building.             │
│ See 'build_settings.hpp' and 'functions_and_classes_list.txt' for more info. │
│                                                                              │
│ And thank you RTFM!                                                          │
│                                                                              │
└──────────────────────────────────────────────────────────────────────────────┘
                                                                                  
┌ [ Tests & Benchmarks ] ──────────────────────────────────────────────────────┐
│                                                                              │
│ Some tests and benchmarks are provided with the allocator.                   │
│                                                                              │
│ To execute them, run 'make test' at the root folder.                         │
│                                                                              │
│ It will compile the library, then the tests, and then launch the tests.      │
│ 'tester_program' will be the name of the executable.                         │
│                                                                              │
│ Please note that the tests are not intended to be run on embedded systems!   │
│ They are intended to performed on a x86_64 Linux opearting system.           │
│                                                                              │
└──────────────────────────────────────────────────────────────────────────────┘
                                                                                  
┌ [ How to include in your project ] ──────────────────────────────────────────┐
│                                                                              │
│ Instructions are for x86_64 Ubuntu LTS 24.04 - but should work across Linux. │
│                                                                              │
│ 0. If needed, swap the compiler in the Makefile to your C++ cross compiler   │
│ 1. Build the library by running 'make' at the root of the repository         │
│ 2. Link 'build/libEMMA.a' with your project                                  │
│ 3. Include the 'include/EMMA.hpp' header in your file(s)                     │
│                                                                              │
└──────────────────────────────────────────────────────────────────────────────┘

┌ [ Basic Usage ] ─────────────────────────────────────────────────────────────┐
│                                                                              │
│ First, construct the allocator. It's arguments are:                          │
│   1. A ptr to the starting address of the memory available to the allocator. │
│   2. The amount of memory (in bytes) available starting from that address.   │
│                                                                              │
│ emma::allocators contains all of the different memory management algorithms. │
│                                                                              │
│ Example 1: emma::allocators::FreeList  my_EMMA(0x80000, 4096);               │
│ Example 2: emma::allocators::FreeList  my_EMMA(ptr_from_system, 1000000);    │
│                                                                              │
│ Emma is now ready to use! Here's all the things it can do for you:           │
│                                                                              │
│┌ 1. Allocate & construct class with variable arguments ─────────────────────┐│
││                                                                            ││
││ Prototpe:   T*  allocate_class<T>(Arguments...);                           ││
││                                                                            ││
││ On succes:                                                                 ││
││    1. Allocate the required space                                          ││
││    2. Construct the class with the given arguments                         ││
││    3. Return a pointer to the class                                        ││
││                                                                            ││
││ On failure, NULL is returned and an exception thrown if they're enabled.   ││
││ Most common fail mode by far is the allocator running out of memory.       ││
││                                                                            ││
││ Example:                                                                   ││
││    MyClass*  ptr = my_EMMA.allocate_class<MyClass>("Cool beans c:", 404);  ││
││                                                                            ││
│└────────────────────────────────────────────────────────────────────────────┘│
│                                                                              │
│┌ 2. Deallocate & deconstruct classes  ──────────────────────────────────────┐│
││                                                                            ││
││ Prototpe:   void  free_class<T>(T* ptr_to_class);                          ││
││                                                                            ││
││ On success:                                                                ││
││    2. Construct the class                                                  ││
││    1. Deallocate the space it held                                         ││
││                                                                            ││
││ Deallocations cannot fail, assuming that the pointer is valid.             ││
││ Meaning it points to a previously allocated class, yet to be deallocated.  ││
││                                                                            ││
││ Example:                                                                   ││
││    my_EMMA.free_class<MyClass>(my_class_ptr);                              ││
││                                                                            ││
│└────────────────────────────────────────────────────────────────────────────┘│
│                                                                              │
│┌ 3. Allocate/deallocate raw pointers  ──────────────────────────────────────┐│
││                                                                            ││
││ Prototpe:   void*  allocate_raw_ptr(std::size_t data_size);                ││
││             void   free_raw_ptr(void *ptr_to_data);                        ││
││                                                                            ││
││ The behaviour is exactly the same as de/allocating classes, just that the  ││
││ returned value is a raw block of memory with no class inside.              ││
││                                                                            ││
││ Works essentially like C's malloc() and free();                            ││
││ 'data_size' represents the number of bytes requested from EMMA.            ││
││                                                                            ││
││ Example:                                                                   ││
││    void*  array_of_cat_names = my_EMMA.allocate_raw_ptr(100);              ││
││    my_EMMA.free_raw_ptr(array_of_cat_names);                               ││
││                                                                            ││
│└────────────────────────────────────────────────────────────────────────────┘│
└──────────────────────────────────────────────────────────────────────────────┘
                                                                                  
┌ [ Default Algorithms ] ──────────────────────────────────────────────────────┐
│                                                                              │
│ One algorithm is provided by default.                                        │
│                                                                              │
│ Free List.                                                                   │
│ It is optimized using a self balancing red-black binary search tree, which   │
│ guarantees that the time complexity for all operations is at most O(log n).  │
│                                                                              │
│ One of the more sophisticated memory management algorithms, and popular      │
│ among real systems due to it's flexibility and relatively high performance.  │
│                                                                              │
│  - Simplified example of memory layout -                                     │
│ ┌─────────┬─────────┐┌─────────┬──────────────────────┐┌─────────┬─────────┐ │
│ │Next|Prev│Allocated││Next|Prev│ RBnode & free memory ││Next|Prev│Allocated│ │
│ └─────────┴─────────┘└─────────┴──────────────────────┘└─────────┴─────────┘ │
│                                                                              │
│ Each block of memory always contains a header with a double linked list.     │
│ The list connects nearby blocks together, which helps us achieve a time      │
│ complexity of O(1) for coalescence (merging of nearby free blocks).          │
│                                                                              │
│ For each free block, there is a red-black tree node following the header.    │
│ These nodes are used only for navigating the free blocks of memory.          │
│                                                                              │
│ The nodes do not use up any potential allocatable space, because they are    │
│ stored inside the free memory they represent & are destroyed on allocation.  │
│                                                                              │
│ The RB-tree also allows us to search purely with the 'best-fit' approach.    │
│ That is when we look for a free block of memory which is closest to our      │
│ desired allocation size. This is as opposed to a 'first-fit' approach.       │
│                                                                              │
│ Not shown in illustration is any required padding needed for alignment.      │
│ The allocated memory, header and nodes are all naturally aligned.            │
│                                                                              │
└──────────────────────────────────────────────────────────────────────────────┘

┌ [ Known Issues ] ────────────────────────────────────────────────────────────┐
│                                                                              │
│ Something know is that EMMA's deallocation doesn't verify if the pointer you │
│ provide actually points to a valid deallocatable block of memory.            │
│                                                                              │
│ If it is valid, it will always succeed.                                      │
│ But it being invalid, is the only known undefined behaviour of EMMA.         │
│                                                                              │
└──────────────────────────────────────────────────────────────────────────────┘
