FLAGS := -Wall -Wpedantic -ggdb

.PHONY: all

all:pallet

pallet:main.c src/config.c src/ipc.c
	cc $^ -o $@ ${FLAGS}

