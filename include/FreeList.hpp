
/* [ FREE LIST ALLOCATOR HEADER FILE ]
 *
 *   This is derived from the base allocator class.
 *
 *   Implements a RB-tree optimized Free List memory management algorithm.
 *   More detailed overview of the algorithm can be found inside the README,
 *   as well as the .cpp file */

#ifndef FREELIST_HPP
# define FREELIST_HPP

# include <EMMA.hpp>
# include <RedBlackTree.hpp>
# include <memory>

namespace emma
{
	namespace allocators
	{
		class FreeList : public emma::BaseAllocator
		{
			public:
				FreeList(void* memory_location, std::size_t memory_maxsize);
				~FreeList();

				void*	allocate_raw_ptr(std::size_t data_size) override;
				void	free_raw_ptr(void *data) override;

				class Header
				{
					public:
						Header(Header* nxt, Header* prv) : next(nxt), prev(prv), node(NULL) {}
						~Header() {}

						Header*	next;
						Header*	prev;
						emma::RedBlackTree::Node* node;

						std::size_t	get_size();
				};

				// These are used just like macros, just through a namespace.
				static constexpr std::size_t HEADER_MAX_PADDING = 2 * sizeof(Header);
				static constexpr std::size_t NODE_MAX_PADDING = 2 * sizeof(emma::RedBlackTree::Node);
				static constexpr std::size_t MIN_INIT_SIZE = NODE_MAX_PADDING + HEADER_MAX_PADDING;

			private:
				emma::RedBlackTree m_rb_tree;
				Header*	m_end_of_memory; // Points to end of memory available

				Header*	get_header_placement_from_ptr(void* ptr);
				void	align_to_natural(std::size_t data_size, void *&ptr, std::size_t &space_left);

				void	create_new_memory_block(Header* prev_header,
						Header* next_header, void* dealloc_position);

				void	split_extra_memory_into_new_block(std::size_t space_left,
						Header* current_header, void* ptr_to_extra_memory);
		};
	};
};

#endif
