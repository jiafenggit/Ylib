/* Process this file with the HeaderBrowser tool (http://www.headerbrowser.org)
   to create documentation. */
/**
 * @header	yarray.h
 * @abstract	All definitions about yarrays.
 * @discussion	Vectors are bufferised arrays of data.
 * @version	1.0 Jan 02 2017
 * @author	Amaury Bouchard <amaury@amaury.net>
 */
#pragma once

#include <stddef.h>
#include <stdbool.h>

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif /* __cplusplus || c_plusplus */

/* ************ PRIVATE DEFINITIONS AND MACROS ************ */
/** @define _YARRAY_DEFAULT_SIZE Default size of yarrays. */
#define _YARRAY_DEFAULT_SIZE	8
/** @define _YARRAY_ROUND_SIZE Round the size to the next power of 2. */
#define _YARRAY_ROUND_SIZE(s)	((size_t)pow(2, ceil(log(s) / log(2))))

/**
 * @typedef	yarray_t
 *		Array object.
 * @field	size		Size of the allocated buffer.
 * @field	count		Used size of the buffer.
 * @field	element_size	Size of each element in the array.
 * @field	data		Buffer.
 */
typedef struct yarray_s {
	size_t size;
	size_t count;
	size_t element_size;
	void **data;
} yarray_t;

/**
 * @typedef	yarray_function_t
 *		Function pointer, used to apply a procedure to an element.
 * @param	index		Index of the element in the array.
 * @param	data		Pointer to the data.
 * @param	user_data	Pointer to some user data.
 */
typedef void (*yarray_function_t)(size_t index, void *data, void *user_data);

/**
 * @function	yarray_init
 *		Initialize a yarray.
 * @param	a		Pointer to the yarray.
 * @param	element_size	Size of each elements in the array.
 * @return	The initialized yarray.
 */
yarray_t *yarray_init(yarray_t *a, size_t element_size);

/**
 * @function	yarray_new
 *		Create a yarray and initialize it.
 * @param	element_size	Size of each elements in the array.
 * @return	The created yarray.
 */
yarray_t *yarray_new(size_t element_size);

/**
 * @function	yarray_clear
 *		Clear a yarray from its content.
 *		The array shouldn't be used after that, or only after a new call to
 *		yarray_init().
 * @param	a		Pointer to the yarray.
 * @param	f		Pointer to a function that will be called to delete each
 *				element of the yarray. Could be NULL; otherwise, must
 *				be of type type: void f(void *elem, void *data);
 * @param	user_data	Pointer to data that would be given to the delete
 *				callback (see previous parameter). Could be NULL.
 */
void yarray_clear(yarray_t *a, void (*f)(void*, void*), void *user_data);

/**
 * @function	yarray_free
 *		Delete an existing yarray.
 * @param	a		A pointer to the yarray.
 * @param	f		Pointer to a function that is called to delete each
 *				element of the array. Could be NULL ; otherwise, must
 *				have this prototype : void f(void *elem, void *data);
 * @param	user_data	Pointer to data that could be given to the delete
 *				callback (see previous parameter). Could be NULL.
 */
void yarray_free(yarray_t *a, void (*f)(void*, void*), void *user_data);

/**
 * @function	yarray_duplicate
 *		Duplicate a yarray.
 * @param	a	The yarray.
 * @result	The new yarray.
 */
yarray_t *yarray_duplicate(yarray_t *a);

/**
 * @function	yarray_truncate
 *		Truncate an existing yarray. The allocated memory doesn't
 *		change.
 * @param	a		The yarray.
 * @param	f		Pointer to a function that is called to delete each
 *				element of the array. Could be NULL ; otherwise, must
 *				have this prototype : void f(void *elem, void *data);
 * @param	user_data	Pointer to data that could be given to the delete
 *				callback (see previous parameter). Could be NULL.
 */
void yarray_truncate(yarray_t *a, void (*f)(void*, void*), void *user_data);

/**
 * @function	yarra_resize
 *		Set the minimum size of a yarray.
 * @param	a	A pointer to the yarray.
 * @param	size	The minimum size for this yarray.
 *			Must be larger than the used size.
 * @return	false if an error occurs, true otherwise.
 */
bool yarray_resize(yarray_t *a, size_t size);

/**
 * @function	yarray_shrink
 *		Shrink a yarray to fit its content.
 * @param	a	A pointer to the yarray.
 * @return	false if an error occurs, true otherwise.
 */
bool yarray_shrink(yarray_t *a);

/**
 * @function	yarray_count
 *		Return the elements' count of a yarray (its used size).
 * @param	a	Pointer to the yarray.
 * @return	The number of elements in the yarray.
 */
size_t yarray_count(yarray_t *a);

/**
 * @function	yarray_add
 *		Add an element at the beginning of a yarray.
 * @param	a	A pointer to the yarray.
 * @param	e	A pointer to the element.
 * @return	false if an error occurs, true otherwise.
 */
bool yarray_add(yarray_t *a, void *e);

/**
 * @function	yarray_push
 *		Add an element at the end of a yarray.
 * @param	a	A pointer to the yarray.
 * @param	e	A pointer to the element.
 * @return	false if an error occurs, true otherwise.
 */
bool yarray_push(yarray_t *v, void *e);

