#include <string.h>
#include <ymem.h>
#include "ydom.h"
#include "ylog.h"

/* Private prototypes -- DON'T USE THEM */
static bool _ydom_xpath_contains_pipe(const char *str);
static yvect_t _ydom_get_descendant_list(ydom_node_t *node);
static yvect_t _ydom_get_descendant_or_self_list(ydom_node_t *node);
static char *_ydom_get_next_xpath_name(const char **str);
static yvect_t _ydom_get_attributes_of_node(ydom_node_t *node, char *attr_name);
static yvect_t _ydom_get_children_of_node(ydom_node_t *node, char *child_name);
static bool _ydom_is_child_of_node(ydom_node_t *node, char *child_name);
static bool _ydom_is_attr_of_node(ydom_node_t *node, char *attr);
static bool _ydom_is_boolean_expression(const char *pt);
static int _ydom_cmp_equal(char *s1, char *s2);
static int _ydom_cmp_not_equal(char *s1, char *s2);
static yvect_t _ydom_process_boolean(yvect_t list, const char *pt);

/* notes : a implementer
** noeud			-- OK
** noeud/noeud			-- OK
** ../noeud			-- OK
** ./noeud			-- OK
** *				-- OK
** //				-- OK
** /				-- OK
** @attribut			-- OK
** @*				-- OK
** noeud/noeud1 | noeud2	-- OK
** node()
** text()
** comment()
** processing-instruction()
** /toto[1]			1er noeud 'toto'
** /toto[titi]			les noeuds 'toto' ayant un fils 'titi'
** /toto[@titre='toto']		les noeuds 'toto' avec @titre='toto'
** /toto[titi | tutu]		les noeuds 'toto' ayant un fils 'titi' ou 'tutu'
** /toto[@titre='toto' | tutu]
** /toto[@titre='toto' | @chapitre='tutu']
** /toto[last()]		le dernier noeud 'toto'
** /toto[position() &lt; 3]	les deux premiers noeuds 'toto'
** /toto[position() != last()]	tous les noeuds 'toto' sauf le dernier
*/

