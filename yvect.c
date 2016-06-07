/* ********************************************************
 * *                       YVECTOR                        *
 * ******************************************************** */

/* ************ PRIVATE DEFINITIONS AND MACROS ************ */
/*! @define _YVECT_DEFAULT_SIZE DEfault size of yvectors. */
#define _YVECT_DEFAULT_SIZE	4096

/*! @define _YVECT_ROUNDSIZE Round a size to 8KB bound. */
#define _YVECT_ROUNDSIZE(s)	((s) + ((s) % 8192))

/*! @define _YVECT_SIZE Compute the size of a new yvector's buffer. */
#define _YVECT_SIZE(s)	_YVECT_ROUNDSIZE(!(s) ? _YSTR_DEFAULT_SIZE : \
			                 ((s) > GB ? ((s) + GB) : ((s) * 2)))

/*! @define _YVECT_HEAD Get a pointer to a yvector's header. */
#define _YVECT_HEAD(p)	((yvect_head_t*)((p) - sizeof(yvect_head_t)))

/* ************ PRIVATE STRUCTURES AND TYPES ************** */
/*!
 * @struct	yvect_head_s
 *		Structure used for the head of yvectors.
 * @field	total	Total size of the yvector.
 * @field	used	Used size of the yvector.
 */
struct yvect_head_s
{
	size_t total;
	size_t used;
};

/*! @typedef yvect_head_t See yvect_head_s structure. */
typedef struct yvect_head_s yvect_head_t;

/* ************ PRIVATE FUNCTION DECLARATIONS ************* */
static yvect_t _yv_create(size_t size);
static void _yv_free(yvect_t *v, void (*f)(void*, void*), void *data);
static void _yv_trunc(yvect_t v, void (*f)(void*, void*), void *data);
static int _yv_setsz(yvect_t *v, size_t sz);
static size_t _yv_len(yvect_t v);
static int _yv_cat(yvect_t *dest, yvect_t src);
static int _yv_ncat(yvect_t *dest, yvect_t src, size_t n);
static yvect_t _yv_dup(yvect_t v);
static yvect_t _yv_concat(yvect_t v1, yvect_t v2);
static int _yv_put(yvect_t *v, void *e);
static int _yv_add(yvect_t *v, void *e);
static int _yv_ins(yvect_t *v, void *e, size_t i);
static void *_yv_pop(yvect_t v);
static void *_yv_get(yvect_t v);
static void *_yv_ext(yvect_t v, size_t i);
static void _yv_uniq(yvect_t v);
static void _yv_sort(yvect_t v, int (*f)(const void*, const void*));
static int _yv_search(yvect_t v, void *e, int (*f)(const void*, const void*));
static void _yv_foreach(yvect_t v, yvect_function_t func, void *user_data);

/*!
 * @function	_yv_create
 *		Creates a new yvector of the given size. Use the default size (4 KB)
 *		if the given size is zero.
 * @param	size	Size of the new yvector.
 * @return	The created yvector.
 */
static yvect_t _yv_create(size_t size) {
	void		**nv;
	yvect_head_t	*y;

	size = !size ? _YVECT_DEFAULT_SIZE : ((size * sizeof(void*)) + sizeof(yvect_head_t));
	if (!(nv = (void**)_ymalloc(size)))
		return (NULL);
	y = (yvect_head_t*)nv;
	nv = (void**)((void*)nv + sizeof(yvect_head_t));
	y->total = size;
	y->used = 0;
	*nv = NULL;
	return ((yvect_t)nv);
}
/*!
 * @function	_yv_free
 *		Delete an existing yvector.
 * @param	v	A pointer to the yvector.
 * @param	f	Pointer to a function that is called to delete each
 *			element of the vector. Could be NULL ; otherwise, must
 *			have this prototype : void f(void *elem, void *data);
 * @param	data	Pointer to data that could be given to the delete
 *			callback (see previous parameter). Could be NULL.
 */
static void _yv_free(yvect_t *v, void (*f)(void*, void*), void *data) {
	yvect_head_t *y;
	size_t i;

	if (!v || !*v)
		return;
	y = _YVECT_HEAD(*v);
	if (f) {
		for (i = 0; i < y->used; ++i)
			f((*v)[i], data);
	}
	_yfree(y);
	*v = NULL;
}
/*!
 * @function	_yv_trunc
 *		Truncate an existing yvector. The allocated memory doesn't
 *		change.
 * @param	v	The yvector.
 * @param	f	Pointer to a function that is called to delete each
 *			element of the vector. Could be NULL ; otherwise, must
 *			have this prototype : void f(void *elem, void *data);
 * @param	data	Pointer to data that could be given to the delete
 *			callback (see previous parameter). Could be NULL.
 */
