default: build

build:
	mpicc main.c -o main.o -std=c99 -Wall

run: build
	mpirun --hostfile hostfile -np ${NP} main.o ${ARRAY_SIZE} ${NUMBER_VECTORS}

clear:
	rm -f *.o

test:
	mpicc main.c -o main.o
	mpirun --hostfile hostfile -np ${NP} main.o ${ARRAY_SIZE} ${NUMBER_VECTORS}



# make run NP=5 ARRAY_SIZE=100000 NUMBER_VECTORS=10