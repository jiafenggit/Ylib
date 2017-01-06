#include <strings.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "ydefs.h"
#include "yjson.h"

/* Private functions */
static void _yjson_remove_space(yjson_parser_t *json);
static yjson_value_t _yjson_parse_chunk(yjson_parser_t *json);
static void _yjson_parse_string(yjson_parser_t *json, yjson_value_t *value);
static void _yjson_parse_number(yjson_parser_t *json, yjson_value_t *value);
static void _yjson_parse_array(yjson_parser_t *json, yjson_value_t *value);
static void _yjson_parse_object(yjson_parser_t *json, yjson_value_t *value);

#define YJSON_INDENT(depth) do { for (int _i = 0; _i < depth; ++_i) printf("\t"); } while (0)

// Initialiser of JSON parser
yjson_parser_t *yjson_init(yjson_parser_t *json, char *input) {
	*json = (yjson_parser_t){
		.input = input,
		.ptr = input,
		.status = YENOERR
	};
	return (json);
}

// Starts a JSON parser.
yjson_value_t yjson_parse(yjson_parser_t *json) {
	yjson_value_t value = _yjson_parse_chunk(json);
	if (json->status == YENOERR && *json->ptr != '\0') {
		json->status = YESYNTAX;
		value.type = YJSON_UNDEF;
	}
	return (value);
}

// Prints a JSON value node and its subnodes.
static void _yjson_array_print_elem(size_t index, void *data, void *user_data) {
	yjson_value_t *val = data;
	int *p = user_data;
	int depth = p[0], len = p[1];
	yjson_value_print(val, depth + 1, false);
	if ((int)index < (len - 1))
		printf(",");
	printf("\n");
}
static void _yjson_object_print_elem(char *key, void *data, void *user_data) {
	yjson_value_t *val = data;
	int *p = user_data;
	int depth = p[0], len = p[1];
	YJSON_INDENT(depth + 1);
	printf("\"%s\": ", key);
	yjson_value_print(val, depth + 1, false);
	if (len)
		printf(",");
	printf("\n");
}
void yjson_value_print(yjson_value_t *value, int depth, bool linefeed) {
	YJSON_INDENT(depth);
	if (value->type == YJSON_BOOL) {
		printf("%s", value->bool_value ? "true" : "false");  
	} else if (value->type == YJSON_INT) {
		printf("%lld", value->int_value);
	} else if (value->type == YJSON_FLOAT) {
		printf("%g", value->float_value);
	} else if (value->type == YJSON_STRING) {
		printf("\"%s\"", value->string_value);
	} else if (value->type == YJSON_ARRAY) {
		printf("[\n");
		int p[2] = {depth, yvector_count(value->array_value)};
		yvector_foreach(value->array_value, _yjson_array_print_elem, p);
		YJSON_INDENT(depth);
		printf("]");
	} else if (value->type == YJSON_OBJECT) {
		printf("[object]");
		printf("{\n");
		int p[2] = {depth, value->object_value->used};
		ymap_foreach(value->object_value, _yjson_object_print_elem, p);
	}
	if (linefeed)
		printf("\n");
}

// Parse
static void _yjson_remove_space(yjson_parser_t *json) {
	while (isspace(*json->ptr)) {
		if (*json->ptr == '\n')
			++json->line;
		++json->ptr;
	}
}

static yjson_value_t _yjson_parse_chunk(yjson_parser_t *json) {
	char c;
	yjson_value_t result = {0};

	_yjson_remove_space(json);
	c = *json->ptr;
	if (c == '\0')
		goto end;
	else if (c == '{') {
		// object
		json->ptr++;
		_yjson_parse_object(json, &result);
		goto end;
	} else if (c == '[') {
		// array
		json->ptr++;
		_yjson_parse_array(json, &result);
		goto end;
	} else if (c == '"') {
		// string
		json->ptr++;
		_yjson_parse_string(json, &result);
		goto end;
	} else if (!strncasecmp(json->ptr, "null", 4)) {
		// null
		json->ptr += 4;
		result.type = YJSON_NULL;
		goto end;
	} else if (!strncasecmp(json->ptr, "false", 5)) {
		// false
		json->ptr += 5;
		result = (yjson_value_t){
			.type = YJSON_BOOL,
			.bool_value = false
		};
		goto end;
	} else if (!strncasecmp(json->ptr, "true", 4)) {
		// true
		json->ptr += 4;
		result = (yjson_value_t){
			.type = YJSON_BOOL,
			.bool_value = true
		};
		goto end;
	} else {
		// number
		_yjson_parse_number(json, &result);
		goto end;
	}
end:
	if (json->status != YENOERR)
		result.type = YJSON_UNDEF;
	else
		_yjson_remove_space(json);
	return (result);
}

