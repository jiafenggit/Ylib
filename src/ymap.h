/*!
 * @header	ymap.h
 * @abstract	All definitions about hash maps.
 * @discussion  Hash maps are data structures that uses hash function to map values.
 * @version     1.0 Aug 13 2012
 * @author      Amaury Bouchard <amaury@amaury.net>
 */
#ifndef __YMAP_H__
#define __YMAP_H__

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif /* __cplusplus || c_plusplus */

/** @define YHM_MAX_LOAD_FACTOR	Maximum load factor of a hash map before increasing it. */
#define YHM_MAX_LOAD_FACTOR	0.7

/** @define YHM_MIN_LOAD_FACTOR Minimum load factor of a hash map before reducing it. */
#define YHM_MIN_LOAD_FACTOR	0.25

/**
 * typedef	ymap_element_t
 *		Structure used to store a hash map's element.
 * @field	key		Element's key.
 * @field	data		Element's data.
 * @field	previous	Pointer to the previous element with the same hash value.
 * @field	next		Pointer to the next element with the same hash value.
 */
typedef struct ymap_element_s {
	char *key;
	void *data;
	struct ymap_element_s *previous;
	struct ymap_element_s *next;
} ymap_element_t;

/**
 * typedef	ymap_bucket_t
 *		Structure used to store the values associated with a hash value.
 * @field	nbr_elements	Elements count for this bucket.
 * @field	elements	Pointer to the first element.
 */
typedef struct ymap_bucket_s {
	size_t		nbr_elements;
	ymap_element_t	*elements;
} ymap_bucket_t;

/**
 * typedef	ymap_function_t
 *		Function pointer, used to apply a procedure to an element.
 * @param	key		Pointer to the key.
 * @param	data		Pointer to the data.
 * @param	user_data	Pointer to some user data.
 */
typedef void (*ymap_function_t)(char *key, void *data, void *user_data);

/**
 * @typedef	ymap_t
 * @field	size		Current size of the hash map.
 * @field	used		Current number of elements stored in the hash map.
 * @field	buckets		Array of buckets.
 * @field	destroy_func	Pointer to the function called when an element is removed.
 * @field	destroy_data	Pointer to some user data given to the destroy function.
 */
typedef struct ymap_s {
	size_t		size;
	size_t		used;
	ymap_bucket_t	*buckets;
	ymap_function_t	destroy_func;
	void		*destroy_data;
} ymap_t;

/**
 * @function	ymap_create
 *		Creates a new hash map.
 * @param	size		Initial size of the hash map.
 * @param	destroy_func	Pointer to the function called when an element is removed.
 * @param	destroy_data	Pointer to some user data given to the destroy function.
 * @return	The created hash map.
 */
ymap_t *ymap_create(size_t size, ymap_function_t destroy_func, void *destroy_data);

/**
 * @function	ymap_free
 *		Destroy a hash map.
 * @param	hash	Pointer to the hash map.
 */
void ymap_free(ymap_t *hashmap);

/**
 * @function	ymap_add
 *		Add an element to a hash map.
 * @param	hashmap	Pointer to the hash map.
 * @param	key	Key used to index the element.
 * @param	data	The element's data.
 */
void ymap_add(ymap_t *hashmap, char *key, void *data);

/**
 * @function	ymap_search
 *		Search an element in a hash map, and returns its value. Returns
 *		NULL if the elements doesn't exist (and if it exists and contains
 *		NULL as its value).
 * @param	hashmap	Pointer to the hash map.
 * @param	key	Key used to index the element.
 * @return	A pointer to the element's data.
 */
void *ymap_search(ymap_t *hashmap, const char *key);

/**
 * @function	ymap_search_element
 *		Search an element in a hash map, and returns a pointer to the
 *		element item. Returns NULL if the element doesn't exist. This
 *		function is useful to find an element wich contains the NULL value,
 *		or to chek if an element exists.
 * @param	hashmap	Pointer to the hash map.
 * @param	key	Key used to index the element.
 * @return	A pointer to the element's structure.
 */
ymap_element_t *ymap_search_element(ymap_t *hashmap, const char *key);

/**
 * @function	ymap_remove
 * 		Remove an element from a hash map.
 * @param	hashmap	Pointer to the hash map.
 * @param	key	Key used to index the element.
 * @return	true if the element was found, false otherwise.
 */
bool ymap_remove(ymap_t *hashmap, const char *key);

/**
 * @function	ymap_resize
 *		Resize a hash map.
 * @param	hashmap	Pointer to the hash map.
 * @param	size	The new size.
 */
void ymap_resize(ymap_t *hashmap, size_t size);

/**
 * @function	ymap_foreach
 *		Apply a function on every elements of a hash map.
 * @param	hashmap		Pointer to the hash map.
 * @param	func		Pointer to the executed function.
 * @param	user_data	Pointer to some user data.
 */
void ymap_foreach(ymap_t *hashmap, ymap_function_t func, void *user_data);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif /* __cplusplus || c_plusplus */

#endif /* __YHASHMAP_H__ */
