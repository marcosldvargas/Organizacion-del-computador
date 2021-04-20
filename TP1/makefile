CC = gcc
CFLAGS ?= -g -O2 -std=gnu11 -Wall -Wextra -Wvla
LDLIBS = -lm
VFLAGS = --leak-check=full --track-origins=yes --show-reachable=yes

main:
	$(CC) *.c $(CFLAGS) $(LDLIBS) -o cachesim

pruebas:
	valgrind $(VFLAGS) ./cachesim adpcm.xex 2048 2 64

pruebas_verboso:
	valgrind $(VFLAGS) ./cachesim adpcm.xex 2048 2 64 -v 0 15000

pruebas_mapeo_directo:
	valgrind $(VFLAGS) ./cachesim adpcm.xex 4096 1 256 -v 0 10000

clean:
	rm -f cachesim

.PHONY: all clean