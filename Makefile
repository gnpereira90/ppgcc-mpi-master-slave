default: build

build: main.c
	mpicc main.c -o main.o

run:
	mpirun --hostfile hostfile -np ${NP} main.o

clear:
	rm -f *.o

test:
	mpicc main.c -o main.o
	mpirun --hostfile hostfile -np ${NP} main.o