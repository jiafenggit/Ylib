/* Process this file with the HeaderBrowser tool (http://www.headerbrowser.org)
   to create documentation. */
/*!
 * @header	ydefs.h
 * @abstract	Basic definitions.
 * @discussion	This file contains all basic definitions used by "ylib". That
 *		includes types, characters and numbers definitions and basic
 *		macros.
 * @version	1.0.0 Jun 28 2002
 * @author	Amaury Bouchard <amaury@amaury.net>
 */
#ifndef __YDEFS_H__
#define __YDEFS_H__

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif // __cplusplus || c_plusplus

#include <stdlib.h>

/* checks the C compiler version */
#if defined (__STDC__)
  /*! @define COMPILER_C89 The compiler is compatible with C89. */
# define COMPILER_C89
# if defined(__STDC_VERSION__)
   /*! @define COMPILER_C90 The compiler is compatible with C90. */
#  define COMPILER_C90
#  if (__STDC_VERSION__ >= 199409L)
    /*! @define COMPILER_C94 The compiler is compatible with C94. */
#   define COMPILER_C94
#  endif
#  if (__STDC_VERSION__ >= 199901L)
    /*! @define COMPILER_C99 The compiler is compatible with C99. */
#   define COMPILER_C99
#  endif
# endif
#endif

/**
 * @define	UNUSED
 *		Macro to define unused parameter in a function prototype, to avoid "missing parameter" warning.
 * @link	http://stackoverflow.com/questions/7090998/portable-unused-parameter-macro-used-on-function-signature-for-c-and-c
 */
#ifndef UNUSED
# if defined(__GNUC__)
#  define UNUSED(x) UNUSED_ ## x __attribute__((unused))
# elif defined(__LCLINT__)
#  define UNUSED(x) /*@unused@*/ x
# elif defined(__cplusplus)
#  define UNUSED(x)
# else
#  define UNUSED(x) x
# endif
#endif

/*!
 * @struct	ybin_s
 *		Structure used for binary data transmission.
 * @field	data	Pointer to data itselves.
 * @field	len	Size of data.
 */
struct ybin_s {
	void *data;
	unsigned int len;
};

/*! @typedef ybin_t See struct ybin_s. */
typedef struct ybin_s ybin_t;

/*! @typedef f32_t Define a type for 32 bits floats. */
typedef float f32_t;
/*! @typedef f64_t Define a type for 64 bits floats. */
typedef double f64_t;
/*! @typedef f96_t Define a type for 96 bits floats. */
typedef long double f96_t;

/*! @define TAB Character definition. */
#define	TAB		'\t'
/*! @define LF Character definition. */
#define	LF		'\n'
/*! @define CR Character definition. */
#define CR		'\r'
/*! @define SPACE Character definition. */
#define	SPACE		' '
/*! @define EXCLAM Character definition. */
#define	EXCLAM		'!'
/*! @define INTERROG Character definition. */
#define	INTERROG	'?'
/*! @define DQUOTE Character definition. */
#define	DQUOTE		'\"'
/*! @define QUOTE Character definition. */
#define	QUOTE		'\''
/*! @define BACKQUOTE Character definition. */
#define BACKQUOTE	'`'
/*! @define UNDERSCORE Character definition. */
#define UNDERSCORE	'_'
/*! @define HASH Character definition. */
#define	HASH		'#'
/*! @define DOLLAR Character definition. */
#define DOLLAR		'$'
/*! @define EURO Character definition. */
#define EURO		'€'
/*! @define PERCENT Character definition. */
#define PERCENT		'%'
/*! @define AMP Character definition. */
#define	AMP		'&'
/*! @define PIPE Character definition. */
#define	PIPE		'|'
/*! @define TILDE Character definition. */
#define TILDE		'~'
/*! @define ASTERISK Character definition. */
#define	ASTERISK	'*'
/*! @define PLUS Character definition. */
#define PLUS		'+'
/*! @define MINUS Character definition. */
#define	MINUS		'-'
/*! @define DOT Character definition. */
#define	DOT		'.'
/*! @define COMMA Character definition. */
#define COMMA		','
/*! @define SLASH Character definition. */
#define	SLASH		'/'
/*! @define BACKSLASH Character definition. */
#define BACKSLASH	'\\'
/*! @define COLON Character definition. */
#define	COLON		':'
/*! @define SEMICOLON Character definition. */
#define SEMICOLON	';'
/*! @define LT Character definition. */
#define	LT		'<'
/*! @define EQ Character definition. */
#define	EQ		'='
/*! @define GT Character definition. */
#define	GT		'>'
/*! @define AT Character definition. */
#define	AT		'@'
/*! @define CARET Character definition. */
#define CARET		'^'
/*! @define LPAR Character definition. */
#define	LPAR		'('
/*! @define RPAR Character definition. */
#define	RPAR		')'
/*! @define LSQUAREBRACKET Character definition. */
#define	LSQUAREBRACKET	'['
/*! @define RSQUAREBRACKET Character definition. */
#define	RSQUAREBRACKET	']'
/*! @define LCURLYBRACKET Character definition. */
#define	LCURLYBRACKET	'{'
/*! @define RCURLYBRACKET Character definition. */
#define	RCURLYBRACKET	'}'

/*! @define YES String definition. */
#define YES		"yes"
/*! @define NO String definition. */
#define NO		"no"

/*! @define KB Definition of one KiloByte value. */
#define KB		1024
/*! @define MB Definition of one MegaByte value. */
#define MB		1048576
/*! @define GB Definition of one GigaByte value. */
#define GB		1073741824
/*! @define TB Definition of one TeraByte value. */
#define TB		1099511627776

/*! @define KILO Definition of one Kilo value. */
#define KILO		1000
/*! @define MEGA Definition of one Mega value. */
#define MEGA		1000000
/*! @define GIGA Definition of one Giga value. */
#define GIGA		1000000000
/*! @define TERA Definition of one Tera value. */
#define TERA		1000000000000
/*! @define PETA Definition of one Peta value. */
#define PETA		1000000000000000
/*! @define EXA Definition of one Exa value. */
#define EXA		1000000000000000000
/*! @define ZETTA Definition of one Zetta value. */
#define ZETTA		1000000000000000000000
/*! @define YOTTA Definition of one Yotta value. */
#define YOTTA		1000000000000000000000000

/*! @define IS_SPACE True if the character is a space. */
#define	IS_SPACE(c)	((c) == SPACE || (c) == TAB || (c) == LF || (c) == CR)

/*! @define IS_CHAR True if the character is an ASCII character. */
#define IS_CHAR(c)	(((c) >= 'a' && (c) <= 'z') || ((c) >= 'A' && (c) <= 'Z'))
   
/*! @define IS_NUM True if the character is a numeric digit. */
#define	IS_NUM(c)	((c) >= '0' && (c) <= '9')

/*! @define IS_HEXA True if the character is an hexadecimal digit. */
#define	IS_HEXA(c)	(IS_NUM(c) || ((c) >= 'a' && (c) <= 'f') || ((c) >= 'A' && (c) <= 'F'))

/*! @define ARRAY_SIZE Return the number of elements in an array. */
#define ARRAY_SIZE(a)	(sizeof(a) / sizeof(a[0]))

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif // __cplusplus || c_plusplus

#endif /* __YDEFS_H__ */
