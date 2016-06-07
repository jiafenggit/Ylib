/* ********************************************************
 * *                       YSTR                           *
 * ******************************************************** */

/* *********** PRIVATE DEFINITIONS AND MACROS ************ */
/*! @define _YSTR_DEFAULT_SIZE Default size for ystrings. */
#define _YSTR_DEFAULT_SIZE	4096

/*! @define _YSTR_100MB Size of a 100 MB buffer. */
#define _YSTR_100MB	(100 * MB)

/*! @define _YSTR_ROUNDSIZE Round a size to 8KB bound. */
#define _YSTR_ROUNDSIZE(s)	((s) + ((s) % 8192))

/*! @define _YSTR_SIZE Compute the size of a new ystring's buffer. */
#define _YSTR_SIZE(s)	_YSTR_ROUNDSIZE(!(s) ? _YSTR_DEFAULT_SIZE : \
			                ((s) > GB ? ((s) + GB) : \
			                 ((s) > _YSTR_100MB ? ((s) + _YSTR_100MB) : ((s) * 2))))

/*! @define _YSTR_HEAD Get a pointer to a ystring's header, from a pointer to the ystring's data. */
#define _YSTR_HEAD(p)	((ystr_head_t*)((p) - sizeof(ystr_head_t)))

/* ********** PRIVATE STRUCTURES AND TYPES ************** */
/*!
 * @struct	ystr_head_s
 *		Structure used for the head of ystrings.
 * @field	total	Total size of the ystring.
 * @field	used	Used size of the ystring.
 */
struct ystr_head_s {
	size_t total;
	size_t used;
};

/*! @typedef ystr_head_t See ystr_head_s structure. */
typedef struct ystr_head_s ystr_head_t;

/* ********** PRIVATE FUNCTION DECLARATIONS ************* */
static ystr_t _ys_new(const char *s);
static void _ys_free(ystr_t *s);
static void _ys_trunc(ystr_t s);
static bool _ys_setsz(ystr_t *s, size_t sz);
static size_t _ys_len(const ystr_t s);
static bool _ys_append(ystr_t *dest, const char *src);
static bool _ys_prepend(ystr_t *dest, const char *src);
static bool _ys_nappend(ystr_t *dest, const char *src, size_t n);
static bool _ys_nprepend(ystr_t *dest, const char *src, size_t n);
static ystr_t _ys_dup(const ystr_t s);
static char *_ys_string(const ystr_t s);
static ystr_t _ys_concat(const char *s1, const char *s2);
static void _ys_ltrim(ystr_t s);
static void _ys_rtrim(ystr_t s);
static void _ys_trim(ystr_t s);
static char _ys_lshift(ystr_t s);
static char _ys_rshift(ystr_t s);
static bool _ys_putc(ystr_t *s, char c);
static bool _ys_addc(ystr_t *s, char c);
static void _ys_upcase(char *s);
static void _ys_lowcase(char *s);
static bool _ys_printf(ystr_t *s, char *format, ...);
static bool _ys_vprintf(ystr_t *s, char *format, va_list args);
static ystr_t _ys_str2hexa(const char *str);
static ystr_t _ys_subs(const char *orig, const char *from, const char *to);
static ystr_t _ys_casesubs(const char *orig, const char *from, const char *to);
static ystr_t _str2xmlentity(char *str);
static ystr_t _xmlentity2str(char *str);

/*!
 * @function	_ys_new
 *		Create a new ystring.
 * @param	s	Original string that will be copied in the ystring.
 *			Could be NULL to create an empty (but allocated) ystring.
 * @return	A pointer to the created ystring.
 */
static ystr_t _ys_new(const char *s) {
	char *res;
	ystr_head_t *y;
	size_t strsz, totalsz;

	// define the needed size
	strsz = !s ? 0 : strlen(s);
	totalsz = _YSTR_SIZE(strsz + 1);
	// allocate memory
	if (!(res = (char*)_ymalloc(totalsz + sizeof(ystr_head_t))))
		return (NULL);
	// set pointers
	y = (ystr_head_t*)res;
	res += sizeof(ystr_head_t);
	// set sizes in the header
	y->total = totalsz;
	y->used = strsz;
	// copy string data
	if (!strsz)
		*res = '\0';
	else
		strcpy(res, s);
	return ((ystr_t)res);
}
/*!
 * @function	_ys_free
 *		Delete an existing ystring.
 * @param	s	A pointer to the ystring.
 */
