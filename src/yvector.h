/* Process this file with the HeaderBrowser tool (http://www.headerbrowser.org)
   to create documentation. */
/**
 * @header	yvector.h
 * @abstract	All definitions about yvectors.
 * @discussion	Vectors are bufferised arrays of pointers.
 * @version	1.0 Dec 28 2016
 * @author	Amaury Bouchard <amaury@amaury.net>
 */
#pragma once

#include <stddef.h>
#include "yerror.h"

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif /* __cplusplus || c_plusplus */

/**
 * @typedef	yvector_t
 *		Vector object.
 * @field	size	Size of the allocated buffer.
 * @field	count	Used size of the buffer.
 * @field	data	Buffer.
 */
typedef struct yvector_s {
	size_t size;
	size_t count;
	void **data;
} yvector_t;

/**
 * @typedef	yvector_function_t
 *		Function pointer, used to apply a procedure to an element.
 * @param	index		Index of the element in the vector.
 * @param	data		Pointer to the data.
 * @param	user_data	Pointer to some user data.
 */
typedef void (*yvector_function_t)(size_t index, void *data, void *user_data);

/**
 * @function	yvector_init
 *		Initialize a yvector.
 * @param	v	Pointer to the yvector.
 * @return	The initialized yvector.
 */
yvector_t *yvector_init(yvector_t *v);

/**
 * @function	yvector_new
 *		Create a yvector and initialize it.
 * @return	The created yvector.
 */
yvector_t *yvector_new(void);

/**
 * @function	yvector_clear
 *		Clear a yvector from its content.
 *		The vector shouldn't be used after that, or only after a new call to
 *		yvector_init().
 * @param	v		Pointer to the yvector.
 * @param	f		Pointer to a function that will be called to delete each
 *				element of the yvector. Could be NULL; otherwise, must
 *				be of type type: void f(void *elem, void *data);
 * @param	user_data	Pointer to data that would be given to the delete
 *				callback (see previous parameter). Could be NULL.
 */
void yvector_clear(yvector_t *v, void (*f)(void*, void*), void *user_data);

/**
 * @function	yvector_free
 *		Delete an existing yvector.
 * @param	v		A pointer to the yvector.
 * @param	f		Pointer to a function that is called to delete each
 *				element of the vector. Could be NULL ; otherwise, must
 *				have this prototype : void f(void *elem, void *data);
 * @param	user_data	Pointer to data that could be given to the delete
 *				callback (see previous parameter). Could be NULL.
 */
void yvector_free(yvector_t *v, void (*f)(void*, void*), void *user_data);

/**
 * @function	yvector_duplicate
 *		Duplicate a yvector.
 * @param	v	The yvector.
 * @result	The new yvector.
 */
yvector_t *yvector_duplicate(yvector_t *v);

/**
 * @function	yvector_truncate
 *		Truncate an existing yvector. The allocated memory doesn't
 *		change.
 * @param	v		The yvector.
 * @param	f		Pointer to a function that is called to delete each
 *				element of the vector. Could be NULL ; otherwise, must
 *				have this prototype : void f(void *elem, void *data);
 * @param	user_data	Pointer to data that could be given to the delete
 *				callback (see previous parameter). Could be NULL.
 */
void yvector_truncate(yvector_t *v, void (*f)(void*, void*), void *user_data);

/**
 * @function	yvector_resize
 *		Set the minimum size of a yvector.
 * @param	v	A pointer to the yvector.
 * @param	size	The minimum size for this yvector.
 *			Must be larger than the used size.
 * @return	YEOK if no error.
 */
yerr_t yvector_resize(yvector_t *v, size_t size);

/**
 * @function	yvector_shrink
 *		Shrink a yvector to fit its content.
 * @param	v	A pointer to the yvector.
 * @return	YEOK if no error.
 */
yerr_t yvector_shrink(yvector_t *v);

/**
 * @function	yvector_count
 *		Return the elements' count of a yvector (its used size).
 * @param	v	Pointer to the yvector.
 * @return	The number of elements in the yvector.
 */
size_t yvector_count(yvector_t *v);

/**
 * @function	yvector_add
 *		Add an element at the beginning of a yvector.
 * @param	v	A pointer to the yvector.
 * @param	e	A pointer to the element.
 * @return	YEOK if no error.
 */
yerr_t yvector_add(yvector_t *v, void *e);

/**
 * @function	yvector_push
 *		Add an element at the end of a yvector.
 * @param	v	A pointer to the yvector.
 * @param	e	A pointer to the element.
 * @return	YEOK if no error.
 */
yerr_t yvector_push(yvector_t *v, void *e);

/**
 * @function	yvector_insert
 *		Insert an element at the given offset of a yvector. The offset must
 *		be less than the current number of elements in the vector; otherwise
 *		the element will be added at the end of the vector. All elements
 *		placed at this offset and after are shifted to the right.
 * @param	v	A pointer to the yvector.
 * @param	e	A pointer to the element.
 * @param	offset	Offset of the element in the vector. Must be less or
 *			equal to the vector's elements count.
 * @return	YEOK if no error.
 */
