default: build

build:
	mpicc main.c -o main.o

run:
	mpirun --hostfile hostfile -np ${NP} main.o ${ARRAY_SIZE}

clear:
	rm -f *.o

test:
	mpicc main.c -o main.o
	mpirun --hostfile hostfile -np ${NP} main.o ${ARRAY_SIZE}



# make run NP=5 ARRAY_SIZE=100000