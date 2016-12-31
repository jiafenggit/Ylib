/* Process this file with the HeaderBrowser tool (http://www.headerbrowser.org)
   to create documentation. */
/**
 * @header      ymem.h
 * @abstract    Memory management
 * @discussion  Functions used for memory management.
 * @version     1.2.0 Dec 29 2016
 * @author      Amaury Bouchard <amaury@amaury.net>
 */
#ifndef __YMEM_H__
#define __YMEM_H__

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif /* __cplusplus || c_plusplus */

#include <stdlib.h>

/**
 * @function	ymalloc
 *		Allocate memory.
 * @param	size	The size of the memory space that must be allocated.
 * @return	A pointer to the allocated memory.
 */
void *ymalloc(size_t size);

/**
 * @function	ycalloc
 *		Alloc chunks of memory.
 * @param	nbr	Number of chunks.
 * @param	size	Size of one chunk.
 * @return	A pointer to the allocated memory.
 */
void *ycalloc(size_t nbr, size_t size);

/**
 * @function	yrealloc
 *		Realloc memory.
 * @param	ptr	Pointer to the allocated zone.
 * @param	size	New size.
 * @return	A pointer to the allocated memory.
 */
void *yrealloc(void *ptr, size_t size);

/**
 * @function	yfree
 *		Freed memory.
 * @param	ptr	Pointer to the allocated zone.
 * @return	NULL
 */
void *yfree(void *ptr);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif /* __cplusplus || c_plusplus */

#endif /* __YMEM_H__ */
