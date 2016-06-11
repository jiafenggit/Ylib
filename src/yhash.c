#include "ylib.h"

yhash_value_t yhash_compute(const char *key) {
	yhash_value_t hash_value;

	for (hash_value = 0; *key; key++)
		hash_value = *key + (hash_value << 6) + (hash_value << 16) - hash_value;
	return (hash_value);
}

