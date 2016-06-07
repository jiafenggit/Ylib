/*!
 * @header	ymap.h
 * @abstract	All definitions about hash maps.
 * @discussion  Hash maps are data structures that uses hash function to map values.
 * @version     1.0 Aug 13 2012
 * @author      Amaury Bouchard <amaury@amaury.net>
 */
#ifndef __YMAP_H__
#define __YMAP_H__

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

#endif /* __YHASHMAP_H__ */
