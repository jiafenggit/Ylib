# #####################################################################
#                              Makefile
# #####################################################################

# Library's name
SONAME =	liby.so

# Name of source files
SRC =		ymem.c		\
		ystr.c		\
		yvect.c		\
		yvector.c	\
		yhash.c		\
		ymap.c		\
		ylog.c		\
		ylist.c		\
		ysax.c		\
		ydom.c		\
		ydom_xpath.c	\
		yjson.c

# Name of header files
INCLUDES =	ydefs.h		\
		yerror.h	\
		ymem.h		\
		ystr.h		\
		yvect.h		\
		yvector.h	\
		yhash.h		\
		ymap.h		\
		ylog.h		\
		ylist.h		\
		ysax.h		\
		ydom.h		\
		yjson.h

# #####################################################################

# Paths to header files
IPATH   =       -I.
# Path to libraries and lib's names
LDPATH  =       -L. -lm
# Compiler options
EXEOPT  =       -O2 # -g for debug

# #####################################################################

CC      =	gcc
RM      =	/bin/rm -f
OBJS    =	$(SRC:.c=.o)

# Objects compilation options
CFLAGS  =	-ansi -std=c11 -pedantic-errors -Wall -Wextra -Wmissing-prototypes \
		  -Wno-long-long -Wno-pointer-arith -Wno-unused-parameter -D_GNU_SOURCE \
		  -D_LARGEFILE_SOURCE -D_THREAD_SAFE -fPIC $(IPATH)
# Link options
LDFLAGS = -shared -Wl,-soname,$(SONAME)

# #####################################################################

.PHONY: clean all

$(SONAME): $(OBJS) $(SRC)
	$(CC) $(OBJS) $(LDFLAGS) -o $(SONAME)
	cp $(SONAME) ../lib/
	cp $(INCLUDES) ../include/

clean:
	$(RM) $(OBJS) $(SONAME) *~ test
	$(RM) ../lib/$(SONAME)
	cd ../include ; $(RM) $(INCLUDES)

all: clean $(SONAME) test

test: main.c
	$(CC) main.c -L. -ly -lm -Wl,-rpath -Wl,'$$ORIGIN/../lib' -o test

.c.o:
	$(CC) $(CFLAGS) -c $<

