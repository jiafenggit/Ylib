/* Process this file with the HeaderBrowser tool (http://www.headerbrowser.org)
   to create documentation. */
/*!
 * @header	yvect.h
 * @abstract	All definitions about yvectors.
 * @discussion	Vectors are bufferised arrays of pointers.
 * @version	1.0 Jun 13 2002
 * @author	Amaury Bouchard <amaury@amaury.net>
 */
#ifndef __YVECT_H__
#define __YVECT_H__

/*! @typedef yvect_t Vector type definition. Always equivalent to (void**). */
typedef void** yvect_t;

/**
 * @typedef	yvect_function_t
 *		Function pointer, used to apply a procedure to an element.
 * @param	index		Index of the element in the vector.
 * @param	data		Pointer to the data.
 * @param	user_data	Pointer to some user data.
 */
typedef void (*yvect_function_t)(size_t index, void *data, void *user_data);

#endif /* __YVECT_H__ */
