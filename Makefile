FLAGS := -Wall -Wpedantic

.PHONY: all

all:
	cc main.c -o main ${FLAGS}