static void _yv_trunc(yvect_t v, void (*f)(void*, void*), void *data) {
	yvect_head_t *y;
	size_t i;

	if (!v || !*v)
		return;
	y = _YVECT_HEAD(*v);
	if (f) {
		for (i = 0; i < y->used; ++i)
			f(v[i], data);
	}
	y->used = 0;
	*v = NULL;
}
/*!
 * @function	_yv_setsz
 *		Set the minimum size of a yvector.
 * @param	v	A pointer to the yvector.
 * @param	sz	The minimum size for this yvector.
 * @return	0 if an error occurs, 1 otherwise.
 */
static int _yv_setsz(yvect_t *v, size_t sz) {
	yvect_head_t *y, *ny;
	size_t totalsz;
	void **nv;

	if (!v || !*v)
		return (0);
	y = _YVECT_HEAD(**v);
	if (sz < y->total)
		return (1);
	totalsz = _YVECT_SIZE(sz);
	if (!(nv = (void**)_ymalloc((totalsz * sizeof(void*)) +
	                            sizeof(yvect_head_t))))
		return (0);
	ny = (yvect_head_t*)nv;
	nv = (void**)((void*)nv + sizeof(yvect_head_t));
	ny->total = totalsz;
	ny->used = y->used;
	memcpy(nv, **v, (y->used + 1) * sizeof(void*));
	_yfree(y);
	*v = nv;
	return (1);
}
/*!
 * @function	_yv_len
 *		Return the length of a yvector (its used size).
 * @param	v	The yvector.
 * @return	The yvector's length.
 */
static size_t _yv_len(yvect_t v) {
	if (!v)
		return (0);
	return (((yvect_head_t*)((void*)v - sizeof(yvect_head_t)))->used);
}
/*!
 * @function	_yv_cat
 *		Concatenate a yvector at the end of another one.
 * @param	dest	A pointer to the yvector to extend.
 * @param	src	A yvector.
 * @return	0 if an error occurs, 1 otherwise.
 */
static int _yv_cat(yvect_t *dest, yvect_t src) {
	size_t srcsz, vectsz, totalsz;
	yvect_head_t *y, *ny;
	void **nv;

	if (!src || !dest || !*dest || !(srcsz = _yv_len(src)))
		return (1);
	y = _YVECT_HEAD(*dest);
	if ((y->used + 1 + srcsz) <= y->total) {
		memcpy(*dest + y->used, src, (srcsz + 1) * sizeof(void*));
		y->used += srcsz;
		return (1);
	}
	vectsz = y->used + srcsz;
	totalsz = _YVECT_SIZE(vectsz);
	if (!(nv = (void**)_ymalloc((totalsz * sizeof(void*)) +
	                            sizeof(yvect_head_t))))
		return (0);
	ny = (yvect_head_t*)nv;
	nv = (void**)((void*)nv + sizeof(yvect_head_t));
	ny->total = totalsz;
	ny->used = vectsz;
	memcpy(nv, *dest, y->used * sizeof(void*));
	memcpy(nv + y->used, src, (srcsz + 1) * sizeof(void*));
	_yfree(y);
	*dest = nv;
	return (1);
}
/*!
 * @function	_yv_ncat
 *		Concatenate a given number of elements from a yvector
 *		at the end of another.
 * @param	dest	A pointer to the yvector to extend.
 * @param	src	A yvector.
 * @param	n	The number of elements to copy.
 * @return	0 if an error occurs, 1 otherwise.
 */
static int _yv_ncat(yvect_t *dest, yvect_t src, size_t n) {
	size_t vectsz, totalsz;
	yvect_head_t *y, *ny;
	void **nv;

	if (!src || !dest || !*dest || !n)
		return (1);
	y = _YVECT_HEAD(*dest);
	if ((y->used + 1 + n) <= y->total) {
		memcpy(*dest + y->used, src, n * sizeof(void*));
		y->used += n;
		(*dest)[y->used] = NULL;
		return (1);
	}
	vectsz = y->used + n;
	totalsz = _YVECT_SIZE(vectsz);
	if (!(nv = (void**)_ymalloc((totalsz * sizeof(void*)) +
	                            sizeof(yvect_head_t))))
		return (0);
	ny = (yvect_head_t*)nv;
	nv = (void**)((void*)nv + sizeof(yvect_head_t));
	ny->total = totalsz;
	ny->used = vectsz;
	memcpy(nv, *dest, y->used * sizeof(void*));
	memcpy(nv + y->used, src, n * sizeof(void*));
	nv[ny->used] = NULL;
	_yfree(y);
	*dest = nv;
	return (1);
}
/*!
 * @function	_yv_dup
 *		Duplicate a yvector.
 * @param	v	The yvector.
 * @result	The new yvector.
 */
