/* ********************************************************
 * *                      HASH MAP                        *
 * ******************************************************** */

/* ************ PRIVATE DEFINITIONS AND MACROS ************ */

/* *********** PRIVATE STRUCTURE AND TYPES **************** */

/* ************** PRIVATE FUNCTION DECLARATIONS *********** */
static ymap_t *_ymap_create(size_t size, ymap_function_t destroy_func, void *destroy_data);
static void _ymap_free(ymap_t *hashmap);
static void _ymap_add(ymap_t *hashmap, char *key, void *data);
static void *_ymap_search(ymap_t *hashmap, const char *key);
static ymap_element_t *_ymap_search_element(ymap_t *hashmap, const char *key);
static bool _ymap_remove(ymap_t *hashmap, const char *key);
static void _ymap_resize(ymap_t *hashmap, size_t size);
static void _ymap_foreach(ymap_t *hashmap, ymap_function_t func, void *user_data);

/**
 * @function	_ymap_create
 *		Creates a new hash map.
 * @param	size		Initial size of the hash map.
 * @param	destroy_func	Pointer to the function called when an element is removed.
 * @param	destroy_data	Pointer to some suer data given to the destroy function.
 * @return	The created hash map.
 */
static ymap_t *_ymap_create(size_t size, ymap_function_t destroy_func, void *destroy_data) {
	ymap_t *hash;

	hash = (ymap_t*)_ymalloc(sizeof(ymap_t));
	hash->buckets = (ymap_bucket_t*)_ycalloc(size, sizeof(ymap_bucket_t));
	hash->size = size;
	hash->used = 0;
	hash->destroy_func = destroy_func;
	hash->destroy_data = destroy_data;
	return (hash);
}

/**
 * @function	_ymap_free
 *		Destroy a hash map.
 * @param	hash	Pointer to the hash map.
 */
static void _ymap_free(ymap_t *hashmap) {
	/* remove elements */
	if (hashmap->used > 0) {
		size_t		offset, offset2;
		ymap_bucket_t	*bucket;
		ymap_element_t	*element, *old_element;

		for (offset = 0; offset < hashmap->size; offset++) {
			bucket = &(hashmap->buckets[offset]);
			for (offset2 = 0, element = bucket->elements;
			     offset2 < bucket->nbr_elements;
			     offset2++) {
				if (hashmap->destroy_func != NULL)
					hashmap->destroy_func(element->key, element->data, hashmap->destroy_data);
				old_element = element;
				element = element->next;
				_yfree(old_element);
			}
		}
	}
	/* remove buckets and the hash map itself */
	_yfree(hashmap->buckets);
	_yfree(hashmap);
}

/**
 * @function	_ymap_add
 *		Add an element to a hash map.
 * @param	hashmap	Pointer to the hash map.
 * @param	key	Key used to index the element.
 * @param	data	The element's data.
 */
static void _ymap_add(ymap_t *hashmap, char *key, void *data) {
	float		load_factor;
	yhash_value_t	hash_value;
	ymap_bucket_t	*bucket;
	ymap_element_t	*element;

	/* resize the map if its load factor would excess the limit */
	load_factor = (float)(hashmap->used + 1) / hashmap->size;
	if (load_factor > YHM_MAX_LOAD_FACTOR)
		_ymap_resize(hashmap, (hashmap->size * 2));
	/* compute the key's hash value */
	hash_value = _yhash_compute(key);
	hash_value %= hashmap->size;
	/* checking the bucket */
	bucket = &(hashmap->buckets[hash_value]);
	if (bucket->nbr_elements == 0) {
		/* create the first element */
		element = (ymap_element_t*)_ymalloc(sizeof(ymap_element_t));
		element->previous = element->next = element;
		/* add the element to the bucket */
		bucket->elements = element;
	} else {
		/* there is already some elements in the bucket, checking if the element exists and must be updated */
		size_t	offset;

		for (offset = 0, element = bucket->elements;
		     offset < bucket->nbr_elements;
		     offset++, element = element->next) {
			if (!strcmp(key, element->key)) {
				/* an existing element was found */
				/* removing old data */
				if (hashmap->destroy_func != NULL)
					hashmap->destroy_func(element->key, element->data, hashmap->destroy_data);
				/* updating the element */
				element->key = key;
				element->data = data;
				return;
			}
		}
		/* no element was already existing with this key */
		element = (ymap_element_t*)_ymalloc(sizeof(ymap_element_t));
		/* add the element to the bucket */
		element->next = bucket->elements;
		element->previous = bucket->elements->previous;
		bucket->elements->previous->next = element;
		bucket->elements->previous = element;
	}
	/* filling the element */
	element->key = key;
	element->data = data;
	/* update the bucket */
	bucket->nbr_elements++;
	/* update the hash map */
	hashmap->used++;
}

/**
 * @function	_ymap_search
 *		Search an element in a hash map, and returns its value. Returns
 *		NULL if the elements doesn't exist (and if it exists and contains
 *		NULL as its value).
 * @param	hashmap	Pointer to the hash map.
 * @param	key	Key used to index the element.
 * @return	A pointer to the element's data.
 */
static void *_ymap_search(ymap_t *hashmap, const char *key) {
	ymap_element_t	*element;

	element = _ymap_search_element(hashmap, key);
	if (element != NULL)
		return (element->data);
	return (NULL);
}

