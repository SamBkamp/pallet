FLAGS := -Wall -Wpedantic -ggdb

.PHONY: all

all:pallet

pallet:main.c src/config.c
	cc $^ -o $@ ${FLAGS}

