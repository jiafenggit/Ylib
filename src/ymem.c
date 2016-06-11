/* ********************************************************
 * *                         YMEM                         *
 * ******************************************************** */

#include "ymem.h"

void *ymalloc(size_t size) {
#ifdef USE_BOEHM_GC
	return (GC_MALLOC(size));
#else
	return (calloc(1, size));
#endif
}

void *ycalloc(size_t nbr, size_t size) {
#ifdef USE_BOEHM_GC
	return (GC_MALLOC(nbr * size));
#else
	return (calloc(nbr, size));
#endif
}

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