/*
** ydom_get_nodes_from_xpath()
** main XPath parsing function
** return a set of nodes, or NULL if the path isn't correct
*/
yvect_t ydom_get_nodes_from_xpath(yvect_t nodes, char *xpath) {
	const char *pt = NULL;
	bool first_char = true;
	yvect_t set, tmp;
	ydom_node_t *node = NULL;
	char *tmp_char = NULL;

	YLOG_ADD(YLOG_DEBUG, "Entering");
	if (!yv_len(nodes) || xpath == NULL || !strlen(xpath)) {
		YLOG_ADD(YLOG_DEBUG, "No input nodes");
		return (NULL);
	}
	if (!(set = yv_new(0))) {
		YLOG_ADD(YLOG_DEBUG, "Memory error");
		return (NULL);
	}
	for (pt = xpath, first_char = true; *pt; pt++, first_char = false) {
		if (IS_SPACE(*pt))
			continue;
		if (*pt == SLASH) {
			if (*(pt + 1) == SLASH) {
				/* Xpath example : "//foo" */
				pt++;
				while ((node = yv_pop(nodes))) {
					tmp = _ydom_get_descendant_or_self_list(node);
					yv_cat(&set, tmp);
					yv_free(&tmp, NULL, NULL);
				}
				tmp = nodes;
				nodes = set;
				set = tmp;
			} else if (first_char) {
				/* Xpath example : "/foo" */
				while ((node = yv_pop(nodes)))
					yv_add(&set, _ydom_get_root_node_of_node(node));
				tmp = nodes;
				nodes = set;
				set = tmp;
			}
		} else if (*pt == DOT && *(pt + 1) == DOT) {
			/* Xpath example : ".." */
			pt++;
			while ((node = yv_pop(nodes)))
				if (node->parent && node->parent->node_type == ELEMENT_NODE)
					yv_add(&set, node->parent);
			tmp = nodes;
			nodes = set;
			set = tmp;
			yv_uniq(nodes);
		} else if (*pt == AT) {
			/* Xpath examples : "@attribute" or "@*" */
			pt++;
			tmp_char = _ydom_get_next_xpath_name(&pt);
			pt--;
			if (tmp_char == NULL)
				continue;
			while ((node = yv_pop(nodes))) {
				tmp = _ydom_get_attributes_of_node(node, tmp_char);
				yv_cat(&set, tmp);
				yv_free(&tmp, NULL, NULL);
			}
			tmp = nodes;
			nodes = set;
			set = tmp;
			yfree(tmp_char);
		} else if (*pt == LSQUAREBRACKET) {
			yvect_t tmp2;
			if (!(tmp2 = yv_new(0))) {
				YLOG_ADD(YLOG_ERR, "Unable to allocate memory");
				return (NULL);
			}
			/* Xpath examples : "toto[titi | tutu]" */
			pt++;
			while (_ydom_xpath_contains_pipe(pt)) {
				tmp_char = _ydom_get_next_xpath_name(&pt);
				yv_add(&set, tmp_char);
				for (; *pt == PIPE || IS_SPACE(*pt); ++pt)
					;
			}
			if (!(tmp_char = _ydom_get_next_xpath_name(&pt)))
				continue;
			yv_add(&set, tmp_char);
			while ((node = yv_pop(nodes))) {
				size_t offset;
				for (offset = 0; offset < yv_len(set); ++offset) {
					tmp_char = set[offset];
					if (*tmp_char == AT) {
						if (_ydom_is_attr_of_node(node, tmp_char))
							yv_add(&tmp2, node);
					} else if (_ydom_is_child_of_node(node, tmp_char))
						yv_add(&tmp2, node);
				}
			}
			while ((tmp_char = yv_pop(set)))
				yfree(tmp_char);
			yv_free(&nodes, NULL, NULL);
			yv_trunc(set, NULL, NULL);
			nodes = tmp2;
		} else if (_ydom_is_boolean_expression(pt)) {
			/* Xpath example : "toto != 'titi'" */
			YLOG_ADD(YLOG_DEBUG, "Exiting after boolean expression");
			return (_ydom_process_boolean(nodes, pt));
		} else {
			while (_ydom_xpath_contains_pipe(pt)) {
				size_t i;
				tmp_char = _ydom_get_next_xpath_name(&pt);
				for (i = 0; i < yv_len(nodes); ++i) {
					node = nodes[i];
					tmp = _ydom_get_children_of_node(node, tmp_char);
					yv_cat(&set, tmp);
					yv_free(&tmp, NULL, NULL);
				}
				yfree(tmp_char);
				for (; *pt == PIPE || IS_SPACE(*pt); ++pt)
					;
			}
			tmp_char = _ydom_get_next_xpath_name(&pt);
			pt--;
			if (tmp_char == NULL) {
				YLOG_ADD(YLOG_DEBUG, "Exiting abnormally");
				return (nodes);
			}
			while ((node = yv_pop(nodes))) {
				if ((tmp = _ydom_get_children_of_node(node, tmp_char))) {
					yv_cat(&set, tmp);
					yv_free(&tmp, NULL, NULL);
				}
			}
			yfree(tmp_char);
			tmp = nodes;
			nodes = set;
			set = tmp;
		}
	}
	yv_free(&set, NULL, NULL);
	YLOG_ADD(YLOG_DEBUG, "Exiting");
	return (nodes);
}

/*
** _ydom_xpath_contains_pipe() -- INTERNAL FUNCTION
** Return true if the string contains a '|' charactere, 0 otherwise
*/
static bool _ydom_xpath_contains_pipe(const char *str) {
	while (IS_SPACE(*str))
		str++;
	while (*str && *str != LT && *str != GT && *str != SLASH &&
	       *str != INTERROG && *str != EXCLAM && /**str != EQ &&*/
	       *str != LSQUAREBRACKET && *str != RSQUAREBRACKET &&
	       /**str != AT &&*/ *str != PIPE)
		str++;
	if (*str == PIPE)
		return (true);
	return (false);
}

/*
** _ydom_get_descendant_list() -- INTERNAL FUNCTION
** return a vector of nodes which contains copies of each sons, sons of sons, ...
** of the node
*/
static yvect_t _ydom_get_descendant_list(ydom_node_t *node) {
	ydom_node_t *node_pt = NULL;
	yvect_t tmp;
	yvect_t res;
	
	if (!(res = yv_new(0)))
		return (NULL);
	for (node_pt = node->first_child; node_pt; node_pt = node_pt->next) {
		if (node_pt->node_type == ELEMENT_NODE) {
			yv_add(&res, node_pt);
			tmp = _ydom_get_descendant_list(node_pt);
			yv_cat(&res, tmp);
			yv_free(&tmp, NULL, NULL);
		}
	}
	return (res);
}