static void _ys_free(ystr_t *s) {
	ystr_head_t *y;

	if (!s || !*s)
		return;
	y = _YSTR_HEAD(*s);
	_yfree(y);
	*s = NULL;
}
/*!
 * @function	_ys_trunc
 *		Truncate an existing ystring. The allocated memory doesn't change.
 * @param	s	The ystring.
 */
static void _ys_trunc(ystr_t s) {
	ystr_head_t *y;

	if (!s)
		return;
	y = _YSTR_HEAD(s);
	*s = '\0';
	y->used = 0;
}
/*!
 * @function	_ys_setsz
 *		Set the minimum size of a ystring.
 * @param	s	A pointer to the ystring.
 * @param	sz	The minimum size for this ystring.
 * @return	false if an error occurs, true otherwise.
 */
static bool _ys_setsz(ystr_t *s, size_t sz) {
	ystr_head_t *y, *ny;
	size_t totalsz;
	char *ns;

	if (!s)
		return (false);
	y = _YSTR_HEAD(*s);
	if (sz <= y->total)
		return (true);
	totalsz = _YSTR_SIZE(sz + 1);
	if (!(ns = (char*)_ymalloc(totalsz + sizeof(ystr_head_t))))
		return (false);
	ny = (ystr_head_t*)ns;
	ns += sizeof(ystr_head_t);
	ny->total = totalsz;
	ny->used = y->used;
	memcpy(ns, s, y->used + 1);
	_yfree(y);
	*s = ns;
	return (true);
}
/*!
 * @function	_ys_len
 *		Return the length of a ystring.
 * @param	s	The ystring.
 * @return	The ystring's length.
 */
static size_t _ys_len(const ystr_t s) {
	if (!s)
		return (0);
	return (_YSTR_HEAD(s)->used);
}
/*!
 * @function	_ys_append
 *		Concatenate a character string at the end of a ystring.
 * @param	dest	A pointer to the ystring.
 * @param	src	A pointer to the character string.
 * @return	false if an error occurs, true otherwise.
 */
static bool _ys_append(ystr_t *dest, const char *src) {
	size_t srcsz, strsz, totalsz;
	ystr_head_t *y, *ny;
	char *ns;

	if (!src || !(srcsz = strlen(src)))
		return (true);
	if (!*dest) {
		*dest = _ys_new(src);
		return (true);
	}
	y = _YSTR_HEAD(*dest);
	if ((y->used + 1 + srcsz) <= y->total) {
		memcpy(*dest + y->used, src, srcsz + 1);
		y->used += srcsz;
		return (true);
	}
	strsz = y->used + srcsz;
	totalsz = _YSTR_SIZE(strsz + 1);
	if (!(ns = (char*)_ymalloc(totalsz + sizeof(ystr_head_t))))
		return (0);
	ny = (ystr_head_t*)ns;
	ns += sizeof(ystr_head_t);
	ny->total = totalsz;
	ny->used = strsz;
	memcpy(ns, *dest, y->used);
	memcpy(ns + y->used, src, srcsz + 1);
	_yfree(y);
	*dest = ns;
	return (true);
}
/*!
 * @function	_ys_prepend
 *		Concatenate a character string at the begining of a ystring.
 * @param	dest	A pointer to the ystring.
 * @param	src	A pointer to the character string.
 * @return	false if an error occurs, true otherwise.
 */
