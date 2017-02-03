/* ********************************************************
 * *                        YARRAY                        *
 * ******************************************************** */
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include "ymem.h"
#include "yarray.h"

/* ************ FUNCTIONS ************* */
yarray_t *yarray_init(yarray_t *a, size_t element_size) {
	if (!a)
		return (NULL);
	*a = (yarray_t){
		.size = _YARRAY_DEFAULT_SIZE,
		.count = 0,
		.element_size = element_size,
		.data = ycalloc(_YARRAY_DEFAULT_SIZE, element_size)
	};
	if (!a->data)
		return (NULL);
	return (a);
}

yarray_t *yarray_new(size_t element_size) {
	yarray_t *a = ymalloc(sizeof(yarray_t));
	if (!a)
		return (NULL);
	if (!yarray_init(a, element_size)) {
		yfree(a);
		return (NULL);
	}
	return (a);
}

void yarray_clear(yarray_t *a, yarray_function_t f, void *user_data) {
	if (!a || !a->size)
		return;
	if (f)
		yarray_foreach(a, f, user_data);
	yfree(a->data);
	a->size = a->count = 0;
}

void *yarray_free(yarray_t *a, yarray_function_t f, void *user_data) {
	if (!a)
		return;
	yarray_clear(a, f, user_data);
	yfree(a);
	return (NULL);
}

yarray_t *yarray_duplicate(yarray_t *a) {
	return (yarray_slice(a, 0, 0));
}

void yarray_truncate(yarray_t *a, yarray_function_t f, void *user_data) {
	if (!a)
		return;
	if (f)
		yarray_foreach(a, f, user_data);
	a->count = 0;
}

yerr_t yarray_resize(yarray_t *a, size_t size) {
	if (!a || !size || a->count > size)
		return (YEBADPARAM);
	void *new_data;
	size_t new_size = _YARRAY_ROUND_SIZE(size);
	if (!(new_data = yrealloc(a->data, new_size * a->element_size)))
		return (YENOMEM);
	a->data = new_data;
	a->size = new_size;
	return (YEOK);
}

yerr_t yarray_shrink(yarray_t *a) {
	if (!a)
		return (YEBADPARAM);
	if (!a->count)
		return (YEOK);
	void *new_data;
	size_t new_size = _YARRAY_ROUND_SIZE(a->count);
	if (!(new_data = yrealloc(a->data, new_size * a->element_size)))
		return (YENOMEM);
	a->data = new_data;
	a->size = new_size;
	return (YEOK);
}

yerr_t yarray_revert(yarray_t *a) {
	if (!a)
		return (YEBADPARAM);
	if (a->count <= 1)
		return (YEOK);
	void *new_data;
	size_t new_size = _YARRAY_ROUND_SIZE(a->count);
	if (!(new_data = ymalloc(new_size)))
		return (YENOMEM);
	for (size_t offset_src = a->count, offset_dest = 0; offset_src > 0; --offset_src, ++offset_dest) {
		uintptr_t ptr = (uintptr_t)a->data + ((offset - 1) * a->element_size);
		memcpy(&new_data[offset_dest], (void*)ptr, a->element_size);
	}
	yfree(a->data);
	a->data = new_data;
	a->size = new_size;
	return (YEOK);
}

size_t yarray_count(yarray_t *a) {
	if (!a)
		return (0);
	return (a->count);
}

yerr_t yarray_add(yarray_t *a, void *e) {
	if (!a)
		return (YEBADPARAM);
	if (a->count == a->size) {
		yerr_t err = yarray_resize(a, (a->size * 2));
		if (err != YEOK)
			return (err);
	}
	uintptr_t ptr = (uintptr_t)a->data + a->element_size;
	memmove((void*)ptr, a->data, a->count * a->element_size);
	memcpy(a->data, e, a->element_size);
	a->count++;
	return (YEOK);
}

yerr_t yarray_push(yarray_t *a, void *e) {
	if (!a)
		return (YEBADPARAM);
	if (a->count == a->size) {
		yerr_t err = yarray_resize(a, (a->size * 2));
		if (err != YEOK)
			return (err);
	}
	uintptr_t ptr = (uintptr_t)a->data + (a->count * a->element_size);
	memcpy((void*)ptr, e, a->element_size);
	a->count++;
	return (YEOK);
}

