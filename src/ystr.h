/* Process this file with the HeaderBrowser tool (http://www.headerbrowser.org)
   to create documentation. */
/*!
 * @header	ystr.h
 * @abstract	All definitions about ystrings.
 * @discussion	The ystrings are a type of buffered strings. When you get a 
 *		ystring, you get a pointer to a character string. It is a 
 *		(char*) pointer, as for any C's character string. But, in fact,
 *		there is more allocated memory than used memory. It is usefull
 *		to extend the string without new memory allocation and copy. 
 *		And before the address of the first character of the string,
 *		there is two integers ; the first that contains the total
 *		allocated size (used string memory + '\0' + free memory size) ;
 *		the second that contains the used memory size. The used size 
 *		doesn't count the ending '\0' character of the string.
 * @version	1.0 May 17 2002
 * @author	Amaury Bouchard <amaury@amaury.net>
 */
#ifndef __YSTR_H__
#define __YSTR_H__

/*! @typedef ystr_t Type definition equivalent to the character string part
 * of ystrings. A variable of this type can always be casted as (char*). */
typedef char* ystr_t;

#endif /* __YSTR_H__ */
