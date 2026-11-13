CC ?= cc
CFLAGS ?= -O2
PREFIX ?= /usr/local
LDFLAGS ?=
LDLIBS = -lm
VERSION := $(shell cat VERSION)
CODENAME ?= Overclocked ASCII

UNAME_S := $(shell uname -s)
UNAME_M := $(shell uname -m)
ifeq ($(UNAME_S),Darwin)
  LDLIBS += -framework IOKit -framework CoreFoundation
endif

fetch: fetch.c fetch_logo_color.h
	$(CC) $(CFLAGS) $(LDFLAGS) -DFETCH_VERSION='"$(VERSION)"' -DFETCH_CODENAME='"$(CODENAME)"' -DFETCH_ARCH='"$(UNAME_M)"' -DFETCH_OS='"$(UNAME_S)"' -o $@ $< $(LDLIBS)

install: fetch
	install -d $(DESTDIR)$(PREFIX)/bin
	install -m 755 fetch $(DESTDIR)$(PREFIX)/bin/fetch

clean:
	rm -f fetch

test: fetch
	mkdir -p tests/.work
	$(CC) -std=c11 -Wall -Wextra -Werror tests/logo-colors.c -o tests/.work/logo-colors
	./tests/.work/logo-colors
	python3 tests/posix.py

.PHONY: install clean test
