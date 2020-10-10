default: build

build: main.c
	mpicc main.c -o main.o

run:
	mpirun --hostfile hostfile -np 25 main.o