/**
 * @function	_ymap_search_element
 *		Search an element in a hash map, and returns a pointer to the
 *		element item. Returns NULL if the element doesn't exist. This
 *		function is useful to find an element wich contains the NULL value,
 *		or to chek if an element exists.
 * @param	hashmap	Pointer to the hash map.
 * @param	key	Key used to index the element.
 * @return	A pointer to the element's structure.
 */
static ymap_element_t *_ymap_search_element(ymap_t *hashmap, const char *key) {
	yhash_value_t	hash_value;
	ymap_bucket_t	*bucket;
	ymap_element_t	*element;
	size_t		offset;

	/* compute the key's hash value */
	hash_value = _yhash_compute(key);
	hash_value %= hashmap->size;
	/* retreiving the bucket */
	bucket = &(hashmap->buckets[hash_value]);
	if (bucket->nbr_elements == 0)
		return (NULL);
	if (bucket->nbr_elements == 1)
		return (bucket->elements);
	for (offset = 0, element = bucket->elements;
	     offset < bucket->nbr_elements;
	     offset++, element = element->next) {
		if (!strcmp(key, element->key))
			return (element);
	}
	return (NULL);
}

/*
 * @function	_ymap_remove
 * 		Remove an element from a hash map.
 * @param	hashmap	Pointer to the hash map.
 * @param	key	Key used to index the element.
 * @return	true if the element was found, false otherwise.
 */
static bool _ymap_remove(ymap_t *hashmap, const char *key) {
	yhash_value_t	hash_value;
	ymap_bucket_t	*bucket;
	ymap_element_t	*element;
	size_t		offset;
	bool		found = false;
	float		load_factor;

	/* compute the key's hash value */
	hash_value = _yhash_compute(key);
	hash_value %= hashmap->size;
	/* retreiving the bucket */
	bucket = &(hashmap->buckets[hash_value]);
	if (bucket->nbr_elements == 0)
		return (false);
	if (bucket->nbr_elements == 1) {
		found = true;
		if (hashmap->destroy_func)
			hashmap->destroy_func(bucket->elements->key, bucket->elements->data, hashmap->destroy_data);
		_yfree(bucket->elements);
		bucket->elements = NULL;
	} else {
		for (offset = 0, element = bucket->elements;
		     offset < bucket->nbr_elements;
		     offset++, element = element->next) {
			if (!strcmp(key, element->key)) {
				found = true;
				if (offset == 0)
					bucket->elements = element->next;
				element->previous->next = element->next;
				element->next->previous = element->previous;
				/* call the destroy function */
				if (hashmap->destroy_func)
					hashmap->destroy_func(element->key, element->data, hashmap->destroy_data);
				_yfree(element);
				break;
			}
		}
	}
	if (found) {
		bucket->nbr_elements--;
		hashmap->used--;
		/* resize the map if its load factor will fall under the limit */
		load_factor = (float)(hashmap->used + 1) / hashmap->size;
		if (load_factor < YHM_MIN_LOAD_FACTOR)
			_ymap_resize(hashmap, (hashmap->size / 2));
	}
	return (found);
}

/**
 * @function	_ymap_resize
 *		Resize a hash map.
 * @param	hashmap	Pointer to the hash map.
 * @param	size	The new size.
 */
static void _ymap_resize(ymap_t *hashmap, size_t size) {
	ymap_t		*new_hashmap;
	size_t		offset, offset2;
	ymap_bucket_t	*bucket, *old_buckets;
	ymap_element_t	*element, *old_element;

	new_hashmap = _ymap_create(size, NULL, NULL);
	for (offset = 0; offset < hashmap->size; offset++) {
		bucket = &(hashmap->buckets[offset]);
		if (bucket->nbr_elements == 0)
			continue;
		for (offset2 = 0, element = bucket->elements;
		     offset2 < bucket->nbr_elements;
		     offset2++, element = element->next) {
			_ymap_add(new_hashmap, element->key, element->data);
			old_element = element;
			_yfree(old_element);
		}
	}
	hashmap->size = size;
	/* moving the list of buckets */
	old_buckets = hashmap->buckets;
	hashmap->buckets = new_hashmap->buckets;
	/* free memory */
	_yfree(old_buckets);
	_yfree(new_hashmap);
}

/**
 * @function	_ymap_foreach
 *		Apply a function on every elements of a hash map.
 * @param	hashmap		Pointer to the hash map.
 * @param	func		Pointer to the executed function.
 * @param	user_data	Pointer to some user data.
 */
static void _ymap_foreach(ymap_t *hashmap, ymap_function_t func, void *user_data) {
	size_t		offset, nbr_processed, offset2;
	ymap_bucket_t	*bucket;
	ymap_element_t	*element;

	for (offset = nbr_processed = 0; offset < hashmap->size; offset++) {
		bucket = &(hashmap->buckets[offset]);
		if (bucket->nbr_elements == 0)
			continue;
		for (offset2 = 0, element = bucket->elements;
		     offset2 < bucket->nbr_elements;
		     offset2++, element = element->next) {
			func(element->key, element->data, user_data);
			if (++nbr_processed == hashmap->used)
				return;
		}
	}
}

