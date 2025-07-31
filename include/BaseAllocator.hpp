
/* [ BASE ALLOCATOR CLASS HEADER ]
 *
 *   This is an abstract class for the allocator.
 *   Allocators belonging to a specific algorithm will inherit this class.
 *
 *   They will override the functions that allocate/free raw pointers.
 *   free/allocate_class() works as an interface/wrapper for said functions.
 */

#ifndef BASEALLOCATOR_HPP
# define BASEALLOCATOR_HPP

# include <EMMA.hpp>
# include <new>

namespace emma
{
	class BaseAllocator
	{
		public:
			BaseAllocator(void* memory_location, std::size_t memory_maxsize) :
			m_memory_location(memory_location), m_memory_maxsize(memory_maxsize) {}

			virtual ~BaseAllocator() {}

			template <class T, typename... Args>
			T* allocate_class(Args... A)
			{
				void* ptr = allocate_raw_ptr(sizeof(T));

				if (ptr != NULL)
					new(ptr) T(A...);

				return ( static_cast<T*>(ptr) );
			}

			template <class T>
			void	free_class(T* ptr_to_class)
			{
				if (ptr_to_class == NULL)
					return;
				std::destroy_at(ptr_to_class);
				free_raw_ptr(static_cast<void*>(ptr_to_class));
			}

			// These are responsible for actually managing the memory
			// The user may also access them directly to allocate raw pointers
			virtual void*	allocate_raw_ptr(std::size_t data_size) = 0;
			virtual void	free_raw_ptr(void *data) = 0;

		protected:
			void*		m_memory_location;
			std::size_t	m_memory_maxsize;
	};
};

#endif
