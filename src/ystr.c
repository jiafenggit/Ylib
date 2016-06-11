/* ********************************************************
 * *                       YSTR                           *
 * ******************************************************** */

#include "ylib.h"

/* *********** PRIVATE DEFINITIONS AND MACROS ************ */
/*! @define _YSTR_DEFAULT_SIZE Default size for ystrings. */
#define _YSTR_DEFAULT_SIZE	4096

/*! @define _YSTR_100MB Size of a 100 MB buffer. */
#define _YSTR_100MB	(100 * MB)

/*! @define _YSTR_ROUNDSIZE Round a size to 8KB bound. */
#define _YSTR_ROUNDSIZE(s)	((s) == _YSTR_DEFAULT_SIZE ? (s) : ((s) + ((s) % 8192)))

/*! @define _YSTR_SIZE Compute the size of a new ystring's buffer. */
#define _YSTR_SIZE(s)	_YSTR_ROUNDSIZE(!(s) ? _YSTR_DEFAULT_SIZE : \
			                (s) > GB ? ((s) + GB) : \
			                (s) > _YSTR_100MB ? ((s) + _YSTR_100MB) : \
			                (s) < _YSTR_DEFAULT_SIZE ? _YSTR_DEFAULT_SIZE : \
			                ((s) * 2))

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

/* ********** FUNCTIONS ************* */

