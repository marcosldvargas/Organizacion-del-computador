CC ?= gcc
MAIN := pruebas_cola

CFLAGS := -Wall -g -std=gnu18

all: test

test: $(MAIN)
	valgrind --leak-check=full --track-origins=yes --show-reachable=yes  ./$(MAIN)

clean:
	rm -f *.o $(MAIN) core vgcore.*

$(MAIN): pruebas_cola.o cola_defs.o cola_impl.o

.PHONY: all test clean