static yvect_t _yv_dup(yvect_t v) {
	yvect_head_t *y, *ny;
	void **nv;

	if (!v)
		return (NULL);
	y = _YVECT_HEAD(v);
	if (!(nv = (void**)_ymalloc((y->total * sizeof(void*)) +
	                            sizeof(yvect_head_t))))
		return (NULL);
	ny = (yvect_head_t*)nv;
	nv = (void**)((void*)nv + sizeof(yvect_head_t));
	ny->total = y->total;
	ny->used = y->used;
	memcpy(nv, v, (y->used + 1) * sizeof(void*));
	return (nv);
}
/*!
 * @function	_yv_concat
 *		Concatenate 2 yvectors to create a new one.
 * @param	v1	The first yvector.
 * @param	v2	The second yvector.
 * @return	The new ystring.
 */
static yvect_t _yv_concat(yvect_t v1, yvect_t v2) {
	void **nv;

	nv = _yv_dup(v1);
	_yv_cat(&nv, v2);
	return (nv);
}
/*!
 * @function	_yv_put
 *		Add an element at the beginning of a yvector.
 * @param	v	A pointer to the yvector.
 * @param	e	A pointer to the element.
 * @return	0 if an error occurs, 1 otherwise.
 */
static int _yv_put(yvect_t *v, void *e) {
	size_t vectsz, totalsz;
	int i;
	yvect_head_t *y, *ny;
	void **nv;

	if (!v || !*v)
		return (0);
	y = _YVECT_HEAD(*v);
	if ((y->used + 2) <= y->total) {
		for (i = y->used; i >= 0; --i)
			(*v)[i + 1] = (*v)[i];
		(*v)[0] = e;
		y->used++;
		return (1);
	}
	vectsz = y->used + 1;
	totalsz = _YVECT_SIZE(vectsz);
	if (!(nv = (void**)_ymalloc((totalsz * sizeof(void*)) +
	                            sizeof(yvect_head_t))))
		return (0);
	ny = (yvect_head_t*)nv;
	nv = (void**)((void*)nv + sizeof(yvect_head_t));
	ny->total = totalsz;
	ny->used = vectsz;
	nv[0] = e;
	memcpy((void*)((void*)nv + sizeof(void*)), *v, (y->used + 1) * sizeof(void*));
	_yfree(y);
	*v = nv;
	return (1);
}
/*!
 * @function	_yv_add
 *		Add an element at the end of a yvector.
 * @param	v	A pointer to the yvector.
 * @param	e	A pointer to the element.
 * @return	0 if an error occurs, 1 otherwise.
 */
static int _yv_add(yvect_t *v, void *e) {
	size_t vectsz, totalsz;
	yvect_head_t *y, *ny;
	void **nv;

	if (!v || !*v)
		return (0);
	y = _YVECT_HEAD(*v);
	if ((y->used + 2) <= y->total) {
		(*v)[y->used] = e;
		(*v)[y->used + 1] = NULL;
		y->used++;
		return (1);
	}
	vectsz = y->used + 1;
	totalsz = _YVECT_SIZE(vectsz);
	if (!(nv = (void**)_ymalloc((totalsz * sizeof(void*)) +
	                            sizeof(yvect_head_t))))
		return (0);
	ny = (yvect_head_t*)nv;
	nv = (void**)((void*)nv + sizeof(yvect_head_t));
	ny->total = totalsz;
	ny->used = vectsz;
	memcpy(nv, *v, y->used * sizeof(void*));
	nv[y->used] = e;
	nv[ny->used] = NULL;
	_yfree(y);
	*v = nv;
	return (1);
}
/*!
 * @function	_yv_ins
 *		Insert an element at the given offset of a yvector. All
 *		elements placed at this offset and after are shifted to the
 *		right.
 * @param	v	A pointer to the yvector.
 * @param	e	A pointer to the element.
 * @param	i	Offset of the element in the vector. Must be less or
 *			equal to the vector's used size.
 * @return	0 if an error occurs, 1 otherwise.
 */
static int _yv_ins(yvect_t *v, void *e, size_t i) {
	yvect_head_t *y;
	size_t j;

	if (!v || !*v)
		return (0);
	y = _YVECT_HEAD(*v);
	if (!v || !*v || !_yv_setsz(v, y->used + 2))
		return (0);
	if (i > y->used)
		return (0);
	for (j = i; j < (y->used + 1); ++j)
		(*v)[j + 1] = (*v)[j];
	(*v)[i] = e;
	y->used++;
	return (1);
}
/*!
 * @function	_yv_pop
 *		Remove the first element of a yvector and return it.
 * @param	v	The yvector.
 * @return	A pointer to the removed element.
 */
