/* Process this file with the HeaderBrowser tool (http://www.headerbrowser.org)
   to create documentation. */
/**
 * @header	ytarray.h
 * @abstract	All definitions about typed arrays.
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
	yarray_##__type##_t *yarray_##__type##_new();										\
	/* clear */	\
	void yarray_##__type##_clear(yarray_##__type##_t *a, yarray_##__type##_function_t f, void *user_data);			\
	/* free */	\
	void yarray_##__type##_free(yarray_##__type ##_t *a, yarray_##__type##_function_t f, void *user_data);			\
	/* duplicate */	\
	yarray_##__type##_t *yarray_##__type##_duplicate(yarray_##__type##_t *a);						\
	/* truncate */	\
	void yarray_##__type##_truncate(yarray_##__type##_t *a, yarray_##__type##_function_t f, void *user_data);		\
	/* resize */	\
	bool yarray_##__type##_resize(yarray_##__type##_t *a, size_t size);							\
	/* shrink */	\
	bool yarray_##__type##_shrink(yarray_##__type ##_t *a);									\
	/* count */	\
	size_t yarray_##__type##_count(yarray_##__type##_t *a);									\
	/* add */	\
	bool yarray_##__type##_add(yarray_##__type##_t *a, __type e);								\
	/* push */	\
	bool yarray_##__type##_push(yarray_##__type##_t *a, __type e);								\
	/* insert */	\
	bool yarray_##__type##_insert(yarray_##__type##_t *a, __type e, size_t offset);						\
	/* get */	\
	void *yarray_##__type##_get(yarray_##__type##_t *a, size_t offset);							\
	/* shift */	\
	void yarray_##__type##_shift(yarray_##__type##_t *a);									\
	/* pop */	\
	void yarray_##__type##_pop(yarray_##__type##_t *a);									\
	/* extract */	\
	void yarray_##__type##_extract(yarray_##__type##_t *a, size_t offset);							\
	/* slice */	\
	yarray_##__type##_t *yarray_##__type##_slice(yarray_##__type##_t *a, size_t offset, size_t length);			\
	/* splice */	\
	yarray_##__type##_t *yarray_##__type##_splice(yarray_##__type##_t *a, size_t offset, size_t length);			\
	/* inject */	\
	bool yarray_##__type##_inject(yarray_##__type##_t *dest, yarray_##__type##_t *src, size_t offset);			\
	/* ninject */	\
	bool yarray_##__type##_ninject(yarray_##__type##_t *dest, size_t offset, yarray_##__type##_t *src, size_t start, size_t length);	\
	/* append */	\
	bool yarray_##__type##_append(yarray_##__type##_t *dest, yarray_##__type##_t *src);					\
	/* prepend */	\
	bool yarray_##__type##_prepend(yarray_##__type##_t *dest, yarray_##__type##_t *src);					\
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
	/* _resize */	\
	bool yarray_##__type##_resize(yarray_##__type##_t *a, size_t size) {								\
		void *new_data;														\
		if (!a || !size || a->count > size ||											\
		    !(new_data = yrealloc(a->data, _YARRAY_ROUND_SIZE(size) * sizeof(__type))))						\
			return (false);													\
		a->data = new_data;													\
		a->size = size; 													\
		return (true);														\
	}																\
	/* count */	\
	size_t yarray_##__type##_count(yarray_##__type##_t *a) {									\
		if (!a)															\
			return (0);													\
		return (a->count);													\
	}																\
	/* push */	\
	bool yarray_##__type##_push(yarray_##__type##_t *a, __type e) {									\
		if (!a || (a->count == a->size && !yarray_##__type##_resize(a, (a->size * 2))))						\
			return (false);													\
		a->data[a->count] = e;													\
		a->count++;														\
		return (true);														\
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
