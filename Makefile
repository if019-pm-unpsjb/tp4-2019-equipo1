CC=gcc
CFLAGS=-Wall -g
CFLAGPTHREAD=-pthread
PROG=servidor cliente
SERV=serv
CLIENT=cli

.PHONY: all
all: $(PROG)

servidor: servidorPortero.c
	$(CC) -o $(SERV) servidorPortero.c $(CFLAGPTHREAD)

cliente: clientePortero.c
	$(CC) -o $(CLIENT) clientePortero.c 

.PHONY: clean
clean:
	rm -f cli serv
