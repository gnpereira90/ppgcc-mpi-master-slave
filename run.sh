#!/bin/bash

mkdir -p logs/sort-sequential
mkdir -p logs/master-slave-sort

tries=3

number_vectors=1000
vector_size=100000

cd sort-sequential
for ((i = 1; i <= $tries; i++)); do
    make run ARRAY_SIZE=$vector_size NUMBER_VECTORS=$number_vectors > ../logs/sort-sequential/sort-sequential-$i.txt
done

cd ../master-slave-sort
for ((i = 1; i <= $tries; i++)); do
    processes=($(seq 2 1 20))
    for np in "${processes[@]}"; do
        make run NP=$np ARRAY_SIZE=$vector_size NUMBER_VECTORS=$number_vectors > ../logs/master-slave-sort/master-slave-sort-$np-$i.txt
    done
done