static bool _ys_prepend(ystr_t *dest, const char *src) {
	size_t srcsz, strsz, totalsz;
	ystr_head_t *y, *ny;
	char *ns;

	if (!src || !(srcsz = strlen(src)))
		return (true);
	if (!*dest) {
		*dest = _ys_new(src);
		return (true);
	}
	y = _YSTR_HEAD(*dest);
	if ((y->used + 1 + srcsz) <= y->total) {
		char *pt1, *pt2;

		for (pt1 = *dest + y->used, pt2 = pt1 + srcsz;
		     pt1 >= *dest; --pt1, --pt2) {
			*pt2 = *pt1;
		}
		memcpy(*dest, src, srcsz);
		y->used += srcsz;
		return (true);
	}
	strsz = y->used + srcsz;
	totalsz = _YSTR_SIZE(strsz);
	if (!(ns = (char*)_ymalloc(totalsz + sizeof(ystr_head_t))))
		return (false);
	ny = (ystr_head_t*)ns;
	ns += sizeof(ystr_head_t);
	ny->total = totalsz;
	ny->used = strsz;
	memcpy(ns, src, srcsz);
	memcpy(ns + srcsz, *dest, y->used + 1);
	_yfree(y);
	*dest = ns;
	return (true);
}
/*!
 * @function	_ys_nappend
 *		Concatenate a given number of characters from a
 *		character string to an ystring.
 * @param	dest	A pointer to the ystring.
 * @param	src	A pointer to the character string.
 * @param	n	The number of characters to copy.
 * @return	false if an error occurs, true otherwise.
 */
static bool _ys_nappend(ystr_t *dest, const char *src, size_t n) {
	size_t strsz, totalsz;
	ystr_head_t *y, *ny;
	char *ns;

	if (!src || !n)
		return (true);
	if (!*dest) {
		*dest = _ys_new(src);
		return (true);
	}
	y = _YSTR_HEAD(*dest);
	if ((y->used + 1 + n) <= y->total) {
		strncpy(*dest + y->used, src, n);
		y->used += n;
		(*dest)[y->used] = '\0';
		return (true);
	}
	strsz = y->used + n;
	totalsz = _YSTR_SIZE(strsz);
	if (!(ns = (char*)_ymalloc(totalsz + sizeof(ystr_head_t))))
		return (false);
	ny = (ystr_head_t*)ns;
	ns += sizeof(ystr_head_t);
	ny->total = totalsz;
	ny->used = strsz;
	strcpy(ns, *dest);
	strncpy(ns + y->used, src, n);
	ns[ny->used] = '\0';
	_yfree(y);
	*dest = ns;
	return (true);
}
/*!
 * @function	_ys_nprepend
 *		Concatenate a given number of characters from a
 *		character string at thbegining of an ystring.
 * @param	dest	A pointer to the ystring.
 * @param	src	A pointer to the character string.
 * @param	n	The number of characters to copy.
 * @return	0 if an error occurs, 1 otherwise.
 */
static bool _ys_nprepend(ystr_t *dest, const char *src, size_t n) {
	size_t strsz, totalsz;
	ystr_head_t *y, *ny;
	char *ns;

	if (!src || !n)
		return (true);
	if (!*dest) {
		*dest = _ys_new(src);
		return (true);
	}
	n = (strlen(src) < n) ? strlen(src) : n;
	y = _YSTR_HEAD(*dest);
	if ((y->used + 1 + n) <= y->total) {
		char *pt1, *pt2;

		for (pt1 = *dest + y->used, pt2 = pt1 + n;
		     pt1 >= *dest; --pt1, --pt2) {
			*pt2 = *pt1;
		}
		memcpy(*dest, src, n);
		y->used += n;
		return (1);
	}
	strsz = y->used + n;
	totalsz = _YSTR_SIZE(strsz);
	if (!(ns = (char*)_ymalloc(totalsz + sizeof(ystr_head_t))))
		return (false);
	ny = (ystr_head_t*)ns;
	ns += sizeof(ystr_head_t);
	ny->total = totalsz;
	ny->used = strsz;
	memcpy(ns, src, n);
	memcpy(ns + n, *dest, y->used + 1);
	_yfree(y);
	*dest = ns;
	return (0);
}
/*!
 * @function	_ys_dup
 *		Duplicate a ystring.
 * @param	s	The ystring.
 * @return	The new ystring.
 */
