# #####################################################################
#                              Makefile
# #####################################################################

# Library's static name (libNAME.a)
NAME	=	liby.a

# Name of source files (names.c)
SRC       =	ylib.c

# Name of header files (names.h)
HEADS     =	ylib.h

CAT_H	=	ydefs.h	\
		ymem.h	\
		ystr.h	\
		yvect.h \
		yhash.h \
		ymap.h

CAT_C	=	ymem.c	\
		ystr.c	\
		yvect.c \
		yhash.c \
		ymap.c

# #####################################################################

# Paths to header files
IPATH   =       -I.
# Path to libraries and lib's names
LDPATH  =       -L.
# Compiler options
EXEOPT  =       -O2 # -g for debug

# #####################################################################

CC      =	gcc
RM      =	/bin/rm -f
OBJS    =	$(SRC:.c=.o)

# Objects compilation options
CFLAGS  =	-ansi -std=c99 -pedantic -Wall -Wextra -Wmissing-prototypes \
		  -Wno-long-long -Wno-pointer-arith $(IPATH) -D_GNU_SOURCE \
		  -D_LARGEFILE_SOURCE -D_THREAD_SAFE -fPIC
CFLAGS_CYGWIN =	-Wall -Wmissing-prototypes -Wno-long-long $(IPATH) \
		  -D_GNU_SOURCE -D_LARGEFILE_SOURCE -D_THREAD_SAFE

# #####################################################################

$(NAME): gen_sources $(OBJS) $(SRC)
	ar -r $(NAME) $(OBJS)

gen_sources: $(CAT_C) $(CAT_H)
	@echo "Generate ylib.c file"
	@cp _ylib_begin.c ylib.c
	@for f in $(CAT_C); do \
		echo -n "#line 1 \"" >> ylib.c; \
		echo -n $$f >> ylib.c; \
		echo "\"" >> ylib.c; \
		cat $$f >> ylib.c; \
	 done
	@cat _ylib_end.c >> ylib.c
	@echo "Generate ylib.h file"
	@cat _ylib_begin.h $(CAT_H) _ylib_end.h > ylib.h

test: $(NAME) main.c
	$(CC) main.c -L. -ly -Wl,-rpath -Wl,'$$ORIGIN' -o test

cygwin: $(SRC)
	$(CC) $(CFLAGS_CYGWIN) -c $(SRC)
	ar -r $(NAME) $(OBJS)
	ranlib $(NAME)
	cp $(NAME) ../

clean:
	$(RM) $(OBJS) $(NAME) $(SRC) $(HEADS) test *~

all: clean $(NAME)

cygall: clean cygwin

doc:	# needs the HeaderBrowser program
	headerbrowser $(HEADS)

docclean:
	$(RM) -r hbresult-html

.c.o:
	$(CC) $(CFLAGS) -c $<

