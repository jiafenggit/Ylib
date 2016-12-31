/* ********************************************************
 * *                         YMEM                         *
 * ******************************************************** */

#include "ymem.h"

/* Allocate memory. */
void *ymalloc(size_t size) {
#ifdef USE_BOEHM_GC
	return (GC_MALLOC(size));
#else
	return (calloc(1, size));
#endif
}

/* Alloc chunks of memory. */
void *ycalloc(size_t nbr, size_t size) {
#ifdef USE_BOEHM_GC
	return (GC_MALLOC(nbr * size));
#else
	return (calloc(nbr, size));
#endif
}

/* Realloc memory. */
void *yrealloc(void *ptr, size_t size) {
#ifdef USE_BOEHM_GC
	return (GC_REALLOC(ptr, size));
#else
	return (realloc(ptr, size));
#endif
}

/* Freed memory. */
void *yfree(void *ptr) {
	if (!ptr)
		return (NULL);
#ifdef USE_BOEHM_GC
	GC_FREE(ptr);
#else
	free(ptr);
#endif
	return (NULL);
}

