
/* [ EMMA'S MAIN INCLUDE FILE ]
 *
 *   This file includes all of the relevant headers together, so that projects
 *   utilizing EMMA only need to include this one header.
 *
 *   This header is also used internally. */

#ifndef EMMA_HPP
# define EMMA_HPP

# include <string>
# include <cstddef>
# include <exception>
# include <stdbool.h>
# include "../build_settings.hpp"
# include "BaseAllocator.hpp"
# include "RedBlackTree.hpp"
# include "FreeList.hpp"

namespace emma
{
	// Just an std::exception with a constructor for the what() message
	# if EMMA_ENABLE_EXCEPTIONS
	class ExceptionWithMessage: public std::exception
	{
		public:
			ExceptionWithMessage(const std::string& msg) : m_message(msg) {}

			virtual const char *what() const noexcept override
			{
				return (m_message.c_str());
			}
		private:
			std::string m_message;
	};
	# endif

	/* Throws an exception if they're enabled,
	 * otherwise returns an object of a requested type - usually NULL.
	 *
	 * This function is very useful as there are a lot of cases where we want
	 * to throw an exception if they are enabled, but return if they aren't.
	 * This allows one liners like: return error_return<void*>(NULL, "Oh-no!") */
	template <class T>
	T return_error(T return_value, const std::string& message)
	{
		# if EMMA_ENABLE_EXCEPTIONS
			throw emma::ExceptionWithMessage(message);
		# else
			(void) message;
		# endif
		return (return_value);
	}
};


#endif
