/* ********************************************************
 * *                       YVECTOR                        *
 * ******************************************************** */
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include "ymem.h"
#include "yvector.h"

/* ************ PRIVATE DEFINITIONS AND MACROS ************ */
/** @define _YVECT_DEFAULT_SIZE Default size of yvectors. */
#define _YVECTOR_DEFAULT_SIZE	8
/** @define _YVECT_ROUND_SIZE Round the size to the next power of 2. */
#define _YVECTOR_ROUND_SIZE(s)	((size_t)pow(2, ceil(log(s) / log(2))))

/* ************ FUNCTIONS ************* */
yvector_t *yvector_init(yvector_t *v) {
	if (!v)
		return (NULL);
	*v = (yvector_t){
		.size = _YVECTOR_DEFAULT_SIZE,
		.count = 0,
		.data = ymalloc(_YVECTOR_DEFAULT_SIZE * sizeof(void*))
	};
	if (!v->data)
		return (NULL);
	return (v);
}

yvector_t *yvector_new() {
	yvector_t *v = ymalloc(sizeof(yvector_t));
	if (!v)
		return (NULL);
	return (yvector_init(v));
}

void yvector_clear(yvector_t *v, void (*f)(void*, void*), void *user_data) {
	if (!v || !v->size)
		return;
	if (f) {
		for (size_t i = 0; i < v->count; ++i)
			f(v->data[i], user_data);
	}
	free(v->data);
	v->size = v->count = 0;
}

void yvector_free(yvector_t *v, void (*f)(void*, void*), void *user_data) {
	if (!v)
		return;
	yvector_clear(v, f, user_data);
	free(v);
}

yvector_t *yvector_duplicate(yvector_t *v) {
	return (yvector_slice(v, 0, 0));
}

void yvector_truncate(yvector_t *v, void (*f)(void*, void*), void *user_data) {
	if (!v)
		return;
	if (f) {
		for (size_t i = 0; i < v->count; ++i)
			f(v->data[i], user_data);
	}
	v->count = 0;
}

bool yvector_resize(yvector_t *v, size_t size) {
	void *new_data;

	if (!v || !size || v->count > size ||
	    !(new_data = yrealloc(v->data, _YVECTOR_ROUND_SIZE(size))))
		return (false);
	v->data = new_data;
	v->size = size;
	return (true);
}

bool yvector_shrink(yvector_t *v) {
	void * new_data;

	if (!v || !v->count ||
	    !(new_data = yrealloc(v->data, _YVECTOR_ROUND_SIZE(v->count))))
		return (false);
	v->data = new_data;
	v->size = v->count;
	return (true);
}

size_t yvector_count(yvector_t *v) {
	if (!v)
		return (0);
	return (v->count);
}

bool yvector_add(yvector_t *v, void *e) {
	if (!v || (v->count == v->size && !yvector_resize(v, (v->size * 2))))
		return (false);
	memmove(&v->data[1], v->data, v->count);
	v->data[0] = e;
	return (true);
}

bool yvector_push(yvector_t *v, void *e) {
	if (!v || (v->count == v->size && !yvector_resize(v, (v->size * 2))))
		return (false);
	v->data[v->count++] = e;
	return (true);
}

bool yvector_insert(yvector_t *v, void *e, size_t offset) {
	if (!v || (v->count == v->size && !yvector_resize(v, (v->size * 2))))
		return (false);
	if (offset >= v->count)
		return (yvector_add(v, e));
	memmove(&v->data[offset + 1], &v->data[offset], (v->count - offset));
	v->data[offset] = e;
}

void *yvector_get(yvector_t *v, size_t offset) {
	if (!v || offset >= v->count)
		return (NULL);
	return (v->data[offset]);
}

void *yvector_shift(yvector_t *v) {
	if (!v || !v->count)
		return (NULL);
	void *result = v->data[0];
	memmove(v->data, &v->data[1], (v->count - 1));
	v->count--;
	return (result);
}

void *yvector_pop(yvector_t *v) {
	if (!v || !v->count)
		return (NULL);
	v->count--;
	return (v->data[v->count]);
}

void *yvector_extract(yvector_t *v, size_t offset) {
	if (!v || offset >= v->count)
		return (NULL);
	void *result = v->data[offset];
	memmove(&v->data[offset], &v->data[offset + 1], (v->count - offset));
	v->count--;
	return (result);
}