ystr_t ys_new(const char *s) {
	char *res;
	ystr_head_t *y;
	size_t strsz, totalsz;

	// define the needed size
	strsz = !s ? 0 : strlen(s);
	totalsz = _YSTR_SIZE(strsz + 1);
	// allocate memory
	if (!(res = (char*)ymalloc(totalsz + sizeof(ystr_head_t))))
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

void ys_free(ystr_t *s) {
	ystr_head_t *y;

	if (!s || !*s)
		return;
	y = _YSTR_HEAD(*s);
	yfree(y);
	*s = NULL;
}

void ys_trunc(ystr_t s) {
	ystr_head_t *y;

	if (!s)
		return;
	y = _YSTR_HEAD(s);
	*s = '\0';
	y->used = 0;
}

bool ys_setsz(ystr_t *s, size_t sz) {
	ystr_head_t *y, *ny;
	size_t totalsz;
	char *ns;

	if (!s)
		return (false);
	y = _YSTR_HEAD(*s);
	if (sz <= y->total)
		return (true);
	totalsz = _YSTR_SIZE(sz + 1);
	if (!(ns = (char*)ymalloc(totalsz + sizeof(ystr_head_t))))
		return (false);
	ny = (ystr_head_t*)ns;
	ns += sizeof(ystr_head_t);
	ny->total = totalsz;
	ny->used = y->used;
	memcpy(ns, s, y->used + 1);
	yfree(y);
	*s = ns;
	return (true);
}

size_t ys_len(const ystr_t s) {
	if (!s)
		return (0);
	return (_YSTR_HEAD(s)->used);
}

bool ys_append(ystr_t *dest, const char *src) {
	size_t srcsz, strsz, totalsz;
	ystr_head_t *y, *ny;
	char *ns;

	if (!src || !(srcsz = strlen(src)))
		return (true);
	if (!*dest) {
		*dest = ys_new(src);
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
	if (!(ns = (char*)ymalloc(totalsz + sizeof(ystr_head_t))))
		return (0);
	ny = (ystr_head_t*)ns;
	ns += sizeof(ystr_head_t);
	ny->total = totalsz;
	ny->used = strsz;
	memcpy(ns, *dest, y->used);
	memcpy(ns + y->used, src, srcsz + 1);
	yfree(y);
	*dest = ns;
	return (true);
}

bool ys_prepend(ystr_t *dest, const char *src) {
	size_t srcsz, strsz, totalsz;
	ystr_head_t *y, *ny;
	char *ns;

	if (!src || !(srcsz = strlen(src)))
		return (true);
	if (!*dest) {
		*dest = ys_new(src);
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
	if (!(ns = (char*)ymalloc(totalsz + sizeof(ystr_head_t))))
		return (false);
	ny = (ystr_head_t*)ns;
	ns += sizeof(ystr_head_t);
	ny->total = totalsz;
	ny->used = strsz;
	memcpy(ns, src, srcsz);
	memcpy(ns + srcsz, *dest, y->used + 1);
	yfree(y);
	*dest = ns;
	return (true);
}

bool ys_nappend(ystr_t *dest, const char *src, size_t n) {
	size_t strsz, totalsz;
	ystr_head_t *y, *ny;
	char *ns;

	if (!src || !n)
		return (true);
	if (!*dest) {
		*dest = ys_new(src);
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
	if (!(ns = (char*)ymalloc(totalsz + sizeof(ystr_head_t))))
		return (false);
	ny = (ystr_head_t*)ns;
	ns += sizeof(ystr_head_t);
	ny->total = totalsz;
	ny->used = strsz;
	strcpy(ns, *dest);
	strncpy(ns + y->used, src, n);
	ns[ny->used] = '\0';
	yfree(y);
	*dest = ns;
	return (true);
}

bool ys_nprepend(ystr_t *dest, const char *src, size_t n) {
	size_t strsz, totalsz;
	ystr_head_t *y, *ny;
	char *ns;

	if (!src || !n)
		return (true);
	if (!*dest) {
		*dest = ys_new(src);
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
	if (!(ns = (char*)ymalloc(totalsz + sizeof(ystr_head_t))))
		return (false);
	ny = (ystr_head_t*)ns;
	ns += sizeof(ystr_head_t);
	ny->total = totalsz;
	ny->used = strsz;
	memcpy(ns, src, n);
	memcpy(ns + n, *dest, y->used + 1);
	yfree(y);
	*dest = ns;
	return (0);
}

ystr_t ys_dup(const ystr_t s) {
	ystr_head_t *y, *ny;
	char *ns;

	if (!s)
		return (ys_new(NULL));
	y = _YSTR_HEAD(s);
	if (!(ns = (char*)ymalloc(y->total + sizeof(ystr_head_t))))
		return (ns);
	ny = (ystr_head_t*)ns;
	ns += sizeof(ystr_head_t);
	ny->total = y->total;
	ny->used = y->used;
	memcpy(ns, s, y->used);
	ns[y->used] = '\0';
	return ((ystr_t)ns);
}

char *ys_string(const ystr_t s) {
	ystr_head_t *y;
	char *res;

	if (!s)
		return (NULL);
	y = _YSTR_HEAD(s);
	if (!(res = (char*)ymalloc(y->used + 1)))
		return (NULL);
	return (memcpy(res, s, y->used + 1));
}

ystr_t ys_concat(const char *s1, const char *s2) {
	char *ns;

	ns = ys_new(s1);
	ys_append(&ns, s2);
	return ((ystr_t)ns);
}

void ys_ltrim(ystr_t s) {
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

void ys_rtrim(ystr_t s) {
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

void ys_trim(ystr_t s) {
	ys_ltrim(s);
	ys_rtrim(s);
}

char ys_lshift(ystr_t s) {
	char c;

	if (!s || !*s)
		return ('\0');
	c = *s;
	// write a space onto the first character, and then call ltrim
	*s = ' ';
	ys_ltrim(s);
	return (c);
}

char ys_rshift(ystr_t s) {
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

bool ys_putc(ystr_t *s, char c) {
	ystr_head_t *y, *ny;
	char *pt1, *pt2, *ns;
	size_t totalsz;

	if (c == '\0')
		return (true);
	if (!*s) {
		char tc[2] = {'\0', '\0'};
		tc[0] = c;
		*s = ys_new(tc);
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
	if (!(ns = (char*)ymalloc(totalsz + sizeof(ystr_head_t))))
		return (false);
	ny = (ystr_head_t*)ns;
	ns += sizeof(ystr_head_t);
	ny->total = totalsz;
	ny->used = y->used + 1;
	*ns = c;
	memcpy(ns + 1, *s, y->used + 1);
	yfree(y);
	*s = ns;
	return (true);
}

bool ys_addc(ystr_t *s, char c) {
	char tc[2] = {'\0', '\0'};

	tc[0] = c;
	return (ys_append(s, tc));
}

void ys_upcase(char *s) {
	if (!s)
		return;
	for (; *s; ++s) {
		if (*s >= 'a' && *s <= 'z')
			*s = 'A' + (*s - 'a');
	}
}

void ys_lowcase(char *s) {
	if (!s)
		return;
	for (; *s; ++s) {
		if (*s >= 'A' && *s <= 'Z')
			*s = 'a' + (*s - 'A');
	}
}

bool ys_printf(ystr_t *s, char *format, ...) {
	va_list p_list;
	bool res;

	va_start(p_list, format);
	res = ys_vprintf(s, format, p_list);
	va_end(p_list);
	return (res);
}

bool ys_vprintf(ystr_t *s, char *format, va_list args) {
	ystr_head_t *y;
	int i;

	if (!s || !*s || !format)
		return (false);
	y = _YSTR_HEAD(*s);
	if ((i = vsnprintf(*s, y->total, format, args)) == -1) {
		**s = '\0';
	}
	y->used = (i == -1) ? 0 : i;
	return ((i == -1) ? false : true);
}

ystr_t ys_str2hexa(const char *str) {
	char h[3] = {'\0', '\0', '\0'};
	const char *pt;
	ystr_t ys;

	if (!str || !(ys = ys_new(NULL)))
		return (NULL);
	for (pt = str; *pt; ++pt) {
		snprintf(h, 3, "%x", *pt);
		ys_append(&ys, h);
	}
	return (ys);
}

ystr_t ys_subs(const char *orig, const char *from, const char *to) {
	ystr_t ys;
	const char *pt;
	size_t from_len;

	if (!orig || !(ys = ys_new(NULL)))
		return (NULL);
	from_len = (from) ? strlen(from) : 0;
	for (pt = orig; *pt; ++pt) {
		if (from_len && !strncmp(from, pt, from_len)) {
			ys_append(&ys, to);
			pt = pt + from_len - 1;
		} else
			ys_addc(&ys, *pt);
	}
	return (ys);
}

ystr_t ys_casesubs(const char *orig, const char *from, const char *to) {
	ystr_t ys;
	const char *pt;
	size_t from_len;

	if (!orig || !(ys = ys_new(NULL)))
		return (NULL);
	from_len = (from) ? strlen(from) : 0;
	for (pt = orig; *pt; ++pt) {
		if (from_len && !strncasecmp(from, pt, from_len)) {
			ys_append(&ys, to);
			pt = pt + from_len - 1;
		} else
			ys_addc(&ys, *pt);
	}
	return (ys);
}

ystr_t str2xmlentity(char *str) {
	char *pt;
	ystr_t res;

	if (!str || !(res = ys_new(NULL)))
		return (NULL);
	for (pt = str; *pt; ++pt) {
		if (*pt == LT)
			ys_append(&res, "&lt;");
		else if (*pt == GT)
			ys_append(&res, "&gt;");
		else if (*pt == DQUOTE)
			ys_append(&res, "&quot;");
		else if (*pt == QUOTE)
			ys_append(&res, "&apos;");
		else if (*pt == AMP)
			ys_append(&res, "&amp;");
		else
			ys_addc(&res, *pt);
	}
	return (res);
}

ystr_t xmlentity2str(char *str) {
	ystr_t res;
	char *pt, *pt2;
	int i;
	
	if (!str || !(res = ys_new("")))
		return (NULL);
	for (pt = str; *pt; pt += i) {
		if (*pt != AMP) {
			ys_addc(&res, *pt);
			i = 1;
		} else if (!strncmp(pt, "&amp;", (i = strlen("&amp;"))))
			ys_addc(&res, AMP);
		else if (!strncmp(pt, "&lt;", (i = strlen("&lt;"))))
			ys_addc(&res, LT);
		else if (!strncmp(pt, "&gt;", (i = strlen("&gt;"))))
			ys_addc(&res, GT);
		else if (!strncmp(pt, "&quot;", (i = strlen("&quot;"))))
			ys_addc(&res, DQUOTE);
		else if (!strncmp(pt, "&apos;", (i = strlen("&apos;"))))
			ys_addc(&res, QUOTE);
		else if (*(pt + 1) == HASH && (pt2 = strchr(pt + 2, ';'))) {
			ys_addc(&res, atoi(pt + 2));
			i = (pt2 - pt) + 1;
		} else {
			ys_addc(&res, *pt);
			i = 1;
		}
	}
	return (res);
}

