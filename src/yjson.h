#pragma once

#include <stdbool.h>
#include "yerror.h"
#include "yvector.h"
#include "ymap.h"

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif /* __cplusplus || c_plusplus */

/**
 * @typedef	yjson_parser_t
 *		JSON parser object.
 * @field	input	Pointer to the input string.
 * @field	ptr	Pointer to the currently parsed character.
 * @field	line	Number of the currently parsed line.
 * @field	status	Parsing status.
 */
typedef struct yjson_parser_s {
	char *input;
	char *ptr;
	unsigned int line;
	yerr_t status;
} yjson_parser_t;

/**
 * @typedef	yjson_value_t
 *		JSON node element value.
 * @field	type		Type of the node.
 * @field	bool_value	Boolean value.
 * @field	int_value	Integer value.
 * @field	float_value	Floating point number value.
 * @field	string_value	Pointer to character string value.
 * @field	array_value	Vector of nodes.
 * @field	object_value	Map of keys/values.
 */
typedef struct yjson_value_s {
	enum {
		YJSON_UNDEF = 0,
		YJSON_NULL,
		YJSON_BOOL,
		YJSON_INT,
		YJSON_FLOAT,
		YJSON_STRING,
		YJSON_ARRAY,
		YJSON_OBJECT
	} type;
	union {
		bool bool_value;
		long long int int_value;
		double float_value;
		char *string_value;
		yvector_t *array_value;
		ymap_t *object_value;
	};
} yjson_value_t;

/**
 * @function	yjson_init
 *		Initialiser of JSON parser.
 * @param	json	Pointer to JSON parser object.
 * @param	input	Pointer to the string to parse.
 * @return	A pointer to the initialised JSON parser object.
 */
yjson_parser_t *yjson_init(yjson_parser_t *json, char *input);

/**
 * @function	yjson_parse
 *		Starts a JSON parser.
 * @param	json	Pointer to the JSON parser object.
 * @return	The root node value.
 */
yjson_value_t yjson_parse(yjson_parser_t *json);

/** @define yjson_print Prints a JSON value. */
#define yjson_print(val)	yjson_value_print(val, 0, true)

/**
 * @function	yjson_value_print
 *		Prints a JSON value node and its subnodes.
 * @param	value		Pointer to the value node.
 * @param	depth		Indentation depth.
 * @param	linefeed	True to add a linefeed at the end.
 */
void yjson_value_print(yjson_value_t *value, int depth, bool linefeed);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif /* __cplusplus || c_plusplus */