yerr_t yvector_insert(yvector_t *v, void *e, size_t offset);

/**
 * @function	yvector_get
 *		Return the element at the given offset.
 * @param	v	Pointer to the yvector.
 * @param	offset	Offset of the element.
 * @return	A pointer to the element.
 */
void *yvector_get(yvector_t *v, size_t offset);

/**
 * @function	yvector_shift
 *		Remove the first element of a yvector and return it.
 * @param	v	A pointer to the yvector.
 * @return	A pointer to the removed element.
 */
void *yvector_shift(yvector_t *v);

/**
 * @function	yvector_pop
 *		Remove the last element of a yvector and return it.
 * @param	v	A pointer to the yvector.
 * @return	A pointer to the removed element.
 */
void *yvector_pop(yvector_t *v);

/*!
 * @function	yvector_extract
 *		Extract the element placed at the given offset of a yvector.
 *		All elements placed after the offset are shifted to the left.
 * @param	v	Pointer to the yvector.
 * @param	offset	Offset of the element in the vector. Must be less than
 *			the vector's used size.
 * @return	A pointer to the removed element.
 */
void *yvector_extract(yvector_t *v, size_t offset);

/**
 * @function	yvector_slice
 *		Create a yvector from a slice of an existing yvector. The source is
 *		unchanged.
 * @param	v	Pointer to the source yvector.
 * @param	offset	Offset of the beginning of the slice.
 * @param	length	Length of the slice. Go to the end of the vector if set to 0.
 * @return	The created yvector.
 */
yvector_t *yvector_slice(yvector_t *v, size_t offset, size_t length);

/**
 * @function	yvector_splice
 *		Remove a slice of a yvector and return it as a new yvector.
 * @param	v	Pointer to the soure yvector.
 * @param	offset	Offset of the beginning of the slice.
 * @param	length	Length of the slice. Go to the end of the vector if set to 0.
 * @return	The created yvector.
 */
yvector_t *yvector_splice(yvector_t *v, size_t offset, size_t length);

/**
 * @function	yvector_inject
 *		Inject the content of a yvector inside another one at the given offset.
 * @param	dest	Pointer to the destination yvector.
 * @param	src	Pointer to the source yvector.
 * @param	offset	Offset of the injection.
 * @return	YEOK if no error.
 */
yerr_t yvector_inject(yvector_t *dest, yvector_t *src, size_t offset);

/**
 * @function	yvector_ninject
 *		Inject a given number of elements of a yvector inside another one at
 *		the given offset.
 * @param	dest	Pointer to the destination yvector.
 * @param	offset	Offset of the injection.
 * @param	src	Pointer to the source yvector.
 * @param	start	Offset of the starting source slice.
 * @param	length	Length of the source slice. 0 to go to the end of the vector.
 * @return	YEOK if no error.
 */
yerr_t yvector_ninject(yvector_t *dest, size_t offset, yvector_t *src, size_t start, size_t length);

/**
 * @function	yvector_append
 *		Concatenate a yvector at the end of another one.
 * @param	dest	A pointer to the yvector to extend.
 * @param	src	A pointer to the source yvector.
 * @return	YEOK if no error.
 */
yerr_t yvector_append(yvector_t *dest, yvector_t *src);

/**
 * @function	yvector_prepend
 *		Concatenate a yvector at the beginning of another one.
 * @param	dest	Pointer to the yvector to extend.
 * @param	src	Pointer to the source yvector.
 * @return	YEOK if no error.
 */
yerr_t yvector_prepend(yvector_t *dest, yvector_t *src);

/**
 * @function	yvector_uniq
 *		Remove all values of a yvector to let only one entry of
 *		each value.
 * @param	v	The yvector.
 */
void yvector_uniq(yvector_t *v);

/**
 * @function	yvector_sort
 *		Do a quick sort of all elements of a yvector. See qsort(3) and qsort_r(3).
 * @param	v		Pointer to the yvector.
 * @param	f		A pointer to the function used to compare elements.
 * @param	user_data	A pointer to some data that will be given to the compare function as its third parameter.
 */
void yvector_sort(yvector_t *v, int (*f)(const void*, const void*, void*), void *user_data);

/**
 * @function	yvector_search
 *		Search the offset of an element in a yvector. WARNING: the
 *		yvector must be sorted (using yv_sort()) because this
 *		function uses dichotomy.
 * @param	v	Pointer to the yvector.
 * @param	e	The element to compare.
 * @param	f	A pointer to the function used to compare elements.
 * @return	The offset of the element in the yvector, or (-1) if the
 *		element can't be found.
 */
size_t yvector_search(yvector_t *v, void *e, int (*f)(const void*, const void*));

/**
 * @function	yv_foreach
 *		Apply a function on every elements of a vector.
 * @param	v		Pointer to the yvector.
 * @param	func		Pointer to the executed function.
 * @param	user_data	Pointer to some user data.
 */
void yvector_foreach(yvector_t *v, yvector_function_t func, void *user_data);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif /* __cplusplus || c_plusplus */