static void *_yv_pop(yvect_t v) {
	yvect_head_t *y;
	void *res;
	size_t i;

	if (!v)
		return (NULL);
	y = _YVECT_HEAD(v);
	if (!y->used)
		return (NULL);
	res = *v;
	for (i = 0; i < y->used; ++i)
		v[i] = v[i + 1];
	y->used--;
	return (res);
}
/*!
 * @function	_yv_get
 *		Remove the last element of a yvector and return it.
 * @param	v	The yvector.
 * @return	A pointer to the removed element.
 */
static void *_yv_get(yvect_t v) {
	yvect_head_t *y;
	void *res;

	if (!v)
		return (NULL);
	y = _YVECT_HEAD(v);
	if (!y->used)
		return (NULL);
	res = v[y->used - 1];
	v[y->used - 1] = NULL;
	y->used--;
	return (res);
}
/*!
 * @function	_yv_ext
 *		Extract the element placed at the given offset of a yvector.
 *		All elements placed after the offset are shifted to the left.
 * @param	v	The yvector.
 * @param	i	Offset of the element in the vector. Must be less than
 *			the vector's used size.
 * @return	A pointer to the removed element.
 */
static void *_yv_ext(yvect_t v, size_t i) {
	yvect_head_t *y;
	void *res;
	size_t j;

	if (!v)
		return (NULL);
	y = _YVECT_HEAD(v);
	if (i >= y->used)
		return (NULL);
	res = v[i];
	for (j = i; j < y->used; ++j)
		v[j] = v[j + 1];
	y->used--;
	return (res);
}
/*!
 * @function	_yv_uniq
 *		Remove all values of a yvector to let only one entry of
 *		each value.
 * @param	v	The yvector.
 */

static void _yv_uniq(yvect_t v) {
	yvect_head_t *y;
	size_t i, j, k;

	if (!v)
		return;
	y = _YVECT_HEAD(v);
	for (i = 0; i < y->used; ++i) {
		for (j = i + 1; j < y->used; ++j) {
			if (v[i] == v[j]) {
				for (k = j + 1; k < (y->used - 1); ++k)
					v[k] = v[k + 1];
				y->used--;
			}
		}
	}
}
/*!
 * @function	_yv_sort
 *		Do a quick sort of all elements of a yvector. See qsort(3).
 * @param	v	The yvector.
 * @param	f	A pointer to the function used to compare elements.
 */
static void _yv_sort(yvect_t v, int (*f)(const void*, const void*)) {
	yvect_head_t *y;

	if (!v)
		return;
	y = _YVECT_HEAD(v);
	qsort(v, y->used, sizeof(void*), f);
}
/*!
 * @function	_yv_search
 *		Search the offset of an element in a yvector. WARNING: the
 *		yvector must be sorted (using _yv_sort()) because this
 *		function uses dichotomy.
 * @param	v	The yvector.
 * @param	e	The element to compare.
 * @param	f	A pointer to the function used to compare elements.
 * @return	The offset of the element in the yvector, or (-1) if the
 *		element doesn't be finded.
 */
static int _yv_search(yvect_t v, void *e, int (*f)(const void*, const void*)) {
	yvect_head_t *y;
	int o_start, o_end, o_pivot;
	int cmp_res;

	if (!v || !f)
		return (-1);
	y = _YVECT_HEAD(v);
	o_start = 0;
	o_end = y->used - 1;
	for (; ; ) {
		o_pivot = (o_end + o_start) / 2;
		if (!(cmp_res = f(e, v[o_pivot])))
			return (o_pivot);
		if (cmp_res < 0) {
			if (o_pivot == (o_start + 1)) {
				if (!f(e, v[o_start]))
					return (o_start);
				else
					return (-1);
				}
				o_end = o_pivot;
		} else {
			if (o_pivot == (o_end - 1)) {
				if (!f(e, v[o_end]))
					return (o_end);
				else
					return (-1);
			}
			o_start = o_pivot;
		}
	}
}

/**
 * @function	_yv_foreach
 *		Apply a function on every elements of a vector.
 * @param	v		The yvector.
 * @param	func		Pointer to the executed function.
 * @param	user_data	Pointer to some user data.
 */
static void _yv_foreach(yvect_t v, yvect_function_t func, void *user_data) {
	size_t offset;
	yvect_head_t *y;

	y = _YVECT_HEAD(v);
	for (offset = 0; offset < y->used; ++offset) {
		func(offset, v[offset], user_data);
	}
}
