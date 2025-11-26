FLAGS := -Wall -Wpedantic -ggdb

.PHONY: all

main:main.c
	cc main.c -o main ${FLAGS}
