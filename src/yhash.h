#ifndef __YHASH_H__
#define __YHASH_H__

#include <stdint.h>

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif /* __cplusplus || c_plusplus */

/**
 * @typedef	yhash_value_t
 * 		An hash value.
 */
typedef uint64_t yhash_value_t;

/**
 * @function	yhash_compute
 *		Compute the hash value of a key, using the SDBM algorithm.
 * @see		http://www.cse.yorku.ca/~oz/hash.html
 * @see		http://en.literateprograms.org/Hash_function_comparison_%28C,_sh%29
 * @param	Key	The data to hash.
 * @return	The computed hash value.
 */
yhash_value_t yhash_compute(const char *key);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif /* __cplusplus || c_plusplus */

#endif /* __YHASHMAP_H__ */