yerr_t yarray_insert(yarray_t *a, void *e, size_t offset) {
	if (!a)
		return (YEBADPARAM);
	if (a->count == a->size) {
		yerr_t err = yarray_resize(a, (a->size * 2));
		if (err != YEOK)
			return (err);
	}
	if (offset >= a->count)
		return (yarray_push(a, e));
	uintptr_t ptr_dest = (uintptr_t)a->data + ((offset + 1) * a->element_size);
	uintptr_t ptr_src = (uintptr_t)a->data + (offset * a->element_size);
	memmove((void*)ptr_dest, (void*)ptr_src, (a->count - offset) * a->element_size);
	memcpy((void*)ptr_src, e, a->element_size);
	return (YEOK);
}

void *yarray_get(yarray_t *a, size_t offset) {
	if (!a || offset >= a->count)
		return (NULL);
	uintptr_t ptr = (uintptr_t)a->data + (offset * a->element_size);
	return ((void*)ptr);
}

void yarray_shift(yarray_t *a) {
	if (!a || !a->count)
		return;
	uintptr_t ptr = (uintptr_t)a->data + a->element_size;
	memmove(a->data, (void*)ptr, (a->count - 1) * a->element_size);
	a->count--;
}

void yarray_pop(yarray_t *a) {
	if (!a || !a->count)
		return;
	a->count--;
}

void yarray_extract(yarray_t *a, size_t offset) {
	if (!a || offset >= a->count)
		return;
	uintptr_t ptr_src = (uintptr_t)a->data + ((offset + 1) * a->element_size);
	uintptr_t ptr_dest = (uintptr_t)a->data + (offset * a->element_size);
	memmove((void*)ptr_dest, (void*)ptr_src, (a->count - offset) * a->element_size);
	a->count--;
}

yarray_t *yarray_slice(yarray_t *a, size_t offset, size_t length) {
	if (!a || offset >= a->count)
		return (NULL);
	if (!length || (offset + length) > a->count)
		length = a->count - offset;
	yarray_t *nv = ymalloc(sizeof(yarray_t));
	if (!nv)
		return (NULL);
	size_t size = _YARRAY_ROUND_SIZE(length);
	*nv = (yarray_t){
		.size = size,
		.count = length,
		.element_size = a->element_size,
		.data = ymalloc(size * a->element_size)
	};
	if (!nv->data) {
		yfree(nv);
		return (NULL);
	}
	uintptr_t ptr = (uintptr_t)a->data + (offset * a->element_size);
	memcpy(nv->data, (void*)ptr, length);
	return (nv);
}

yarray_t *yarray_splice(yarray_t *a, size_t offset, size_t length) {
	if (!a || offset >= a->count)
		return (NULL);
	if (!length || (offset + length) > a-> count)
		length = a->count - offset;
	yarray_t *nv = yarray_slice(a, offset, length);
	if (!nv)
		return (NULL);
	uintptr_t ptr_dest = (uintptr_t)a->data + (offset * a->element_size);
	uintptr_t ptr_src = (uintptr_t)a->data + ((offset + length) * a->element_size);
	memmove((void*)ptr_dest, (void*)ptr_src, (a->count - offset - length) * a->element_size);
	a->count -= length;
	return (nv);
}

yerr_t yarray_inject(yarray_t *dest, yarray_t *src, size_t offset) {
	uintptr_t ptr_dest, ptr_src;

	if (!dest || !src)
		return (YEBADPARAM);
	if (src->element_size != dest->element_size)
		return (YEINCOMPATIBLE);
	offset = (offset > dest->count) ? dest->count : offset;
	size_t total = dest->count + src->count;
	if (dest->size < total) {
		yerr_t err = yarray_resize(dest, total);
		if (err != YEOK)
			return (err);
	}
	if (offset == dest->count) {
		ptr_dest = (uintptr_t)dest->data + (offset * src->element_size);
		memcpy((void*)ptr_dest, src->data, src->count * src->element_size);
		return (YEOK);
	}
	ptr_dest = (uintptr_t)dest->data + ((offset + src->count) * src->element_size);
	ptr_src = (uintptr_t)dest->data + (offset * src->element_size);
	memmove((void*)ptr_dest, (void*)ptr_src, src->count * src->element_size);
	memcpy((void*)ptr_src, src->data, src->count * src->element_size);
	dest->count += src->count;
	return (YEOK);
}

