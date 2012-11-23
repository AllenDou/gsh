CC=gcc
CFLAGS=-Wall -g -O0
LINK= -ldl -lm
bin = bin
LINK=-lm

all:
	$(CC) $(CFLAGS) -o $(bin)/cache -std=c99 cache.c -I lib/ lib/*.c $(LINK)
	$(CC) $(CFLAGS) -o $(bin)/fmload -std=c99 fmload.c -I lib/ lib/*.c $(LINK)
	$(CC) $(CFLAGS) -o $(bin)/carsvm -std=c99 carsvm.c -I lib/ lib/*.c $(LINK)
	$(CC) $(CFLAGS) -o $(bin)/bc_cli -std=c99 bc.c -I lib/ lib/*.c $(LINK)
clean:
	rm -rf $(bin)/*
