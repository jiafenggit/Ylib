#include <stdio.h>
#include "ylib.h"

typedef struct {
	ystr_t s;
	uint8_t i;
} test_t;

void removeTest(void *element, void *data);
void removeTest(void *element, void *data) {
	test_t *t = element;
	char *s = data;

	printf("On efface '%d' (%s) [%s]\n", t->i, t->s, s);
	y.str.free(&t->s);
	y.sys.free(t);
}

void main() {
	yvect_t v;
	test_t *t;

	v = y.vect.create(0);
	for (int i = 0; i < 8; ++i) {
		t = y.sys.malloc(sizeof(test_t));
		t->i = i;
		t->s = y.str.create("pouet");
		if (i % 2)
			y.str.append(&t->s, " ! ");
		y.str.trim(t->s);
		y.vect.add(&v, t);
	}
	for (int i = 0; i < y.vect.len(v); ++i) {
		t = v[i];
		printf("(%d) : i='%d', s='%s'\n", i, t->i, t->s);
	}
	y.vect.free(&v, removeTest, "abc");
}