yerr_t yarray_ninject(yarray_t *dest, size_t offset, yarray_t *src, size_t start, size_t length) {
	uintptr_t ptr_dest, ptr_src;

	if (!dest || !src || (start + length) > src->count)
		return (YEBADPARAM);
	if (src->element_size != dest->element_size)
		return (YEINCOMPATIBLE);
	offset = (offset > dest->count) ? dest->count : offset;
	length = !length ? (src->count - start) : length;
	size_t total = dest->count + length;
	if (dest->size < total) {
		yerr_t err = yarray_resize(dest, total);
		if (err != YEOK)
			return (err);
	}
	if (offset == dest->count) {
		ptr_dest = (uintptr_t)dest->data + (offset * src->element_size);
		ptr_src = (uintptr_t)src->data + (start * src->element_size);
		memcpy((void*)ptr_dest, (void*)ptr_src, length * src->element_size);
		return (YEOK);
	}
	ptr_dest = (uintptr_t)dest->data + ((offset + length) * src->element_size);
	ptr_src = (uintptr_t)dest->data + (offset * src->element_size);
	memmove((void*)ptr_dest, (void*)ptr_src, length * src->element_size);
	ptr_dest = ptr_src;
	ptr_src = (uintptr_t)src->data + (start * src->element_size);
	memcpy((void*)ptr_dest, (void*)ptr_src, length * src->element_size);
	dest->count += length;
	return (YEOK);
}

yerr_t yarray_append(yarray_t *dest, yarray_t *src) {
	return (yarray_inject(dest, src, dest->count));
}

yerr_t yarray_prepend(yarray_t *dest, yarray_t *src) {
	return (yarray_inject(dest, src, 0));
}

void yarray_uniq(yarray_t *a) {
	if (!a)
		return;
	for (size_t i = 0; i < a->count; ++i) {
		for (size_t j = i + 1; j < a->count; ++j) {
			uintptr_t ptr1 = (uintptr_t)a->data + (i * a->element_size);
			uintptr_t ptr2 = (uintptr_t)a->data + (j * a->element_size);
			if (memcmp((void*)ptr1, (void*)ptr2, a->element_size))
				continue;
			uintptr_t ptr_dest = (uintptr_t)a->data + (j * a->element_size);
			uintptr_t ptr_src = (uintptr_t)a->data + ((j + 1) * a->element_size);
			memmove((void*)ptr_dest, (void*)ptr_src, (a->count - j - 1) * a->element_size);
			a->count--;
		}
	}
}

void yarray_sort(yarray_t *a, int (*f)(const void*, const void*, void*), void *user_data) {
	if (!a)
		return;
	qsort_r(a->data, a->count, a->element_size, f, user_data);
}

size_t yarray_search(yarray_t *a, void *e, int (*f)(const void*, const void*)) {
	size_t o_start, o_end, o_pivot;
	int cmp_res;

	if (!a || !f)
		return (-1);
	o_start = 0;
	o_end = a->count - 1;
	for (; ; ) {
		o_pivot = (o_end + o_start) / 2;
		uintptr_t ptr = (uintptr_t)a->data + (o_pivot * a->element_size);
		if (!(cmp_res = f(e, (void*)ptr)))
			return (o_pivot);
		if (cmp_res < 0) {
			if (o_pivot == (o_start + 1)) {
				ptr = (uintptr_t)a->data + (o_start * a->element_size);
				if (!f(e, (void*)ptr))
					return (o_start);
				else
					return (-1);
			}
			o_end = o_pivot;
		} else {
			if (o_pivot == (o_end - 1)) {
				ptr = (uintptr_t)a->data + (o_end * a->element_size);
				if (!f(e, (void*)ptr))
					return (o_end);
				else
					return (-1);
			}
			o_start = o_pivot;
		}
	}
}

void yarray_foreach(yarray_t *a, yarray_function_t func, void *user_data) {
	for (size_t offset = 0; offset < a->count; ++offset) {
		uintptr_t ptr = (uintptr_t)a->data + (offset * a->element_size);
		func(offset, (void*)ptr, user_data);
	}
}
