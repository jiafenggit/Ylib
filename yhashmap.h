/* Process this file with the HeaderBrowser tool (http://www.headerbrowser.org)
   to create documentation. */
/*!
 * @header	yhashmap.h
 * @abstract	All definitions about hash maps.
 * @discussion  Hash maps are data structures that uses hash function to map values.
 * @version     1.0 Aug 13 2012
 * @author      Amaury Bouchard <amaury@amaury.net>
 */
#ifndef __YHASHMAP_H__
#define __YHASHMAP_H__

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif /* __cplusplus || c_plusplus */

#ifdef USE_BOEHM_GC
# include "gc.h"
#endif /* USE_BOEHM_GC */

/** @define YHM_MAX_LOAD_FACTOR	Maximum load factor of a hash map before increasing it. */
#define YHM_MAX_LOAD_FACTOR	0.7

/** @define YHM_MIN_LOAD_FACTOR Minimum load factor of a hash map before reducing it. */
#define YHM_MIN_LOAD_FACTOR	0.25

/**
 * @typedef	yhm_size_t
 *		Enum used to define the size of a hash map.
 * @constant	YHM_SIZE_MINI		Minimal size of hash maps (32).
 * @constant	YHM_SIZE_MEDIUM		Medium size of hash maps (256).
 * @constant	YHM_SIZE_DEFAULT	Default size of hash maps (4K).
 * @constant	YHM_SIZE_BIG		Size of big hash maps (65K).
 * @constant	YHM_SIZE_HUGE		Size of huge hash maps (1M).
 */
typedef enum yhm_size_e {
	YHM_SIZE_MINI		= 32,
	YHM_SIZE_MEDIUM		= 256,
	YHM_SIZE_DEFAULT	= 4096,
	YHM_SIZE_BIG		= 65536,
	YHM_SIZE_HUGE		= 1048576
} yhm_size_t;

/**
 * typedef	yhm_element_t
 *		Structure used to store a hash map's element.
 * @field	key		Element's key.
 * @field	data		Element's data.
 * @field	previous	Pointer to the previous element with the same hash value.
 * @field	next		Pointer to the next element with the same hash value.
 */
typedef struct yhm_element_s {
	char			*key;
	void			*data;
	struct yhm_element_s	*previous;
	struct yhm_element_s	*next;
} yhm_element_t;

/**
 * typedef	yhm_bucket_t
 *		Structure used to store the values associated with a hash value.
 * @field	nbr_elements	Elements count for this bucket.
 * @field	elements	Pointer to the first element.
 */
typedef struct yhm_bucket_s {
	size_t		nbr_elements;
	yhm_element_t	*elements;
} yhm_bucket_t;

/**
 * typedef	yhm_function_t
 *		Function pointer, used to apply a procedure to an element.
 * @param	key		Pointer to the key.
 * @param	data		Pointer to the data.
 * @param	user_data	Pointer to some user data.
 */
typedef void (*yhm_function_t)(char *key, void *data, void *user_data);

/**
 * typedef	yhm_hash_value_t
 * 		An hash value.
 */
typedef long int yhm_hash_value_t;

/**
 * @typedef	yhashmap_t
 * @field	size		Current size of the hash map.
 * @field	used		Current number of elements stored in the hash map.
 * @field	buckets		Array of buckets.
 * @field	destroy_func	Pointer to the function called when an element is removed.
 * @field	destroy_data	Pointer to some user data given to the destroy function.
 */
typedef struct yhashmap_s {
	size_t		size;
	size_t		used;
	yhm_bucket_t	*buckets;
	yhm_function_t	destroy_func;
	void		*destroy_data;
} yhashmap_t;

/* ****************** FUNCTIONS **************** */
/**
 * @function	yhm_new
 *		Creates a new hash map.
 * @param	size		Initial size of the hash map.
 * @param	destroy_func	Pointer to the function called when an element is removed.
 * @param	destroy_data	Pointer to some suer data given to the destroy function.
 * @return	The created hash map.
 */
yhashmap_t *yhm_new(size_t size, yhm_function_t destroy_func, void *destroy_data);

/**
 * @function	yhm_delete
 *		Destroy a hash map.
 * @param	hash	Pointer to the hash map.
 */
void yhm_delete(yhashmap_t *hashmap);

/**
 * @function	yhm_hash
 *		Compute the hash value of a key, using the SDBM algorithm.
 * @see		http://www.cse.yorku.ca/~oz/hash.html
 * @see		http://en.literateprograms.org/Hash_function_comparison_%28C,_sh%29#chunk
 * @param	Key	The data to hash.
 * @return	The computed hash value.
 */
yhm_hash_value_t yhm_hash(const char *key);

/**
 * @function	yhm_add
 *		Add an element to a hash map.
 * @param	hashmap	Pointer to the hash map.
 * @param	key	Key used to index the element.
 * @param	data	The element's data.
 */
void yhm_add(yhashmap_t *hashmap, char *key, void *data); 

/**
 * @function	yhm_search
 *		Search an element in a hash map, from its key.
 * @param	hashmap	Pointer to the hash map.
 * @param	key	Key used to index the element.
 * @return	A pointer to the element's data.
 */
void *yhm_search(yhashmap_t *hashmap, const char *key);

/*
 * @function	yhm_remove
 * 		Remove an element from a hash map.
 * @param	hashmap	Pointer to the hash map.
 * @param	key	Key used to index the element.
 * @return	1 if the elemetn was found, 0 otherwise.
 */
char yhm_remove(yhashmap_t *hashmap, const char *key);

/**
 * @function	yhm_resize
 *		Resize a hash map.
 * @param	hashmap	Pointer to the hash map.
 * @param	size	The new size.
 */
void yhm_resize(yhashmap_t *hashmap, size_t size);

/**
 * @function	yhm_foreach
 *		Apply a function on every elements of a hash map.
 * @param	hashmap		Pointer to the hash map.
 * @param	func		Pointer to the executed function.
 * @param	user_data	Pointer to some user data.
 */
void yhm_foreach(yhashmap_t *hashmap, yhm_function_t func, void *user_data);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif /* __cplusplus || c_plusplus */

#endif /* __YHASHMAP_H__ */