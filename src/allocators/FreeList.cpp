
/* [ FREE LIST ALLOCATOR CLASS FILE ]
 *
 * This is derived from the base allocator class.
 *
 * This free list is red-black binary search tree, which
 * guarantees that the time complexity for all operations is at most O(log n).  │
 *                                                                              │
 *  - Simplified example of memory layout -                                     │
 * ┌─────────┬─────────┐┌─────────┬──────────────────────┐┌─────────┬─────────┐ │
 * │Next|Prev│Allocated││Next|Prev│ RBnode & free memory ││Next|Prev│Allocated│ │
 * └─────────┴─────────┘└─────────┴──────────────────────┘└─────────┴─────────┘ │
 *                                                                              │
 * Each block of memory always contains a header with a double linked list.     │
 * The list connects nearby blocks together, which helps us achieve a time      │
 * complexity of O(1) for coalescence (merging of nearby free blocks).          │
 *                                                                              │
 * For each free block, there is a red-black tree node following the header.    │
 * These nodes are used only for navigating the free blocks of memory.          │
 *                                                                              │
 * The nodes do not use up any potential allocatable space, because they are    │
 * stored inside the free memory they represent & are destroyed on allocation.  │
 *                                                                              │
 * Not shown in illustration is any required padding needed for alignment.      │
 * The allocated memory, header and nodes are all naturally aligned.            │
 *
 */

#include <EMMA.hpp>
#include <FreeList.hpp>
#include <stdint.h>
#include <memory>
#include <limits>
#include <new>

static inline bool start_or_size_is_invalid(void* start, std::size_t size)
{/* Returns true if one of the values is invalid and throws exception if enabled.
    Returns false if both start and size are valid */

	static std::string too_small_error_msg =\
    	"Memsize can't be under " + std::to_string(emma::allocators::FreeList::MIN_INIT_SIZE);

	if (size < emma::allocators::FreeList::MIN_INIT_SIZE)
		return emma::return_error<bool>(true, too_small_error_msg);

	if (start == NULL)
		return emma::return_error<bool>(true, "Starting address can't be NULL");

	return false;
}

emma::allocators::FreeList::FreeList(void* start, std::size_t size) : emma::BaseAllocator(start, size)
{/* Params    : (1) Ptr to the start of the memory available for the allocator
 *              (2) Size of the memory available for the allocator
 *  On Success: Initializes the allocator, which will be ready for immediate use.
 *  On failure: Throws an exception if they're enabled.
 *              Otherwise does nothing & attempted allocations return NULL.
 *  Fails if  : There is not enough memory to add a single alligned header/node */

	if (start_or_size_is_invalid(start, size))
		return; // Also throws an exception if they're enbaled

	// Set this to point to the end of the memory we have available
	this->m_end_of_memory = reinterpret_cast<Header*>(static_cast<uint8_t*>(start) + size);
	
	// Create our first free block of memory, using our entire free memory
	create_new_memory_block(NULL, this->m_end_of_memory, start);
}

emma::allocators::FreeList::~FreeList() {}


static inline bool data_size_is_invalid(std::size_t data_size)
{/* Simple error checking helper for allocate_raw_ptr()
    Returns true if data_size is invalid. Throws exception if they are enabled
    Returns false if data_size is valid */

	if (data_size == 0)
		return emma::return_error<bool>(true, "Allocation size can't be 0!");

	if (std::numeric_limits<std::size_t>::max() / data_size < 2)
		return emma::return_error<bool>(true, "Allocation size would overflow!");

	return false;
}

