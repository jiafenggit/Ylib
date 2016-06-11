/* ********************************************************
 * *                       YVECTOR                        *
 * ******************************************************** */

#include "ylib.h"

/* ************ PRIVATE DEFINITIONS AND MACROS ************ */
/*! @define _YVECT_DEFAULT_SIZE DEfault size of yvectors. */
#define _YVECT_DEFAULT_SIZE	4096

/*! @define _YVECT_ROUNDSIZE Round a size to 8KB bound. */
#define _YVECT_ROUNDSIZE(s)	((s) + ((s) % 8192))

/*! @define _YVECT_SIZE Compute the size of a new yvector's buffer. */
#define _YVECT_SIZE(s)	_YVECT_ROUNDSIZE(!(s) ? _YVECT_DEFAULT_SIZE : \
			                 ((s) > GB ? ((s) + GB) : ((s) * 2)))

/*! @define _YVECT_HEAD Get a pointer to a yvector's header. */
#define _YVECT_HEAD(p)	((yvect_head_t*)((void*)(p) - sizeof(yvect_head_t)))

/* ************ PRIVATE STRUCTURES AND TYPES ************** */
/*!
 * @struct	yvect_head_s
 *		Structure used for the head of yvectors.
 * @field	total	Total size of the yvector.
 * @field	used	Used size of the yvector.
 */
struct yvect_head_s {
	size_t total;
	size_t used;
};

/*! @typedef yvect_head_t See yvect_head_s structure. */
typedef struct yvect_head_s yvect_head_t;

/* ************ FUNCTIONS ************* */
yvect_t yv_new(size_t size) {
	void		**nv;
	yvect_head_t	*y;

	size = !size ? _YVECT_DEFAULT_SIZE : ((size * sizeof(void*)) + sizeof(yvect_head_t));
	if (!(nv = (void**)ymalloc(size)))
		return (NULL);
	y = (yvect_head_t*)nv;
	nv = (void**)((void*)nv + sizeof(yvect_head_t));
	y->total = size;
	y->used = 0;
	*nv = NULL;
	return ((yvect_t)nv);
}

void yv_free(yvect_t *v, void (*f)(void*, void*), void *data) {
	yvect_head_t *y;
	size_t i;

	if (!v || !*v)
		return;
	y = _YVECT_HEAD(*v);
	if (f) {
		for (i = 0; i < y->used; ++i)
			f((*v)[i], data);
	}
	yfree(y);
	*v = NULL;
}

