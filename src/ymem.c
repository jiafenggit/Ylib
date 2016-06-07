/* ********************************************************
 * *                         YMEM                         *
 * ******************************************************** */

static void *_ymalloc(size_t size);
static void *_ycalloc(size_t nbr, size_t size);
static void _yfree(void *ptr);

/*!
 * @function	_ymalloc
 *		Allocate memory.
 * @param	size	The size of the memory space that must be allocated.
 * @return	A pointer to the allocated memory.
 */
static void *_ymalloc(size_t size) {
#ifdef USE_BOEHM_GC
	return (GC_MALLOC(size));
#else
	return (calloc(1, size));
#endif
}
/*!
 * @function	_ycalloc
 *		Alloc chunks of memory.
 * @param	nbr	Number of chunks.
 * @param	size	Size of one chunk.
 * @return	A pointer to the allocated memory.
 */
static void *_ycalloc(size_t nbr, size_t size) {
#ifdef USE_BOEHM_GC
	return (GC_MALLOC(nbr * size));
#else
	return (calloc(nbr, size));
#endif
}
/*!
 * @function	_yfree
 *		Freed memory.
 * @param	ptr	Pointer to the allocated zone.
 */
static void _yfree(void *ptr) {
#ifdef USE_BOEHM_GC
	GC_FREE(ptr);
#else
	free(ptr);
#endif
}