static ystr_t _ys_dup(const ystr_t s) {
	ystr_head_t *y, *ny;
	char *ns;

	if (!s)
		return (_ys_new(NULL));
	y = _YSTR_HEAD(s);
	if (!(ns = (char*)_ymalloc(y->total + sizeof(ystr_head_t))))
		return (ns);
	ny = (ystr_head_t*)ns;
	ns += sizeof(ystr_head_t);
	ny->total = y->total;
	ny->used = y->used;
	memcpy(ns, s, y->used);
	ns[y->used] = '\0';
	return ((ystr_t)ns);
}
/*!
 * @function	_ys_string
 *		Create a copy of a ystring. The copy is a simple
 *		(char*) string, not bufferized.
 * @param	s	The ystring.
 * @return	A pointer to the created string, or NULL if an error occurs.
 */
static char *_ys_string(const ystr_t s) {
	ystr_head_t *y;
	char *res;

	if (!s)
		return (NULL);
	y = _YSTR_HEAD(s);
	if (!(res = (char*)_ymalloc(y->used + 1)))
		return (NULL);
	return (memcpy(res, s, y->used + 1));
}
/*!
 * @function	_ys_concat
 *		Concatenate 2 character strings to create a ystring.
 * @param	s1	A pointer to the first character string.
 * @param	s2	A pointer to the second character string.
 * @return	A pointer to the new ystring.
 */
static ystr_t _ys_concat(const char *s1, const char *s2) {
	char *ns;

	ns = _ys_new(s1);
	_ys_append(&ns, s2);
	return ((ystr_t)ns);
}
/*!
 * @function	_ys_ltrim
 *		Remove all spaces at the beginning of a ystring.
 * @param	s	The ystring.
 */
static void _ys_ltrim(ystr_t s) {
	ystr_head_t *y;
	char *pt;
	size_t offset = 0;

	if (!s)
		return;
	y = _YSTR_HEAD(s);
	for (pt = s; IS_SPACE(*pt); ++pt) {
		++offset;
	}
	if (!offset)
		return;
	y->used -= offset;
	memmove(s, s + offset, y->used + 1);
}
/*!
 * @function	_ys_rtrim
 *		Remove all spaces at the end of a ystring.
 * @param	s	The ystring.
 */
static void _ys_rtrim(ystr_t s) {
	ystr_head_t *y;
	char *pt;
	size_t initsz;

	y = _YSTR_HEAD(s);
	initsz = y->used;
	for (pt = s + y->used - 1; IS_SPACE(*pt); --pt) {
		if (pt == s) {
			*pt = '\0';
			y->used = 0;
			return;
		}
		y->used--;
	}
	if (initsz != y->used)
		*(pt + 1) = '\0';
}
/*!
 * @function	_ys_trim
 *		Remove all spaces at the beginning and the end of a ystring.
 * @param	s	The ytring.
 */
static void _ys_trim(ystr_t s) {
	_ys_ltrim(s);
	_ys_rtrim(s);
}
/*!
 * @function	_ys_lshift
 *		Remove the first character of a ystring.
 * @param	s	The ystring.
 * @return	The removed character.
 */
static char _ys_lshift(ystr_t s) {
	char c;

	if (!s || !*s)
		return ('\0');
	c = *s;
	// write a space onto the first character, and then call ltrim
	*s = ' ';
	_ys_ltrim(s);
	return (c);
}
/*!
 * @function	_ys_rshift
 *		Remove the last character of a ystring.
 * @param	s	The ystring.
 * @return	The removed character.
 */
static char _ys_rshift(ystr_t s) {
	ystr_head_t *y;
	char c;

	if (!s || !*s)
		return ('\0');
	y = _YSTR_HEAD(s);
	c = *(s + y->used - 1);
	*(s + y->used - 1) = '\0';
	y->used--;
	return (c);
}
/*!
 * @function	_ys_putc
 *		Add a character at the beginning of a ystring.
 * @param	s	A pointer to the ystring.
 * @param	c	The character to add.
 * @return	false if an error occurs, true otherwise.
 */
