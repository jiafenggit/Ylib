#include "ydefs.h"
#include "ymem.h"
#include "ylist.h"

/* Creates a new list. */
ylist_t *ylist_new() {
	ylist_t *list;

	list = (ylist_t*)ymalloc(sizeof(ylist_t));
	return (list);
}

/* Apply a function on a list's elements. */
void ylist_foreach(ylist_t *list, ylist_func_t func, void *user_data) {
	if (!func)
		return;
	if (!list->ptr) {
		if (!list->first)
			return;
		list->ptr = list->first;
	}
	while (list->ptr) {
		if (list->ptr->data) {
			if (!func(list->ptr->data, user_data))
				break;
		}
		list->ptr = list->ptr->next;
	}
	list->ptr = list->first;	
}

/* Put the internal pointer to the first element of the list. */
void ylist_rewind(ylist_t *list) {
	list->ptr = list->first;
}

/* Destroy a list and all its elements. */
bool ylist_free(ylist_t *list, ylist_func_t delete_function, void *user_data) {
	ylist_elem_t *next_elem;
	bool status = true;

	for (list->ptr = list->first; list->ptr; list->ptr = next_elem) {
		if (list->ptr->data && delete_function) {
			if (!(status = delete_function(list->ptr->data, user_data)))
				break;
		}
		next_elem = list->ptr->next;
		yfree(list->ptr);
	}
	if (status)
		yfree(list);
	return (status);
}

/* Add an element at the end of a list. */
ylist_elem_t *ylist_add(ylist_t *list, void *data) {
	ylist_elem_t *elem;

	elem = ymalloc(sizeof(ylist_elem_t));
	elem->data = data;
	elem->prev = list->last;
	elem->list = list;
	if (list->last)
		list->last->next = elem;
	else
		list->last = list->first = elem;
	list->last = elem;
	return (elem);
}

/* Add an element at the beginnin of a list. */
ylist_elem_t *ylist_put(ylist_t *list, void *data) {
	ylist_elem_t *elem;

	elem = ymalloc(sizeof(ylist_elem_t));
	elem->data = data;
	elem->next = list->first;
	elem->list = list;
	if (list->first)
		list->first->prev = elem;
	else
		list->first = list->last = elem;
	list->first = elem;
	return (elem);
}

/* Remove the last element of a list. */
void *ylist_get(ylist_t *list) {
	ylist_elem_t *elem;
	void *data;

	if (list->last == NULL)
		return (NULL);
	elem = list->last;
	list->last = elem->prev;
	if (elem->prev)
		elem->prev->next = NULL;
	else
		list->first = NULL;
	data = elem->data;
	yfree(elem);
	return (data);
}

/** Remove the first element of a list. */
void *ylist_pop(ylist_t *list) {
	ylist_elem_t *elem;
	void *data;

	if (list->first == NULL)
		return (NULL);
	elem = list->first;
	list->first = elem->next;
	if (elem->next)
		elem->next->prev = NULL;
	else
		list->last = NULL;
	data = elem->data;
	yfree(elem);
	return (data);
}

/* Extract a list element from one list, and add it at the end of another list. */
void ylist_swap(ylist_elem_t *elem, ylist_t *dest) {
	ylist_t *src;

	// extract from source list
	src = elem->list;
	if (src->first == elem)
		src->first = elem->next;
	else if (src->last == elem)
		src->last = elem->prev;
	if (elem->next)
		elem->next->prev = elem->prev;
	if (elem->prev)
		elem->prev->next = elem->next;
	// add to destination list
	elem->list = dest;
	if (dest->last) {
		elem->prev = dest->last;
		dest->last->next = elem;
	} else {
		elem->prev = elem->next = NULL;
		dest->first = dest->last = elem;
	}
}
