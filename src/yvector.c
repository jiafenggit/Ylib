/* ********************************************************
 * *                       YVECTOR                        *
 * ******************************************************** */
#include <string.h>
#include <math.h>
#include "ymem.h"
#include "yvector.h"

/* ************ PRIVATE DEFINITIONS AND MACROS ************ */
/** @define _YVECTOR_DEFAULT_SIZE Default size of yvectors. */
#define _YVECTOR_DEFAULT_SIZE	8
/** @define _YVECTOR_ROUND_SIZE Round the size to the next power of 2. */
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
	if (!yvector_init(v)) {
		yfree(v);
		return (NULL);
	}
	return (v);
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

yerr_t yvector_resize(yvector_t *v, size_t size) {
	void *new_data;

	if (!v || !size || v->count > size)
		return (YEINVAL);
	if (!(new_data = yrealloc(v->data, _YVECTOR_ROUND_SIZE(size) * sizeof(void*))))
		return (YENOMEM);
	v->data = new_data;
	v->size = size;
	return (YEOK);
}

yerr_t yvector_shrink(yvector_t *v) {
	void *new_data;

	if (!v)
		return (YEINVAL);
	if (!v->count)
		return (YEINVAL);
	if (!(new_data = yrealloc(v->data, _YVECTOR_ROUND_SIZE(v->count) * sizeof(void*))))
		return (YENOMEM);
	v->data = new_data;
	v->size = v->count;
	return (YEOK);
}

size_t yvector_count(yvector_t *v) {
	if (!v)
		return (0);
	return (v->count);
}

yerr_t yvector_add(yvector_t *v, void *e) {
	if (!v)
		return (YEINVAL);
	yerr_t err;
	if (v->count == v->size && (err = yvector_resize(v, (v->size * 2))) != YEOK)
		return (err);
	memmove(&v->data[1], v->data, v->count * sizeof(void*));
	v->data[0] = e;
	return (YEOK);
}

yerr_t yvector_push(yvector_t *v, void *e) {
	if (!v)
		return (YEINVAL);
	yerr_t err;
	if (v->count == v->size && (err = yvector_resize(v, (v->size * 2))) != YEOK)
		return (err);
	v->data[v->count++] = e;
	return (YEOK);
}

yerr_t yvector_insert(yvector_t *v, void *e, size_t offset) {
	if (!v)
		return (YEINVAL);
	yerr_t err;
	if (v->count == v->size && (err = yvector_resize(v, (v->size * 2))) != YEOK)
		return (err);
	if (offset >= v->count)
		return (yvector_push(v, e));
	memmove(&v->data[offset + 1], &v->data[offset], (v->count - offset) * sizeof(void*));
	v->data[offset] = e;
	return (YEOK);
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
	memmove(v->data, &v->data[1], (v->count - 1) * sizeof(void*));
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
	memmove(&v->data[offset], &v->data[offset + 1], (v->count - offset) * sizeof(void*));
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
	memcpy(nv->data, &v->data[offset], length * sizeof(void*));
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
	memmove(&v->data[offset], &v->data[offset + length], (v->count - offset - length) * sizeof(void*));
	v->count -= length;
	return (nv);
}

yerr_t yvector_inject(yvector_t *dest, yvector_t *src, size_t offset) {
	if (!dest || !src)
		return (YEINVAL);
	offset = (offset > dest->count) ? dest->count : offset;
	size_t total = dest->count + src->count;
	yerr_t err;
	if (dest->size < total && (err = yvector_resize(dest, total)) != YEOK)
		return (err);
	if (offset == dest->count) {
		memcpy(&dest->data[offset], src->data, src->count * sizeof(void*));
		return (YEOK);
	}
	memmove(&dest->data[offset + src->count], &dest->data[offset], src->count * sizeof(void*));
	memcpy(&dest->data[offset], src->data, src->count * sizeof(void*));
	dest->count += src->count;
	return (YEOK);
}

yerr_t yvector_ninject(yvector_t *dest, size_t offset, yvector_t *src, size_t start, size_t length) {
	if (!dest || !src || (start + length) > src->count)
		return (YEINVAL);
	offset = (offset > dest->count) ? dest->count : offset;
	length = !length ? (src->count - start) : length;
	size_t total = dest->count + length;
	yerr_t err;
	if (dest->size < total && (err = yvector_resize(dest, total)) != YEOK)
		return (err);
	if (offset == dest->count) {
		memcpy(&dest->data[offset], &src->data[start], length * sizeof(void*));
		return (YEOK);
	}
	memmove(&dest->data[offset + length], &dest->data[offset], length * sizeof(void*));
	memcpy(&dest->data[offset], &src->data[start], length * sizeof(void*));
	dest->count += length;
	return (YEOK);
}

yerr_t yvector_append(yvector_t *dest, yvector_t *src) {
	return (yvector_inject(dest, src, dest->count));
}

yerr_t yvector_prepend(yvector_t *dest, yvector_t *src) {
	return (yvector_inject(dest, src, 0));
}

void yvector_uniq(yvector_t *v) {
	if (!v)
		return;
	for (size_t i = 0; i < v->count; ++i) {
		for (size_t j = i + 1; j < v->count; ++j) {
			if (v->data[i] == v->data[j]) {
				memmove(&v->data[j], &v->data[j + 1], (v->count - j - 1));
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

size_t yvector_search(yvector_t *v, void *e, int (*f)(const void*, const void*)) {
	size_t o_start, o_end, o_pivot;
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