yvector_t *yvector_slice(yvector_t *v, size_t offset, size_t length) {
	if (!v || offset >= v->count)
		return (NULL);
	if (!length || (offset + length) > v->count)
		length = v->count - offset;
	yvector_t *nv = ymalloc(sizeof(yvector_t));
	if (!nv)
		return (NULL);
	size_t size = _YVECTOR_ROUND_SIZE(length);
	*nv = (yvector_t){
		.size = size,
		.count = length,
		.data = ymalloc(size * sizeof(void*))
	};
	if (!nv->data) {
		yfree(nv);
		return (NULL);
	}
	memcpy(nv->data, &v->data[offset], length);
	return (nv);
}

yvector_t *yvector_splice(yvector_t *v, size_t offset, size_t length) {
	if (!v || offset >= v->count)
		return (NULL);
	if (!length || (offset + length) > v-> count)
		length = v->count - offset;
	yvector_t *nv = yvector_slice(v, offset, length);
	if (!nv)
		return (NULL);
	memmove(&v->data[offset], &v->data[offset + length], (v->count - offset - length));
	v->count -= length;
	return (nv);
}

bool yvector_inject(yvector_t *dest, yvector_t *src, size_t offset) {
	if (!dest || !src)
		return (false);
	offset = (offset > dest->count) ? dest->count : offset;
	size_t total = dest->count + src->count;
	if (dest->size < total && !yvector_resize(dest, total))
		return (false);
	if (offset == dest->count) {
		memcpy(&dest->data[offset], src->data, src->count);
		return (true);
	}
	memmove(&dest->data[offset + src->count], &dest->data[offset], src->count);
	memcpy(&dest->data[offset], src->data, src->count);
	dest->count += src->count;
	return (true);
}

bool yvector_ninject(yvector_t *dest, size_t offset, yvector_t *src, size_t start, size_t length) {
	if (!dest || !src || (start + length) > src->count)
		return (false);
	offset = (offset > dest->count) ? dest->count : offset;
	length = !length ? (src->count - start) : length;
	size_t total = dest->count + length;
	if (dest->size < total && !yvector_resize(dest, total))
		return (false);
	if (offset == dest->count) {
		memcpy(&dest->data[offset], &src->data[start], length);
		return (true);
	}
	memmove(&dest->data[offset + length], &dest->data[offset], length);
	memcpy(&dest->data[offset], &src->data[start], length);
	dest->count += length;
	return (true);
}

bool yvector_append(yvector_t *dest, yvector_t *src) {
	return (yvector_inject(dest, src, dest->count));
}

bool yvector_prepend(yvector_t *dest, yvector_t *src) {
	return (yvector_inject(dest, src, 0));
}

void yvector_uniq(yvector_t *v) {
	if (!v)
		return;
	for (size_t i = 0; i < v->count; ++i) {
		for (size_t j = i + 1; j < v->count; ++j) {
			if (v->data[i] == v->data[j]) {
				for (size_t k = j + 1; k < (v->count - 1); ++k)
					v->data[k] = v->data[k + 1];
				v->count--;
			}
		}
	}
}

void yvector_sort(yvector_t *v, int (*f)(const void*, const void*, void*), void *user_data) {
	if (!v)
		return;
	qsort_r(v->data, v->count, sizeof(void*), f, user_data);
}

long long int yvector_search(yvector_t *v, void *e, int (*f)(const void*, const void*)) {
	int o_start, o_end, o_pivot;
	int cmp_res;

	if (!v || !f)
		return (-1);
	o_start = 0;
	o_end = v->count - 1;
	for (; ; ) {
		o_pivot = (o_end + o_start) / 2;
		if (!(cmp_res = f(e, v->data[o_pivot])))
			return (o_pivot);
		if (cmp_res < 0) {
			if (o_pivot == (o_start + 1)) {
				if (!f(e, v->data[o_start]))
					return (o_start);
				else
					return (-1);
			}
			o_end = o_pivot;
		} else {
			if (o_pivot == (o_end - 1)) {
				if (!f(e, v->data[o_end]))
					return (o_end);
				else
					return (-1);
			}
			o_start = o_pivot;
		}
	}
}

void yvector_foreach(yvector_t *v, yvector_function_t func, void *user_data) {
	for (size_t offset = 0; offset < v->count; ++offset) {
		func(offset, v->data[offset], user_data);
	}
}