/*
** _ydom_get_descendant_or_self_list() -- INTERNAL FUNCTION
** return a vector of nodes which contains a copy of the node, and copies of each
** sons, sons of sons, ... of the node
*/
static yvect_t _ydom_get_descendant_or_self_list(ydom_node_t *node) {
	yvect_t tmp;
	yvect_t res;

	if (!(res = yv_new(0)))
		return (NULL);
	yv_add(&res, node);
	tmp = _ydom_get_descendant_list(node);
	yv_cat(&res, tmp);
	yv_free(&tmp, NULL, NULL);
	return (res);
}

/*
** _ydom_get_next_xpath_name() -- INTERNAL FUNCTION
** return a copy of the next name in an Xpath string, and put the string pointer
** at the end of the name
*/
static char *_ydom_get_next_xpath_name(const char **str) {
	ystr_t res;
	char *result;
	
	while (IS_SPACE(**str))
		(*str)++;
	if (!(res = ys_new("")))
		return (NULL);
	while (**str && **str != LT && **str != GT && **str != SLASH &&
	       **str != INTERROG && **str != EXCLAM && /***str != EQ &&*/
	       **str != LSQUAREBRACKET && **str != RSQUAREBRACKET &&
	       /***str != AT &&*/ **str != PIPE && !IS_SPACE(**str)) {
		if (**str == LPAR || **str == RPAR) {
			ys_free(&res);
			return (NULL);
		}
		ys_addc(&res, **str);
		(*str)++;
		if (*(*str - 1) == QUOTE) {
			for (; **str != QUOTE; (*str)++)
				ys_addc(&res, **str);
			if (**str == QUOTE) {
				ys_addc(&res, **str);
				(*str)++;
			}
		} else if (*(*str - 1) == DQUOTE) {
			for (; **str != DQUOTE; (*str)++)
				ys_addc(&res, **str);
			if (**str == DQUOTE) {
				ys_addc(&res, **str);
				(*str)++;
			}
		}
	}
	while (IS_SPACE(**str))
		(*str)++;
	result = ys_string(res);
	ys_free(&res);
	return (result);
}

/*
** _ydom_get_attributes_of_node() -- INTERNAL FUNCTION
** return a list of attributes matching a name
** if the attribute name parameter is equal to NULL, all attributes are returned
*/
static yvect_t _ydom_get_attributes_of_node(ydom_node_t *node, char *attr_name) {
	ydom_node_t *node_pt = NULL;
	yvect_t res;

	if (!(res = yv_new(0)))
		return (NULL);
	if (*attr_name == ASTERISK)
		attr_name = NULL;
	for (node_pt = node->attributes; node_pt; node_pt = node_pt->next) {
		if (attr_name == NULL || !strcmp(attr_name, node_pt->name))
			yv_add(&res, node_pt);
	}
	return (res);
}

/*
** _ydom_get_children_of_node() -- INTERNAL FUNCTION
** return a vector of children nodes matching a name
** if the child name parameter is equal to NULL, all children are returned
** return NULL if the node doesn't have any child
*/
static yvect_t _ydom_get_children_of_node(ydom_node_t *node, char *child_name) {
	ydom_node_t *node_pt = NULL;
	yvect_t res;

	if (!(res = yv_new(0)))
		return (NULL);
	if (*child_name == ASTERISK)
		child_name = NULL;
	for (node_pt = node->first_child; node_pt; node_pt = node_pt->next) {
		if (node_pt->node_type == ELEMENT_NODE &&
		    (child_name == NULL || !strcmp(child_name, node_pt->name))) {
			yv_add(&res, node_pt);
		}
	}
	return (res);
}

/*
** _ydom_is_child_of_node()
** return true if a node have a given child, otherwise 0
*/
static bool _ydom_is_child_of_node(ydom_node_t *node, char *child_name) {
	ydom_node_t *node_pt = NULL;

	for (node_pt = node->first_child; node_pt; node_pt = node_pt->next) {
		if (!strcmp(child_name, node_pt->name))
			return (true);
	}
	return (false);
}