static bool _ys_putc(ystr_t *s, char c) {
	ystr_head_t *y, *ny;
	char *pt1, *pt2, *ns;
	size_t totalsz;

	if (c == '\0')
		return (true);
	if (!*s) {
		char tc[2] = {'\0', '\0'};
		tc[0] = c;
		*s = _ys_new(tc);
		return (true);
	}
	y = _YSTR_HEAD(*s);
	if (y->total >= (y->used + 2)) {
		for (pt1 = *s + y->used, pt2 = pt1 + 1; pt1 >= *s; --pt1, --pt2) {
			*pt2 = *pt1;
		}
		**s = c;
		y->used++;
		return (true);
	}
	totalsz = _YSTR_SIZE(y->used + 2);
	if (!(ns = (char*)_ymalloc(totalsz + sizeof(ystr_head_t))))
		return (false);
	ny = (ystr_head_t*)ns;
	ns += sizeof(ystr_head_t);
	ny->total = totalsz;
	ny->used = y->used + 1;
	*ns = c;
	memcpy(ns + 1, *s, y->used + 1);
	_yfree(y);
	*s = ns;
	return (true);
}
/*!
 * @function	_ys_addc
 *		Add a character at the end of a ystring.
 * @param	s	A pointer to the ystring.
 * @param	c	The character to add.
 * @return	false if an error occurs, true otherwise.
 */
static bool _ys_addc(ystr_t *s, char c) {
	char tc[2] = {'\0', '\0'};

	tc[0] = c;
	return (_ys_append(s, tc));
}
/*!
 * @function	_ys_upcase
 *		Convert all characters of a character string to upper
 *		case.
 * @param	s	A pointer to the ystring.
 */
static void _ys_upcase(char *s) {
	if (!s)
		return;
	for (; *s; ++s) {
		if (*s >= 'a' && *s <= 'z')
			*s = 'A' + (*s - 'a');
	}
}
/*!
 * @function	_ys_lowcase
 *		Convert all characters of a character string to lower
 *		case.
 * @param	s	A pointer to the ystring.
 */
static void _ys_lowcase(char *s) {
	if (!s)
		return;
	for (; *s; ++s) {
		if (*s >= 'A' && *s <= 'Z')
			*s = 'a' + (*s - 'A');
	}
}
/*!
 * @function	_ys_printf
 *		Write inside a ystring using formatted arguments. The
 *		ystring must be long enough (use _ys_setsz() before),
 *		otherwise the resulting string will be truncate.
 * @param	s	A pointer to the ystring.
 * @param	format	Format string (like in printf()).
 * @param	...	Variable argument list.
 * @return	false if an error occurs, true otherwise.
 */
static bool _ys_printf(ystr_t *s, char *format, ...) {
	va_list p_list;
	bool res;

	va_start(p_list, format);
	res = _ys_vprintf(s, format, p_list);
	va_end(p_list);
	return (res);
}
/*!
 * @function	_ys_vprintf
 *		Same as _ys_printf(), but the variable arguments are given
 *		trough a va_list.
 * @param	s	A pointer to the ystring.
 * @param	format	Format string (like in printf()).
 * @param	args	Variable argument list.
 * @return	false if an error occurs, true otherwise.
 */
static bool _ys_vprintf(ystr_t *s, char *format, va_list args) {
	ystr_head_t *y;
	int i;

	if (!s || !*s || !format)
		return (false);
	y = _YSTR_HEAD(*s);
	if ((i = vsnprintf(*s, y->total, format, args)) == -1)
		**s = '\0';
	y->used = (i == -1) ? 0 : i;
	return ((i == -1) ? false : true);
}
/*!
 * @function	_ys_str2hexa
 *		Convert a character string to the hexadecimal representation
 *		of this string.
 * @param	str	Character string that must be converted.
 * @return	A ystring that contains the converted string, or NULL.
 */
static ystr_t _ys_str2hexa(const char *str) {
	char h[3] = {'\0', '\0', '\0'};
	const char *pt;
	ystr_t ys;

	if (!str || !(ys = _ys_new(NULL)))
		return (NULL);
	for (pt = str; *pt; ++pt) {
		snprintf(h, 3, "%x", *pt);
		_ys_append(&ys, h);
	}
	return (ys);
}
/*!
 * @function    _ys_subs
 *              Substitute a string by another, inside a charater string.
 * @param       orig    The original character string.
 * @param       from    The string to substitute.
 * @param       to      The substitution string.
 * @return      A ystring that contains the substituted string, or NULL.
 */
