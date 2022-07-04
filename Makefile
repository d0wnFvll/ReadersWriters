CC=gcc
CFLAGS=-O2 -pipe
LDFLAGS=-pthread

SRC=main.c
OBJ=rw

all:
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(OBJ) $(SRC)

clean:
	rm -f $(OBJ) >/dev/null 2>&1 || true

.PHONY: all

