
struct _ylib_namespace {
	struct {
		void *(*malloc)(size_t size);
		void *(*calloc)(size_t nbr, size_t size);
		void (*free)(void *ptr);
	} sys;
	struct {
		ystr_t (*create)(const char *s);
		void (*free)(ystr_t *s);
		void (*trunc)(ystr_t s);
		bool (*setsz)(ystr_t *s, size_t sz);
		size_t (*len)(const ystr_t s);
		bool (*append)(ystr_t *dest, const char *src);
		bool (*prepend)(ystr_t *dest, const char *src);
		bool (*nappend)(ystr_t *dest, const char *src, size_t n);
		bool (*nprepend)(ystr_t *dest, const char *src, size_t n);
		ystr_t (*dup)(const ystr_t s);
		char *(*tostring)(const ystr_t s);
		ystr_t (*concat)(const char *s1, const char *s2);
		void (*ltrim)(ystr_t s);
		void (*rtrim)(ystr_t s);
		void (*trim)(ystr_t s);
		char (*lshift)(ystr_t s);
		char (*rshift)(ystr_t s);
		bool (*putc)(ystr_t *s, char c);
		bool (*addc)(ystr_t *s, char c);
		void (*upcase)(char *s);
		void (*lowcase)(char *s);
		bool (*printf)(ystr_t *s, char *format, ...);
		bool (*vprintf)(ystr_t *s, char *format, va_list args);
		ystr_t (*str2hexa)(const char *str);
		ystr_t (*subs)(const char *orig, const char *from, const char *to);
		ystr_t (*casesubs)(const char *orig, const char *from, const char *to);
		ystr_t (*str2xmlentity)(char *str);
		ystr_t (*xmlentity2str)(char *str);
	} str;
};

#ifndef YLIB_CODE
	extern struct _ylib_namespace y;
#endif /* YLIB_CODE */

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif /* __cplusplus || c_plusplus */

#endif /* __YLIB_H__ */
