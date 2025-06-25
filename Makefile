# dsblocks - dwm status blocks
# See LICENSE file for copyright and license details.

include config.mk

SRC = alsa.c bat.c bri.c date.c dsblocks.c signaler.c

all: dsblocks

config.h:
	cp config.def.h $@

dsblocks: config.h
	$(CC) dsblocks.c -o $@ $(CFLAGS) $(LDFLAGS)

clean:
	rm -f dsblocks dsblocks-$(VERSION).tar.gz

dist: clean
	mkdir -p dsblocks-$(VERSION)
	cp Makefile config.def.h config.mk dsblocks.1 README.md $(SRC)\
					dsblocks-$(VERSION)
	tar -cf dsblocks-$(VERSION).tar dsblocks-$(VERSION)
	gzip dsblocks-$(VERSION).tar
	rm -rf dsblocks-$(VERSION)

install: all
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f dsblocks $(DESTDIR)$(PREFIX)/bin
	chmod 755 $(DESTDIR)$(PREFIX)/bin/dsblocks
	mkdir -p $(DESTDIR)$(MANPREFIX)/man1
	sed "s/VERSION/$(VERSION)/g" < dsblocks.1 > $(DESTDIR)$(MANPREFIX)/man1/dsblocks.1
	chmod 644 $(DESTDIR)$(MANPREFIX)/man1/dsblocks.1

uninstall:
	rm -rf $(DESTDIR)$(PREFIX)/bin/dsblocks\
					$(DESTDIR)$(MANPREFIX)/man1/dsblocks.1

.PHONY: all clean dist install uninstall
