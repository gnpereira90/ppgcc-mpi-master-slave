default: build

build:
	mpicc main.c -o main.o

run:
	mpirun --hostfile hostfile -np ${NP} main.o

clear:
	rm -f *.o

test:
	mpicc main.c -o main.o
	mpirun --hostfile hostfile -np ${NP} main.o