void* emma::allocators::FreeList::allocate_raw_ptr(std::size_t data_size)
{/* Params    : Size of the allocation we want to make
 *  On success: Returns an aligned pointer to the newly allocated data
 *  On failure: Returns NULL. Throws exception if they are enabled.
 *  Fails if  : There is not enough memory,
 *	        or data_size == 0, or data_size would overflow with padding*/

	if (data_size_is_invalid(data_size))
		return NULL;

	// Smallest size that guarantees natural alignment. Extra is trimmed later
	std::size_t	search_size = HEADER_MAX_PADDING + data_size * 2;
	search_size = search_size < MIN_INIT_SIZE ? MIN_INIT_SIZE : search_size;

	// Find best fitting free node
	emma::RedBlackTree::Node* free_node = this->m_rb_tree.search_best_fit(search_size);
	if (free_node == NULL)
		return emma::return_error<void*>(NULL, "No free nodes were found");

	Header*   header = get_header_placement_from_ptr(free_node);
	
	void* aligned_data_ptr = reinterpret_cast<void*>(reinterpret_cast<uint8_t*>(header) + sizeof(Header));
	std::size_t space_left = free_node->value;

	// This aligns the aligned_data_ptr. Also updates the remaining space.
	align_to_natural(data_size, aligned_data_ptr, space_left);

	// Move header forward to the closest aligned position behind the data
	Header* next = header->next;
	Header* prev = header->prev;
	std::destroy_at(header);
	header = get_header_placement_from_ptr(aligned_data_ptr);
	new(header) Header(next, prev);
	if (prev != NULL) // Update the previous node to point to us!!
		prev->next = header;
	if (next != this->m_end_of_memory) // Update the next node to point to us!!
		next->prev = header;

	// Make sure we have enough space to create a node when we deallocate it
	std::size_t space_taken = free_node->value - space_left;
	data_size = (space_taken + data_size) < MIN_INIT_SIZE ? MIN_INIT_SIZE - space_taken : data_size;
	space_left -= data_size;

	// Remove the RB free node
	this->m_rb_tree.remove_node(free_node);
	std::destroy_at(free_node);

	split_extra_memory_into_new_block(\
		space_left, header, static_cast<uint8_t*>(aligned_data_ptr) + data_size);

	return aligned_data_ptr;
}

void emma::allocators::FreeList::free_raw_ptr(void *data)
{/* Params    : (1) Data which has been previously allocated
 *  On success: Deallocates the requested data
 *  On failure: Does nothing
 *  Fails if  : Data is NULL. Otherwise cannot fail (assuming ptr is valid) */

	Header*	our_header   = get_header_placement_from_ptr(data);
	Header*	left_header  = our_header->prev;
	Header*	right_header = our_header->next;
	Header*	new_next     = our_header->next;

	// If the block on our right is free, destroy it and extend our own memory
	if (right_header != this->m_end_of_memory && right_header->node != NULL)
	{
		new_next = right_header->next;
		our_header->next = right_header->next;
		if (right_header->next != this->m_end_of_memory)
			right_header->next->prev = our_header;

		this->m_rb_tree.remove_node(right_header->node);
		std::destroy_at(right_header->node);
		std::destroy_at(right_header);
		right_header = NULL;
	}
	// If the block on our left is free, destroy ourselves and extend left block
	if (left_header != NULL && left_header->node != NULL)
	{
		// Update our right block to point to our left block
		if (right_header != NULL && right_header != this->m_end_of_memory)
			right_header->prev = left_header;

		left_header->next = our_header->next;

		this->m_rb_tree.remove_node(our_header->node);
		std::destroy_at(our_header->node);
		std::destroy_at(our_header);

		std::size_t new_memory_size = reinterpret_cast<std::size_t>(\
			reinterpret_cast<uintptr_t>(left_header->next) - reinterpret_cast<uintptr_t>(left_header));

		// Update size of the left block's node
		this->m_rb_tree.remove_node(left_header->node);
		left_header->node->value = new_memory_size;
		this->m_rb_tree.insert_node(left_header->node);
	}
	else // Left block isn't free or is NULL
	{
		std::destroy_at(our_header);
		if (left_header == NULL) // We are the root block. We can reset padding.
			our_header = static_cast<Header*>(m_memory_location);

		create_new_memory_block(left_header, new_next, static_cast<void*>(our_header));
	}
}


void emma::allocators::FreeList::split_extra_memory_into_new_block(\
std::size_t space_left, Header* prev_header, void* extra_memory)
{/* Params    : (1) Free space left over from an allocation
 *              (2) Ptr to header of what we just allocated
 *              (3) Ptr to the unused extra memory
 *  On success: Creates new header and RB-Node inside the leftover space.
 *  On failure: Does nothing
 *  Fails if  : There is not enough memory to add aligned headers/nodes */

	if (space_left < (HEADER_MAX_PADDING + NODE_MAX_PADDING))
			return ; // Not enough space for a new block
	
	std::size_t original_space_left = space_left;

	// Figure out where to put the header in an aligned way
	void*	aligned_header = extra_memory;
	align_to_natural(sizeof(Header), aligned_header, space_left);

	// Same for the node
	void*	aligned_node = static_cast<uint8_t*>(aligned_header) + sizeof(Header);
	align_to_natural(sizeof(emma::RedBlackTree::Node), aligned_node, space_left);

	// Move header to closest aligned position behind the node
	aligned_header = get_header_placement_from_ptr(aligned_node);
	std::size_t	memory_left_minus_header = reinterpret_cast<std::size_t>(original_space_left \
		- (reinterpret_cast<uintptr_t>(aligned_header) - reinterpret_cast<uintptr_t>(extra_memory))
		- sizeof(Header));

	// Construct new node & store the size available (minus the header)
	new(aligned_node) emma::RedBlackTree::Node(memory_left_minus_header);
	this->m_rb_tree.insert_node(static_cast<emma::RedBlackTree::Node*>(aligned_node));

	// Construct new header & update the linked list
	new(aligned_header) Header(prev_header->next, prev_header);
	prev_header->next = static_cast<Header*>(aligned_header);
	static_cast<Header*>(aligned_header)->node = static_cast<emma::RedBlackTree::Node*>(aligned_node);	
 }