void yv_trunc(yvect_t v, void (*f)(void*, void*), void *data) {
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

int yv_setsz(yvect_t *v, size_t sz) {
	yvect_head_t *y, *ny;
	size_t totalsz;
	void **nv;

	if (!v || !*v)
		return (0);
	y = _YVECT_HEAD(**v);
	if (sz < y->total)
		return (1);
	totalsz = _YVECT_SIZE(sz);
	if (!(nv = (void**)ymalloc((totalsz * sizeof(void*)) +
	                            sizeof(yvect_head_t))))
		return (0);
	ny = (yvect_head_t*)nv;
	nv = (void**)((void*)nv + sizeof(yvect_head_t));
	ny->total = totalsz;
	ny->used = y->used;
	memcpy(nv, **v, (y->used + 1) * sizeof(void*));
	yfree(y);
	*v = nv;
	return (1);
}

size_t yv_len(yvect_t v) {
	if (!v)
		return (0);
	return (((yvect_head_t*)((void*)v - sizeof(yvect_head_t)))->used);
}

int yv_cat(yvect_t *dest, yvect_t src) {
	size_t srcsz, vectsz, totalsz;
	yvect_head_t *y, *ny;
	void **nv;

	if (!src || !dest || !*dest || !(srcsz = yv_len(src)))
		return (1);
	y = _YVECT_HEAD(*dest);
	if ((y->used + 1 + srcsz) <= y->total) {
		memcpy(*dest + y->used, src, (srcsz + 1) * sizeof(void*));
		y->used += srcsz;
		return (1);
	}
	vectsz = y->used + srcsz;
	totalsz = _YVECT_SIZE(vectsz);
	if (!(nv = (void**)ymalloc((totalsz * sizeof(void*)) +
	                            sizeof(yvect_head_t))))
		return (0);
	ny = (yvect_head_t*)nv;
	nv = (void**)((void*)nv + sizeof(yvect_head_t));
	ny->total = totalsz;
	ny->used = vectsz;
	memcpy(nv, *dest, y->used * sizeof(void*));
	memcpy(nv + y->used, src, (srcsz + 1) * sizeof(void*));
	yfree(y);
	*dest = nv;
	return (1);
}

int yv_ncat(yvect_t *dest, yvect_t src, size_t n) {
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
	if (!(nv = (void**)ymalloc((totalsz * sizeof(void*)) +
	                            sizeof(yvect_head_t))))
		return (0);
	ny = (yvect_head_t*)nv;
	nv = (void**)((void*)nv + sizeof(yvect_head_t));
	ny->total = totalsz;
	ny->used = vectsz;
	memcpy(nv, *dest, y->used * sizeof(void*));
	memcpy(nv + y->used, src, n * sizeof(void*));
	nv[ny->used] = NULL;
	yfree(y);
	*dest = nv;
	return (1);
}

yvect_t yv_dup(yvect_t v) {
	yvect_head_t *y, *ny;
	void **nv;

	if (!v)
		return (NULL);
	y = _YVECT_HEAD(v);
	if (!(nv = (void**)ymalloc((y->total * sizeof(void*)) +
	                            sizeof(yvect_head_t))))
		return (NULL);
	ny = (yvect_head_t*)nv;
	nv = (void**)((void*)nv + sizeof(yvect_head_t));
	ny->total = y->total;
	ny->used = y->used;
	memcpy(nv, v, (y->used + 1) * sizeof(void*));
	return (nv);
}

yvect_t yv_concat(yvect_t v1, yvect_t v2) {
	void **nv;

	nv = yv_dup(v1);
	yv_cat(&nv, v2);
	return (nv);
}

int yv_put(yvect_t *v, void *e) {
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
	if (!(nv = (void**)ymalloc((totalsz * sizeof(void*)) +
	                            sizeof(yvect_head_t))))
		return (0);
	ny = (yvect_head_t*)nv;
	nv = (void**)((void*)nv + sizeof(yvect_head_t));
	ny->total = totalsz;
	ny->used = vectsz;
	nv[0] = e;
	memcpy((void*)((void*)nv + sizeof(void*)), *v, (y->used + 1) * sizeof(void*));
	yfree(y);
	*v = nv;
	return (1);
}

int yv_add(yvect_t *v, void *e) {
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
	if (!(nv = (void**)ymalloc((totalsz * sizeof(void*)) +
	                            sizeof(yvect_head_t))))
		return (0);
	ny = (yvect_head_t*)nv;
	nv = (void**)((void*)nv + sizeof(yvect_head_t));
	ny->total = totalsz;
	ny->used = vectsz;
	memcpy(nv, *v, y->used * sizeof(void*));
	nv[y->used] = e;
	nv[ny->used] = NULL;
	yfree(y);
	*v = nv;
	return (1);
}

int yv_ins(yvect_t *v, void *e, size_t i) {
	yvect_head_t *y;
	size_t j;

	if (!v || !*v)
		return (0);
	y = _YVECT_HEAD(*v);
	if (!v || !*v || !yv_setsz(v, y->used + 2))
		return (0);
	if (i > y->used)
		return (0);
	for (j = i; j < (y->used + 1); ++j)
		(*v)[j + 1] = (*v)[j];
	(*v)[i] = e;
	y->used++;
	return (1);
}

void *yv_pop(yvect_t v) {
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

void *yv_get(yvect_t v) {
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

void *yv_ext(yvect_t v, size_t i) {
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

void yv_uniq(yvect_t v) {
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

void yv_sort(yvect_t v, int (*f)(const void*, const void*)) {
	yvect_head_t *y;

	if (!v)
		return;
	y = _YVECT_HEAD(v);
	qsort(v, y->used, sizeof(void*), f);
}

int yv_search(yvect_t v, void *e, int (*f)(const void*, const void*)) {
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

void yv_foreach(yvect_t v, yvect_function_t func, void *user_data) {
	size_t offset;
	yvect_head_t *y;

	y = _YVECT_HEAD(v);
	for (offset = 0; offset < y->used; ++offset) {
		func(offset, v[offset], user_data);
	}
}
