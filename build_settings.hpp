
/* [ PURPOSE OF FILE ]
 *
 *   This file is a collection of preprocessors,
 *   they are used to customize the behaviour of the allocator.
 *  
 *   These options may also be overwritten with -D during compilation */

#ifndef BUILD_SETTINGS
# define BUILD_SETTINGS


/* [ ENABLE_EXCEPTIONS ]
 *   Enable exception throwing.
 *
 *   If enabled, the allocators will throw an exception with an error message
 *   describing the specific cause of error.
 *
 *   If disabled, the allocators will return NULL and do nothing else.
 *
 *   This is disabled by default as the performance overhead of C++ exceptions
 *   is significant, especially on lower end hardware.
 *
 *   This can also be used a type of assert.
 *   Enable this without adding a try/catch to your allocation, and the program
 *   will hault if the allocator comes across any problem. Useful for debugging.
 *
 *   0 = OFF, 1 = ON. */
# ifndef EMMA_ENABLE_EXCEPTIONS
#  define EMMA_ENABLE_EXCEPTIONS 0
# endif


#endif
