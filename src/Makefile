# #####################################################################
#                              Makefile
# #####################################################################

# Library's name
SONAME =	liby.so

# Name of source files
SRC =		ymem.c		\
		ystr.c		\
		yvect.c		\
		yhash.c		\
		ymap.c		\
		ylog.c		\
		ylist.c		\
		ysax.c		\
		ydom.c		\
		ydom_xpath.c

# Name of header files
INCLUDES =	ydefs.h		\
		yerror.h	\
		ymem.h		\
		ystr.h		\
		yvect.h		\
		yhash.h		\
		ymap.h		\
		ylog.h		\
		ylist.h		\
		ysax.h		\
		ydom.h

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
# Link options
LDFLAGS = -shared -Wl,-soname,$(SONAME)

# #####################################################################

.PHONY: clean all

$(SONAME): $(OBJS) $(SRC)
	$(CC) $(OBJS) $(LDFLAGS) -o $(SONAME)
	if [ ! -d ../lib/ ]; then mkdir ../lib; fi
	cp $(SONAME) ../lib/
	if [ ! -d ../include ]; then mkdir ../include; fi
	cp $(INCLUDES) ../include/

clean:
	$(RM) $(OBJS) $(SONAME) *~ test
	$(RM) ../lib/$(SONAME)
	if [ -d ../include ]; then cd ../include ; $(RM) $(INCLUDES); fi

all: clean $(SONAME) test

test: main.c
	$(CC) main.c -L. -ly -Wl,-rpath -Wl,'$$ORIGIN/../lib' -o test

.c.o:
	$(CC) $(CFLAGS) -c $<

