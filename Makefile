install: dsblocks
	cp -f dsblocks /usr/bin/dsblocks
	chmod 755 /usr/bin/dsblocks

dsblocks:
	cp config.def.h config.h
	gcc dsblocks.c -o dsblocks -flto -Os -lasound -lm -lX11

clean:
	rm -rf *.o dsblocks
