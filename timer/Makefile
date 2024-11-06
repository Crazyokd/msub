CFLAGS=-g -ggdb -Wall -W -std=c99 -fvisibility=hidden
LDFLAGS=-L. -Wl,-R. -lrktw -static

C_SOURCES := $(wildcard *.c)
D_FILES := $(patsubst %.c,%.d,$(C_SOURCES))

.PHONY: clean

all: generate-deps librktw.a librktw.so test

generate-deps: $(D_FILES)

test: test.o
	$(CC) $^ -o $@ $(CFLAGS) $(LDFLAGS)

librktw.so: rk-tw.c
	$(CC) -fPIC -shared $^ -o $@ $(CFLAGS)

librktw.a: rk-tw.o
	$(AR) rcs $@ $^

%.d: %.c
	@set -e; rm -f $@; \
	$(CC) -MM $(CFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

include $(wildcard *.d)

clean:
	rm -f *.o *.d librktw.a librktw.so test