void emma::allocators::FreeList::create_new_memory_block(\
Header* prev_header, Header* next_header, void* deallocated_ptr)
{/* Params    : (1) Ptr to a header on our left (or NULL if it doesn't exist)
 *              (2) Ptr to a header on our right (or end of memory if it doesn't exist)
 *              (3) Ptr to the unused memory
 *  On success: Creates new header and RB-Node inside the space.
 *  On failure: Does nothing
 *  Fails if  : There is not enough memory, or next_header is NULL*/

	if (next_header == NULL)
		return;

	std::size_t space_left = static_cast<std::size_t>(\
	reinterpret_cast<uintptr_t>(next_header) - reinterpret_cast<uintptr_t>(deallocated_ptr));

	std::size_t original_space_left = space_left;

	if (space_left < (HEADER_MAX_PADDING + NODE_MAX_PADDING))
		return ; // Not enough space for a new block

	// Figure out where to put the header in an aligned way
	void*	aligned_header = deallocated_ptr;
	align_to_natural(sizeof(Header), aligned_header, space_left);

	// Same for the node
	void*	aligned_node = static_cast<uint8_t*>(aligned_header) + sizeof(Header);
	align_to_natural(sizeof(emma::RedBlackTree::Node), aligned_node, space_left);

	// Move header to closest aligned position behind the node
	// Space available - padding - sizeof header
	aligned_header = get_header_placement_from_ptr(aligned_node);
	std::size_t memory_left_minus_header = reinterpret_cast<std::size_t>(\
	            original_space_left
	            - (reinterpret_cast<uintptr_t>(aligned_header)
	            - reinterpret_cast<uintptr_t>(deallocated_ptr))
	            - sizeof(Header));

	// Construct new node & store the size available (minus the header)
	new(aligned_node) emma::RedBlackTree::Node(memory_left_minus_header);
	this->m_rb_tree.insert_node(static_cast<emma::RedBlackTree::Node*>(aligned_node));

	// Construct new header & update the linked list
	new(aligned_header) Header(next_header, prev_header);
	if (prev_header != NULL)
		prev_header->next = static_cast<Header*>(aligned_header);
	if (next_header != NULL && next_header != this->m_end_of_memory)
		next_header->prev = static_cast<Header*>(aligned_header);
	static_cast<Header*>(aligned_header)->node = static_cast<emma::RedBlackTree::Node*>(aligned_node);
 }


emma::allocators::FreeList::Header* \
emma::allocators::FreeList::get_header_placement_from_ptr(void *ptr)
{/* Params    : (1) Ptr to data/node that is located after the header
 *  On success: Returns location of header, based on the location of the ptr
 *  On failure: Returns NULL
 *  Fails if  : Input ptr is NULL */

	if (ptr == NULL)
		return NULL;

	// We get the earliest point where the header could be (right behind us).
	// Then we just move backwards to the first point that works for alignment.
	uint8_t*	earliest_unaligned_header = static_cast<uint8_t*>(ptr) - sizeof(Header);
	std::size_t	align_offset = static_cast<std::size_t>(\
				reinterpret_cast<uintptr_t>(earliest_unaligned_header) % sizeof(Header));
	return (reinterpret_cast<Header*>(earliest_unaligned_header - align_offset));
}


void emma::allocators::FreeList::align_to_natural(std::size_t data_size, void *&ptr, std::size_t &space_left)
{/* Params    : (1) Ptr to data/node that is located after the header
 *              (2) Ptr to to our memory
 *              (3) Space available in memory
 *  On success: Updates ptr to point to an aligned position, decrements data_size by the offset
 *  Fails if  : Cannot fail if unless space left is too small. There is no check for this!*/

	// We move back to the first aligned position
	uint8_t*	u_ptr = static_cast<uint8_t*>(ptr);
	std::size_t	align_offset = static_cast<std::size_t>(reinterpret_cast<uintptr_t>(u_ptr) % data_size);
	u_ptr -= align_offset;

	// Then we move forward by 1 unit of natural alignment. Simple!
	u_ptr += data_size;
	ptr = reinterpret_cast<void*>(u_ptr);

	space_left -= (data_size - align_offset);
}
