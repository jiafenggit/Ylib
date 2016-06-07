/* *** definition of private functions *** */
static yhash_value_t _yhash_compute(const char *key);

/**
 * @function	_yhash_compute
 *		Compute the hash value of a key, using the SDBM algorithm.
 * @see		http://www.cse.yorku.ca/~oz/hash.html
 * @see		http://en.literateprograms.org/Hash_function_comparison_%28C,_sh%29
 * @param	Key	The data to hash.
 * @return	The computed hash value.
 */
static yhash_value_t _yhash_compute(const char *key) {
	yhash_value_t hash_value;

	for (hash_value = 0; *key; key++)
		hash_value = *key + (hash_value << 6) + (hash_value << 16) - hash_value;
	return (hash_value);
}

