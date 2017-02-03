/* Process this file with the HeaderBrowser tool (http://www.headerbrowser.org)
   to create documentation. */
/**
 * @header	ytarray.h
 * @abstract	All definitions about typed arrays.
 * @discussion	
 * @discussion	Vectors are bufferised arrays of data.
 * @version	1.0 Jan 29 2017
 * @author	Amaury Bouchard <amaury@amaury.net>
 */
#pragma once

#include <stddef.h>
#include "yarray.h"

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif /* __cplusplus || c_plusplus */

#define YTARRAY_DEFINE(__type)													\
	typedef struct {													\
		size_t	size;													\
		size_t	count;													\
		__type	*data;													\
	} yarray_##__type##_t;													\
																\
	typedef void (*yarray_##__type##_function_t)(size_t index, __type data, void *user_data);				\
																\
	/* init */	\
	yarray_##__type##_t *yarray_##__type##_init(yarray_##__type##_t *a);							\
	/* new */	\
	yarray_##__type##_t *yarray_##__type##_new(void);									\
	/* clear */	\
	void yarray_##__type##_clear(yarray_##__type##_t *a, yarray_##__type##_function_t f, void *user_data);			\
	/* free */	\
	void yarray_##__type##_free(yarray_##__type ##_t *a, yarray_##__type##_function_t f, void *user_data);			\
	/* duplicate */	\
	yarray_##__type##_t *yarray_##__type##_duplicate(yarray_##__type##_t *a);						\
	/* truncate */	\
	void yarray_##__type##_truncate(yarray_##__type##_t *a, yarray_##__type##_function_t f, void *user_data);		\
	/* resize */	\
	yerr_t yarray_##__type##_resize(yarray_##__type##_t *a, size_t size);							\
	/* shrink */	\
	yerr_t yarray_##__type##_shrink(yarray_##__type##_t *a);								\
	/* revert */	\
	yerr_t yarray_##__type##_revert(yarray_##__type##_t *a);								\
	/* count */	\
	size_t yarray_##__type##_count(yarray_##__type##_t *a);									\
	/* add */	\
	yerr_t yarray_##__type##_add(yarray_##__type##_t *a, __type e);								\
	/* push */	\
	yerr_t yarray_##__type##_push(yarray_##__type##_t *a, __type e);							\
	/* insert */	\
	yerr_t yarray_##__type##_insert(yarray_##__type##_t *a, __type e, size_t offset);					\
	/* get */	\
	__type yarray_##__type##_get(yarray_##__type##_t *a, size_t offset);							\
	/* shift */	\
	__type yarray_##__type##_shift(yarray_##__type##_t *a);									\
	/* pop */	\
	__type yarray_##__type##_pop(yarray_##__type##_t *a);									\
	/* extract */	\
	__type yarray_##__type##_extract(yarray_##__type##_t *a, size_t offset);						\
	/* slice */	\
	yarray_##__type##_t *yarray_##__type##_slice(yarray_##__type##_t *a, size_t offset, size_t length);			\
	/* splice */	\
	yarray_##__type##_t *yarray_##__type##_splice(yarray_##__type##_t *a, size_t offset, size_t length);			\
	/* inject */	\
	yerr_t yarray_##__type##_inject(yarray_##__type##_t *dest, yarray_##__type##_t *src, size_t offset);			\
	/* ninject */	\
	yerr_t yarray_##__type##_ninject(yarray_##__type##_t *dest, size_t offset, yarray_##__type##_t *src, size_t start, size_t length);	\
	/* append */	\
	yerr_t yarray_##__type##_append(yarray_##__type##_t *dest, yarray_##__type##_t *src);					\
	/* prepend */	\
	yerr_t yarray_##__type##_prepend(yarray_##__type##_t *dest, yarray_##__type##_t *src);					\
	/* uniq */	\
	void yarray_##__type##_uniq(yarray_##__type##_t *a);									\
	/* sort */	\
	void yarray_##__type##_sort(yarray_##__type##_t *a, int (*f)(const void*, const void*, void*), void *user_data);	\
	/* search */	\
	size_t yarray_##__type##_search(yarray_##__type##_t *a, void *e, int (*f)(const void*, const void*));			\
	/* foreach */	\
	void yarray_##__type##_foreach(yarray_##__type##_t *a, yarray_##__type##_function_t func, void *user_data);

#define YTARRAY_IMPLEMENT(__type)													\
	/* init */	\
	yarray_##__type##_t *yarray_##__type##_init(yarray_##__type##_t *a) {								\
		if (!a)															\
			return (NULL);													\
		*a = (yarray_##__type##_t){												\
			.size = _YARRAY_DEFAULT_SIZE,											\
			.count = 0,													\
			.data = ycalloc(_YARRAY_DEFAULT_SIZE, sizeof(__type))								\
		};															\
		if (!a->data)														\
			return (NULL);													\
		return (a);														\
	}																\
	/* new */	\
	yarray_##__type##_t *yarray_##__type##_new(void) {										\
		yarray_##__type##_t *a = ymalloc(sizeof(yarray_##__type##_t));								\
		if (!a)															\
			return (NULL);													\
		if (!yarray_##__type##_init(a)) {											\
			yfree(a);													\
			return (NULL);													\
		}															\
		return (a);														\
	}																\
	/* clear */	\
	void yarray_##__type##_clear(yarray_##__type##_t *a, yarray_##__type##_function_t f, void *user_data) {				\
		if (!a || !a->size)													\
			return;														\
		if (f)														\
			yarray_##__type##_foreach(a, f, user_data);									\
		yfree(a->data);														\
		a->size = a->count = 0;													\
	}																\
	/* free */	\
	void yarray_##__type##_free(yarray_##__type##_t *a, yarray_##__type##_function_t f, void *user_data) {				\
		if (!a)															\
			return;														\
		yarray_##__type##_clear(a, f, user_data);										\
		yfree(a);														\
	}																\
	/* duplicate */	\
	yarray_##__type##_t *yarray_##__type##_duplicate(yarray_##__type##_t *a) {							\
		return (yarray_##__type##_slice(a, 0, 0));										\
	}																\
	/* truncate */	\
	void yarray_##__type##_truncate(yarray_##__type##_t *a, yarray_##__type##_function_t f, void *user_data) {			\
		if (!a)															\
			return;														\
		if (f) {														\
			yarray_##__type##_foreach(a, f, user_data);									\
		}															\
		a->count = 0;														\
	}																\
	/* _resize */	\
	yerr_t yarray_##__type##_resize(yarray_##__type##_t *a, size_t size) {								\
		if (!a || !size || a->count > size)											\
			return (YEBADPARAM);												\
		__type *new_data;													\
		size_t new_size = _YARRAY_ROUND_SIZE(size);										\
		if (!(new_data = yrealloc(a->data, new_size * sizeof(__type))))								\
			return (YENOMEM);												\
		a->data = new_data;													\
		a->size = new_size; 													\
		return (YEOK);														\
	}																\
	/* shrink */	\
	yerr_t yarray_##__type##_shrink(yarray_##__type##_t *a) {									\
		if (!a)															\
			return (YEBADPARAM);												\
		if (!a->count)														\
			return (YEOK);													\
		__type *new_data;													\
		size_t new_size = _YARRAY_ROUND_SIZE(a->count);										\
		if(!(new_data = yrealloc(a->data, new_size * sizeof(__type))))								\
			return (false);													\
		a->data = new_data;													\
		a->size = new_size;													\
		return (true);														\
	}																\
	/* revert */	\
	yerr_t yarray_##__type##_revert(yarray_##__type##_t *a) {									\
		if (!a)															\
			return (YEBADPARAM);												\
		if (a->count <= 1)													\
			return (YEOK);													\
		void *new_data;														\
		size_t new_size = _YARRAY_ROUND_SIZE(a->count);										\
		if (!(new_data = ymalloc(new_size)))											\
			return (false); 												\
		for (size_t offset_src = a->count, offset_dest = 0; offset_src > 0; --offset_src, ++offset_dest) {			\
			new_data[offset_dest] = a->data[offset_src - 1];								\
		}															\
		yfree(a->data);														\
		a->data = new_data;													\
		a->size = new_size;													\
		return (true);														\
	}																\
	/* count */	\
	size_t yarray_##__type##_count(yarray_##__type##_t *a) {									\
		if (!a)															\
			return (0);													\
		return (a->count);													\
	}																\
	/* add */	\
	bool yarray_##__type##_add(yarray_##__type##_t *a, __type e) {									\
		if (!a || (a->count == a->size && !yarray_##__type##_resize(a, (a->size * 2))))						\
			return (false);													\
		uintptr_t ptr = (uintptr_t)a->data + sizeof(__type);									\
		memmove((void*)ptr, a->data, a->count * sizeof(__type));								\
		a->data[0] = e;														\
		a->count++;														\
		return (true);														\
	}																\
	/* push */	\
	bool yarray_##__type##_push(yarray_##__type##_t *a, __type e) {									\
		if (!a || (a->count == a->size && !yarray_##__type##_resize(a, (a->size * 2))))						\
			return (false);													\
		a->data[a->count] = e;													\
		a->count++;														\
		return (true);														\
	}																\
	/* insert */	\
	bool yarray_##__type##_insert(yarray_##__type##_t *a, __type e, size_t offset) {						\
		if (!a || (a->count == a->size && !yarray_##__type##_resize(a, (a->size * 2))))						\
			return (false);													\
		if (offset >= a->count)													\
			return (yarray_##__type##_push(a, e));										\
		uintptr_t ptr_dest = (uintptr_t)a->data + ((offset + 1) * sizeof(__type));						\
		uintptr_t ptr_src = (uintptr_t)a->data + (offset * sizeof(__type));							\
		memmove((void*)ptr_dest, (void*)ptr_src, (a->count - offset) * sizeof(__type));						\
		a->data[offset] = e;													\
		return (true);														\
	}																\
	/* get */	\
	__type yarray_##__type##_get(yarray_##__type##_t *a, size_t offset) {								\
		if (!a || offset >= a->count)												\
			return ((__type){0});													\
		return (a->data[offset]);												\
	}																\
	/* shift */	\
	__type yarray_##__type##_shift(yarray_##__type##_t *a) {									\
		if (!a || !a->count)													\
			return ((__type){0});												\
		__type e = a->data[0];													\
		uintptr_t ptr = (uintptr_t)a->data + sizeof(__type);									\
		memmove(a->data, (void*)ptr, (a->count - 1) * sizeof(__type));								\
		a->count--;														\
		return (e);														\
	}																\
	/* pop */	\
	__type yarray_##__type##_pop(yarray_##__type##_t *a) {										\
		if (!a || !a->count)													\
			return ((__type){0});												\
		a->count--;														\
		return (a->data[a->count]);												\
	}																\
	/* extract */	\
	__type yarray_##__type##_extract(yarray_##__type##_t *a, size_t offset) {							\
		if (!a || offset >= a->count)												\
			return ((__type){0});												\
		__type e = a->data[offset];												\
		uintptr_t ptr_src = (uintptr_t)a->data + ((offset + 1) * sizeof(__type));						\
		uintptr_t ptr_dest = (uintptr_t)a->data + (offset * sizeof(__type));							\
		memmove((void*)ptr_dest, (void*)ptr_src, (a->count - offset) * sizeof(__type));						\
		a->count--;														\
		return (e);														\
	}																\
	/* slice */	\
	yarray_##__type##_t *yarray_##__type##_slice(yarray_##__type##_t *a, size_t offset, size_t length) {				\
		if (!a || offset >= a->count)												\
			return (NULL);													\
		if (!length || (offset + length) > a->count)										\
			length = a->count - offset;											\
		yarray_##__type##_t *nv = ymalloc(sizeof(yarray_##__type##_t));								\
		if (!nv)														\
			return (NULL);													\
		size_t size = _YARRAY_ROUND_SIZE(length);										\
		*nv = (yarray_##__type##_t){												\
			.size = size,													\
			.count = length,												\
			.data = ymalloc(size * sizeof(__type))										\
		};															\
		if (!nv->data) {													\
			yfree(nv);													\
			return (NULL);													\
		}															\
		uintptr_t ptr = (uintptr_t)a->data + (offset * sizeof(__type));								\
		memcpy(nv->data, (void*)ptr, length);											\
		return (nv);														\
	}																\
	/* splice */	\
	yarray_##__type##_t *yarray_##__type##_splice(yarray_##__type##_t *a, size_t offset, size_t length) {				\
		if (!a || offset >= a->count)												\
			return (NULL);													\
		if (!length || (offset + length) > a-> count)										\
			length = a->count - offset;											\
		yarray_##__type##_t *nv = yarray_##__type##_slice(a, offset, length);							\
		if (!nv)														\
			return (NULL);													\
		uintptr_t ptr_dest = (uintptr_t)a->data + (offset * sizeof(__type));							\
		uintptr_t ptr_src = (uintptr_t)a->data + ((offset + length) * sizeof(__type));						\
		memmove((void*)ptr_dest, (void*)ptr_src, (a->count - offset - length) * sizeof(__type));				\
		a->count -= length;													\
		return (nv);														\
	}																\
	/* inject */	\
	bool yarray_##__type##_inject(yarray_##__type##_t *dest, yarray_##__type##_t *src, size_t offset) {				\
		uintptr_t ptr_dest, ptr_src;												\
		if (!dest || !src)													\
			return (false);													\
		offset = (offset > dest->count) ? dest->count : offset;									\
		size_t total = dest->count + src->count;										\
		if (dest->size < total && !yarray_##__type##_resize(dest, total))							\
			return (false);													\
		if (offset == dest->count) {												\
			ptr_dest = (uintptr_t)dest->data + (offset * sizeof(__type));							\
			memcpy((void*)ptr_dest, src->data, src->count * sizeof(__type));						\
			return (true);													\
		}															\
		ptr_dest = (uintptr_t)dest->data + ((offset + src->count) * sizeof(__type));						\
		ptr_src = (uintptr_t)dest->data + (offset * sizeof(__type));								\
		memmove((void*)ptr_dest, (void*)ptr_src, src->count * sizeof(__type));							\
		memcpy((void*)ptr_src, src->data, src->count * sizeof(__type));								\
		dest->count += src->count;												\
		return (true);														\
	}																\
	/* ninject */	\
	bool yarray_##__type##_ninject(yarray_##__type##_t *dest, size_t offset, yarray_##__type##_t *src, size_t start, size_t length) {	\
		uintptr_t ptr_dest, ptr_src;												\
		if (!dest || !src || (start + length) > src->count)									\
			return (false);													\
		offset = (offset > dest->count) ? dest->count : offset;									\
		length = !length ? (src->count - start) : length;									\
		size_t total = dest->count + length;											\
		if (dest->size < total && !yarray_##__type##_resize(dest, total))							\
			return (false);													\
		if (offset == dest->count) {												\
			ptr_dest = (uintptr_t)dest->data + (offset * sizeof(__type));							\
			ptr_src = (uintptr_t)src->data + (start * sizeof(__type));							\
			memcpy((void*)ptr_dest, (void*)ptr_src, length * sizeof(__type));						\
			return (true);													\
		}															\
		ptr_dest = (uintptr_t)dest->data + ((offset + length) * sizeof(__type));						\
		ptr_src = (uintptr_t)dest->data + (offset * sizeof(__type));								\
		memmove((void*)ptr_dest, (void*)ptr_src, length * sizeof(__type));							\
		ptr_dest = ptr_src;													\
		ptr_src = (uintptr_t)src->data + (start * sizeof(__type));								\
		memcpy((void*)ptr_dest, (void*)ptr_src, length * sizeof(__type));							\
		dest->count += length;													\
		return (true);														\
	}																\
	/* append */	\
	bool yarray_##__type##_append(yarray_##__type##_t *dest, yarray_##__type##_t *src) {						\
		return (yarray_##__type##_inject(dest, src, dest->count));								\
	}																\
	/* prepend */	\
	bool yarray_##__type##_prepend(yarray_##__type##_t *dest, yarray_##__type##_t *src) {						\
		return (yarray_##__type##_inject(dest, src, 0));									\
	}																\
	/* uniq */	\
	void yarray_##__type##_uniq(yarray_##__type##_t *a) {										\
		if (!a)															\
			return;														\
		for (size_t i = 0; i < a->count; ++i) {											\
			for (size_t j = i + 1; j < a->count; ++j) {									\
				uintptr_t ptr1 = (uintptr_t)a->data + (i * sizeof(__type));						\
				uintptr_t ptr2 = (uintptr_t)a->data + (j * sizeof(__type));						\
				if (memcmp((void*)ptr1, (void*)ptr2, sizeof(__type)))							\
					continue;											\
				uintptr_t ptr_dest = (uintptr_t)a->data + (j * sizeof(__type));						\
				uintptr_t ptr_src = (uintptr_t)a->data + ((j + 1) * sizeof(__type));					\
				memmove((void*)ptr_dest, (void*)ptr_src, (a->count - j - 1) * sizeof(__type));				\
				a->count--;												\
			}														\
		}															\
	}																\
	/* sort */	\
	void yarray_##__type##_sort(yarray_##__type##_t *a, int (*f)(const void*, const void*, void*), void *user_data) {		\
		if (!a)															\
			return;														\
		qsort_r(a->data, a->count, sizeof(__type), f, user_data);								\
	}																\
	/* search */	\
	size_t yarray_##__type##_search(yarray_##__type##_t *a, void *e, int (*f)(const void*, const void*)) {				\
		size_t o_start, o_end, o_pivot;												\
		int cmp_res;														\
		if (!a || !f)														\
			return (-1);													\
		o_start = 0;														\
		o_end = a->count - 1;													\
		for (; ; ) {														\
			o_pivot = (o_end + o_start) / 2;										\
			uintptr_t ptr = (uintptr_t)a->data + (o_pivot * sizeof(__type));						\
			if (!(cmp_res = f(e, (void*)ptr)))										\
				return (o_pivot);											\
			if (cmp_res < 0) {												\
				if (o_pivot == (o_start + 1)) {										\
					ptr = (uintptr_t)a->data + (o_start * sizeof(__type));						\
					if (!f(e, (void*)ptr))										\
						return (o_start);									\
					else												\
						return (-1);										\
				}													\
				o_end = o_pivot;											\
			} else {													\
				if (o_pivot == (o_end - 1)) {										\
					ptr = (uintptr_t)a->data + (o_end * sizeof(__type));						\
					if (!f(e, (void*)ptr))										\
						return (o_end);										\
					else												\
						return (-1);										\
				}													\
				o_start = o_pivot;											\
			}														\
		}															\
	}																\
	/* foreach */	\
	void yarray_##__type##_foreach(yarray_##__type##_t *a, yarray_##__type##_function_t func, void *user_data) {	\
		for (size_t offset = 0; offset < a->count; ++offset) {									\
			func(offset, a->data[offset], user_data);									\
		}															\
	}

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif /* __cplusplus || c_plusplus */
