default: build

build:
	g++ main.c -o main.o

run: 
	./main.o ${ARRAY_SIZE} ${NUMBER_VECTORS} ${DEBUG}

test: build 
	./main.o

# make
# make run ARRAY_SIZE=10 NUMBER_VECTORS=10 DEBUG=1
# make run ARRAY_SIZE=10 NUMBER_VECTORS=10 DEBUG=0