/*
** _ydom_is_attr_of_node()
** return true if a node have a given attribute
*/
static bool _ydom_is_attr_of_node(ydom_node_t *node, char *attr) {
	ystr_t attr_name, attr_value;
	char *pt, *pt2;
	ydom_node_t *attr_pt = NULL;
	bool res = false;
	char init_delim = '\0';

	if (!(attr_name = ys_new("")))
		return (false);
	if (!(attr_value = ys_new(""))) {
		ys_free(&attr_name);
		return (false);
	}
	attr = (*attr == AT) ? (attr + 1) : attr;
	if ((pt = strchr(attr, EQ))) {
		ys_nappend(&attr_name, attr, pt - attr);
		pt++;
		for (; IS_SPACE(*pt); ++pt)
			;
		if (*pt == QUOTE || *pt == DQUOTE) {
			init_delim = *pt;
			pt++;
		}
		if (init_delim == QUOTE) {
			if (!(pt2 = strchr(pt, QUOTE)))
				ys_append(&attr_value, pt);
			else
				ys_nappend(&attr_value, pt, pt2 - pt);
		} else if (!(pt2 = strchr(pt, DQUOTE)))
			ys_append(&attr_value, pt);
		else
			ys_nappend(&attr_value, pt, pt2 - pt);
			/*
			tmp = str2xmlentity(attr_value);
			yfree(attr_value);
			attr_value = tmp;
			*/
	} else
		ys_append(&attr_name, attr);
	for (attr_pt = node->attributes; attr_pt && !res; attr_pt = attr_pt->next)
		if (!strcmp(attr_name, attr_pt->name) &&
		    (!ys_len(attr_value) || !strcmp(attr_value, attr_pt->value)))
			res = true;
	ys_free(&attr_name);
	ys_free(&attr_value);
	return (res);
}

/*
** _ydom_is_boolean_expression()
** return true if the next word of an XPath expression is a boolean expression.
*/
static bool _ydom_is_boolean_expression(const char *pt) {
	if (!strncmp(pt, "=", 1) || !strncmp(pt, "!=", 2) || !strncmp(pt, "&gt;", 4) || 
	    !strncmp(pt, "&lt;", 4) || !strncmp(pt, "&lt;=", 5) || !strncmp(pt, "&gt;=", 5))
		return (true);
	return (false);
}

static int _ydom_cmp_equal(char *s1, char *s2) {
	return (strcmp(s1, s2));
}

static int _ydom_cmp_not_equal(char *s1, char *s2) {
	return (!strcmp(s1, s2));
}

/*
** _ydom_process_boolean()
** process a boolean expression
*/
static yvect_t _ydom_process_boolean(yvect_t list, const char *pt) {
	ydom_node_t *node = NULL;
	bool tmp_bool = false;
	yvect_t res;
	int (*f)();
	char *cmp, *s1, *s2;
	char init_delim;
	int i;

	if (!(res = yv_new(0)))
		return (NULL);
	if (!strncmp(pt, "=", 1)) {
		f = _ydom_cmp_equal;
		pt++;
	} else if (!strncmp(pt, "!=", 2)) {
		f = _ydom_cmp_not_equal;
		pt += 2;
	} else {
		yv_free(&res, NULL, NULL);
		return (NULL);
	}
	cmp = _ydom_get_next_xpath_name(&pt);
	if (cmp[0] == QUOTE || cmp[0] == DQUOTE) {
		init_delim = cmp[0];
		for (i = 0; cmp[i]; ++i)
			cmp[i] = cmp[i + 1];
		if ((cmp[strlen(cmp) - 1] == QUOTE && init_delim == QUOTE)|| 
		    (cmp[strlen(cmp) - 1] == DQUOTE && init_delim == DQUOTE)) {
			cmp[strlen(cmp) - 1] = '\0';
		}
	}
	while ((node = yv_pop(list))) {
		s1 = ydom_node_get_value(node);
		s2 = ydom_node_get_name(node);
		if ((ydom_node_is_attr(node) && !f(s1, cmp)) ||
		    (ydom_node_is_element(node) && !f(s2, cmp))) {
			yfree(s1);
			yfree(s2);
			tmp_bool = true;
			break;
		}
		yfree(s1);
		yfree(s2);
	}
	yv_free(&list, NULL, NULL);
	yfree(cmp);
	if (tmp_bool) {
		yv_add(&res, node);
	}
	return (res);
}
