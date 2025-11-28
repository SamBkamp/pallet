FLAGS := -Wall -Wpedantic -ggdb

.PHONY: all

all:pallet mount-fs

pallet:main.c
	cc main.c -o pallet ${FLAGS}

mount-fs:mount-fs.c
	cc mount-fs.c -o mount-fs ${FLAGS}