static ystr_t _ys_subs(const char *orig, const char *from, const char *to) {
	ystr_t ys;
	const char *pt;
	size_t from_len;

	if (!orig || !(ys = _ys_new(NULL)))
		return (NULL);
	from_len = (from) ? strlen(from) : 0;
	for (pt = orig; *pt; ++pt) {
		if (from_len && !strncmp(from, pt, from_len)) {
			_ys_append(&ys, to);
			pt = pt + from_len - 1;
		} else
			_ys_addc(&ys, *pt);
	}
	return (ys);
}
/*!
 * @function    _ys_casesubs
 *              Substitute a string by another, in a case-insensitive manner.
 * @param       orig    The original character string.
 * @param       from    The string to substitute.
 * @param       to      The substitution string.
 * @return      A ystring that contains the substituted string, or NULL.
 */
static ystr_t _ys_casesubs(const char *orig, const char *from, const char *to) {
	ystr_t ys;
	const char *pt;
	size_t from_len;

	if (!orig || !(ys = _ys_new(NULL)))
		return (NULL);
	from_len = (from) ? strlen(from) : 0;
	for (pt = orig; *pt; ++pt) {
		if (from_len && !strncasecmp(from, pt, from_len)) {
			_ys_append(&ys, to);
			pt = pt + from_len - 1;
		} else
			_ys_addc(&ys, *pt);
	}
	return (ys);
}
/*!
 * @function	_str2xmlentity
 *		Convert a character string into a ystring where each XML special
 *		characters are replaced by their corresponding XML entities.
 * @param	str	Character string that must be converted.
 * @return	The converted string, or NULL.
 */
static ystr_t _str2xmlentity(char *str) {
	char *pt;
	ystr_t res;

	if (!str || !(res = _ys_new(NULL)))
		return (NULL);
	for (pt = str; *pt; ++pt) {
		if (*pt == LT)
			_ys_append(&res, "&lt;");
		else if (*pt == GT)
			_ys_append(&res, "&gt;");
		else if (*pt == DQUOTE)
			_ys_append(&res, "&quot;");
		else if (*pt == QUOTE)
			_ys_append(&res, "&apos;");
		else if (*pt == AMP)
			_ys_append(&res, "&amp;");
		else
			_ys_addc(&res, *pt);
	}
	return (res);
}
/*!
 * @function	_xmlentity2str
 *		Convert a string into a ystring where XML entities are replaced by
 *		their XML special characters.
 * @param	str	The XML encoded character string.
 * @return	The unconverted string, or NULL.
 */
static ystr_t _xmlentity2str(char *str) {
	ystr_t res;
	char *pt, *pt2;
	int i;
	
	if (!str || !(res = _ys_new("")))
		return (NULL);
	for (pt = str; *pt; pt += i) {
		if (*pt != AMP) {
			_ys_addc(&res, *pt);
			i = 1;
		} else if (!strncmp(pt, "&amp;", (i = strlen("&amp;"))))
			_ys_addc(&res, AMP);
		else if (!strncmp(pt, "&lt;", (i = strlen("&lt;"))))
			_ys_addc(&res, LT);
		else if (!strncmp(pt, "&gt;", (i = strlen("&gt;"))))
			_ys_addc(&res, GT);
		else if (!strncmp(pt, "&quot;", (i = strlen("&quot;"))))
			_ys_addc(&res, DQUOTE);
		else if (!strncmp(pt, "&apos;", (i = strlen("&apos;"))))
			_ys_addc(&res, QUOTE);
		else if (*(pt + 1) == HASH && (pt2 = strchr(pt + 2, ';'))) {
			_ys_addc(&res, atoi(pt + 2));
			i = (pt2 - pt) + 1;
		} else {
			_ys_addc(&res, *pt);
			i = 1;
		}
	}
	return (res);
}

