#pragma once

#define DMALLOC_DISABLE
#include "dmalloc.h"

#ifdef _MSC_VER
#define WIN32_MANGLE(x) our_##x
#else
#define WIN32_MANGLE(x) x
#endif /* _MSC_VER */

/*
 * DMALLOC_PNT malloc
 *
 * Overloading the malloc(3) function.  Allocate and return a memory
 * block of a certain size.
 *
 * Returns a valid pointer on success or NULL on failure.
 *
 * ARGUMENTS:
 *
 * size -> Number of bytes requested.
 */
extern
RESTRICT
DMALLOC_PNT	WIN32_MANGLE(malloc)(DMALLOC_SIZE size);

/*
 * DMALLOC_PNT calloc
 *
 * Overloading the calloc(3) function.  Returns a block of zeroed memory.
 *
 * Returns a valid pointer on success or NULL on failure.
 *
 * ARGUMENTS:
 *
 * num_elements -> Number of elements being allocated.
 *
 * size -> The number of bytes in each element.
 */
extern
RESTRICT
DMALLOC_PNT	WIN32_MANGLE(calloc)(DMALLOC_SIZE num_elements, DMALLOC_SIZE size);

/*
 * DMALLOC_PNT realloc
 *
 * Overload of realloc(3).  Resizes and old pointer to a new number of bytes.
 *
 * Returns a valid pointer on success or NULL on failure.
 *
 * ARGUMENTS:
 *
 * old_pnt -> Pointer to an existing memory chunk that we are
 * resizing.  If this is NULL then it basically does a malloc.
 *
 * new_size -> New number of bytes requested for the old pointer.
 */
extern
RESTRICT
DMALLOC_PNT	WIN32_MANGLE(realloc)(DMALLOC_PNT old_pnt, DMALLOC_SIZE new_size);

/*
 * DMALLOC_PNT recalloc
 *
 * Overload of recalloc(3) which exists on some systems.  Resizes and
 * old pointer to a new number of bytes.  If we are expanding, then
 * any new bytes will be zeroed.
 *
 * Returns a valid pointer on success or NULL on failure.
 *
 * ARGUMENTS:
 *
 * old_pnt -> Pointer to an existing memory chunk that we are resizing.
 *
 * new_size -> New number of bytes requested for the old pointer.
 */
extern
DMALLOC_PNT	WIN32_MANGLE(recalloc)(DMALLOC_PNT old_pnt, DMALLOC_SIZE new_size);

/*
 * DMALLOC_PNT memalign
 *
 * Overloading the memalign(3) function.  Allocate and return a memory
 * block of a certain size which have been aligned to a certain
 * alignment.
 *
 * Returns a valid pointer on success or NULL on failure.
 *
 * ARGUMENTS:
 *
 * alignment -> Value to which the allocation must be aligned.  This
 * should probably be a multiple of 2 with a maximum value equivalent
 * to the block-size which is often 1k or 4k.
 *
 * size -> Number of bytes requested.
 */
extern
DMALLOC_PNT	WIN32_MANGLE(memalign)(DMALLOC_SIZE alignment, DMALLOC_SIZE size);

/*
 * DMALLOC_PNT valloc
 *
 * Overloading the valloc(3) function.  Allocate and return a memory
 * block of a certain size which have been aligned to page boundaries
 * which are often 1k or 4k.
 *
 * Returns a valid pointer on success or NULL on failure.
 *
 * ARGUMENTS:
 *
 * size -> Number of bytes requested.
 */
extern
DMALLOC_PNT	WIN32_MANGLE(valloc)(DMALLOC_SIZE size);

#ifndef DMALLOC_STRDUP_MACRO
/*
 * DMALLOC_PNT strdup
 *
 * Overload of strdup(3).  Allocate and return an allocated block of
 * memory holding a copy of a string.
 *
 * Returns a valid pointer on success or NULL on failure.
 *
 * ARGUMENTS:
 *
 * string -> String we are duplicating.
 */
extern
char	*WIN32_MANGLE(strdup)(const char *string);
#endif /* ifndef DMALLOC_STRDUP_MACRO */

#ifndef DMALLOC_STRNDUP_MACRO
/*
 * DMALLOC_PNT strndup
 *
 * Overload of strndup(3).  Allocate and return an allocated block of
 * memory holding a copy of a string with a maximum length.
 *
 * Returns a valid pointer on success or NULL on failure.
 *
 * ARGUMENTS:
 *
 * string -> String we are duplicating.
 *
 * max_len -> Max length of the string to duplicate.
 */
extern
DMALLOC_API
char	*WIN32_MANGLE(strndup)(const char *string, const DMALLOC_SIZE max_len);
#endif /* ifndef DMALLOC_STRNDUP_MACRO */

/*
 * DMALLOC_FREE_RET free
 *
 * Release a pointer back into the heap.
 *
 * Returns FREE_ERROR, FREE_NOERROR or void depending on whether STDC
 * is defined by your compiler.
 *
 * ARGUMENTS:
 *
 * pnt -> Existing pointer we are freeing.
 */
extern
DMALLOC_FREE_RET	WIN32_MANGLE(free)(DMALLOC_PNT pnt);

/*
 * DMALLOC_FREE_RET cfree
 *
 * Same as free.
 *
 * Returns FREE_ERROR, FREE_NOERROR or void depending on whether STDC
 * is defined by your compiler.
 *
 * ARGUMENTS:
 *
 * pnt -> Existing pointer we are freeing.
 */
extern
DMALLOC_API
DMALLOC_FREE_RET	WIN32_MANGLE(cfree)(DMALLOC_PNT pnt);