/**
 * @function	yarray_insert
 *		Insert an element at the given offset of a yarray. The offset must
 *		be less than the current number of elements in the array; otherwise
 *		the element will be added at the end of the array. All elements
 *		placed at this offset and after are shifted to the right.
 * @param	a	A pointer to the yarray.
 * @param	e	A pointer to the element.
 * @param	offset	Offset of the element in the array. Must be less or
 *			equal to the array's elements count.
 * @return	false if an error occurs, true otherwise.
 */
bool yarray_insert(yarray_t *a, void *e, size_t offset);

/**
 * @function	yarray_get
 *		Return the element at the given offset.
 * @param	a	Pointer to the yarray.
 * @param	offset	Offset of the element.
 * @return	A pointer to the element.
 */
void *yarray_get(yarray_t *a, size_t offset);

/**
 * @function	yarray_shift
 *		Remove the first element of a yarray.
 * @param	a	A pointer to the yarray.
 */
void yarray_shift(yarray_t *a);

/**
 * @function	yarray_pop
 *		Remove the last element of a yarray.
 * @param	a	A pointer to the yarray.
 */
void yarray_pop(yarray_t *a);

/*!
 * @function	yarray_extract
 *		Extract the element placed at the given offset of a yarray.
 *		All elements placed after the offset are shifted to the left.
 * @param	a	Pointer to the yarray.
 * @param	offset	Offset of the element in the array. Must be less than
 *			the array's used size.
 */
void yarray_extract(yarray_t *a, size_t offset);

/**
 * @function	yarray_slice
 *		Create a yarray from a slice of an existing yarray. The source is
 *		unchanged.
 * @param	a	Pointer to the source yarray.
 * @param	offset	Offset of the beginning of the slice.
 * @param	length	Length of the slice. Go to the end of the array if set to 0.
 * @return	The created yarray.
 */
yarray_t *yarray_slice(yarray_t *a, size_t offset, size_t length);

/**
 * @function	yarray_splice
 *		Remove a slice of a yarray and return it as a new yarray.
 * @param	a	Pointer to the soure yarray.
 * @param	offset	Offset of the beginning of the slice.
 * @param	length	Length of the slice. Go to the end of the array if set to 0.
 * @return	The created yarray.
 */
yarray_t *yarray_splice(yarray_t *a, size_t offset, size_t length);

/**
 * @function	yarray_inject
 *		Inject the content of a yarray inside another one at the given offset.
 * @param	dest	Pointer to the destination yarray.
 * @param	src	Pointer to the source yarray.
 * @param	offset	Offset of the injection.
 * @return	false if an error occurs, true otherwise.
 */
bool yarray_inject(yarray_t *dest, yarray_t *src, size_t offset);

/**
 * @function	yarray_ninject
 *		Inject a given number of elements of a yarray inside another one at
 *		the given offset.
 * @param	dest	Pointer to the destination yarray.
 * @param	offset	Offset of the injection.
 * @param	src	Pointer to the source yarray.
 * @param	start	Offset of the starting source slice.
 * @param	length	Length of the source slice. 0 to go to the end of the array.
 * @return	false if an error occurs, true otherwise.
 */
bool yarray_ninject(yarray_t *dest, size_t offset, yarray_t *src, size_t start, size_t length);

/**
 * @function	yarray_append
 *		Concatenate a yarray at the end of another one.
 * @param	dest	A pointer to the yarray to extend.
 * @param	src	A pointer to the source yarray.
 * @return	false if an error occurs, true otherwise.
 */
bool yarray_append(yarray_t *dest, yarray_t *src);

/**
 * @function	yarray_prepend
 *		Concatenate a yarray at the beginning of another one.
 * @param	dest	Pointer to the yarray to extend.
 * @param	src	Pointer to the source yarray.
 * @return	false if an error occurs, true otherwise.
 */
bool yarray_prepend(yarray_t *dest, yarray_t *src);

/**
 * @function	yarray_uniq
 *		Remove all values of a yarray to let only one entry of
 *		each value.
 * @param	a	The yarray.
 */
void yarray_uniq(yarray_t *a);

/**
 * @function	yarray_sort
 *		Do a quick sort of all elements of a yarray. See qsort(3) and qsort_r(3).
 * @param	a		Pointer to the yarray.
 * @param	f		A pointer to the function used to compare elements.
 * @param	user_data	A pointer to some data that will be given to the compare function as its third parameter.
 */
void yarray_sort(yarray_t *a, int (*f)(const void*, const void*, void*), void *user_data);

/**
 * @function	yarray_search
 *		Search the offset of an element in a yarray. WARNING: the
 *		yarray must be sorted (using yv_sort()) because this
 *		function uses dichotomy.
 * @param	a	Pointer to the yarray.
 * @param	e	The element to compare.
 * @param	f	A pointer to the function used to compare elements.
 * @return	The offset of the element in the yarray, or (-1) if the
 *		element can't be found.
 */
size_t yarray_search(yarray_t *a, void *e, int (*f)(const void*, const void*));

/**
 * @function	yv_foreach
 *		Apply a function on every elements of a array.
 * @param	a		Pointer to the yarray.
 * @param	func		Pointer to the executed function.
 * @param	user_data	Pointer to some user data.
 */
void yarray_foreach(yarray_t *a, yarray_function_t func, void *user_data);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif /* __cplusplus || c_plusplus */