static void _yjson_parse_string(yjson_parser_t *json, yjson_value_t *value) {
	value->string_value = json->ptr;
	while (*json->ptr != '\0' &&
	       (*json->ptr != '"' ||
	        (json->ptr > json->input && *(json->ptr - 1) == '\\'))) {
		if (*json->ptr == '\n')
			++json->line;
		++json->ptr;
	}
	if (*json->ptr != '"') {
		value->type = YJSON_UNDEF;
		json->status = YESYNTAX;
	}
	value->type = YJSON_STRING;
	*json->ptr = '\0';
	++json->ptr;
}

static void _yjson_parse_number(yjson_parser_t *json, yjson_value_t *value) {
	/*
	 * Number are on the form:
	 * minus character (optional)
	 * one or more digits (0-9)
	 * dot character followed by one or more digits (optional)
	 * exponential part (optional)
	 *	"e|e+|e-|E|E+|E-" followed by one or more digits
	 */
	char *end;
	char *ptr = json->ptr;
	bool dot = false;

	// check syntax
	if (*ptr == '-')
		++ptr;
	if (!isdigit(*ptr)) {
		json->status = YESYNTAX;
		return;
	}
	while (isdigit(*ptr))
		++ptr;
	if (*ptr == '.') {
		dot = true;
		++ptr;
		if (!isdigit(*ptr)) {
			json->status = YESYNTAX;
			return;
		}
	}
	// conversion
	if (dot) {
		value->type = YJSON_FLOAT;
		value->float_value = strtod(json->ptr, &end);
	} else {
		value->type = YJSON_INT;
		value->int_value = strtol(json->ptr, &end, 10);
	}
	json->ptr = end;
}

static void _yjson_parse_array(yjson_parser_t *json, yjson_value_t *value) {
	yvector_t *vect = yvector_new();

	value->array_value = vect;
	_yjson_remove_space(json);
	while (*json->ptr != '\0') {
		yjson_value_t val = _yjson_parse_chunk(json);
		if (json->status != YENOERR)
			return;
		yjson_value_t *pval = malloc(sizeof(yjson_value_t));
		*pval = val;
		yvector_push(vect, pval);
		_yjson_remove_space(json);
		if (*json->ptr == ']') {
			value->type = YJSON_ARRAY;
			++json->ptr;
			return;
		} else if (*json->ptr != ',') {
			json->status = YESYNTAX;
			return;
		}
		++json->ptr;
		_yjson_remove_space(json);
	}
}

static void _yjson_parse_object(yjson_parser_t *json, yjson_value_t *value) {
	ymap_t *map = ymap_create(10, NULL, json);

	printf("parse_object()\n");
	value->object_value = map;
	_yjson_remove_space(json);
	while (*json->ptr != '\0') {
		yjson_value_t val_key = _yjson_parse_chunk(json);
		if (json->status != YENOERR)
			return;
		char *key = val_key.string_value;
		_yjson_remove_space(json);
		if (*json->ptr != ':') {
			json->status = YESYNTAX;
			return;
		}
		++json->ptr;
		yjson_value_t val = _yjson_parse_chunk(json);
		if (json->status != YENOERR)
			return;
		yjson_value_t *pval = malloc(sizeof(yjson_value_t));
		*pval = val;
		ymap_add(map, key, pval);
		if (*json->ptr == '}') {
			value->type = YJSON_OBJECT;
			++json->ptr;
			return;
		} else if (*json->ptr != ',') {
			json->status = YESYNTAX;
			return;
		}
		++json->ptr;
		_yjson_remove_space(json);
	}
}
