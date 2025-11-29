FLAGS := -Wall -Wpedantic -ggdb

.PHONY: all

all:pallet

pallet:main.c
	cc $< -o $@ ${FLAGS}

