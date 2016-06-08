FILES	=	y	\
		y.sys

doc:
	for f in $(FILES); do \
		cat $$f.pod _footer.pod > __file.pod; \
		pod2man -c "Ylib" -r "2.0.0" __file.pod $$f.man.3; \
		rm -f __file.pod; \
	done

clean:
	rm -f *.man.3

all: clean doc

