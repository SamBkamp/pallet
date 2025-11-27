FLAGS := -Wall -Wpedantic -ggdb

.PHONY: all

pallet:main.c
	cc main.c -o pallet ${FLAGS}
