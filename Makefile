default: build

build: main.c
	mpicc main.c -o main.o

run:
	mpirun --hostfile hostfile -np 25 main.o

clear:
	rm -f main.o

test:
	mpicc main.c -o main.o
	mpirun --hostfile hostfile -np 1 main.o