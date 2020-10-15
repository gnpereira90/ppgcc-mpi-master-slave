#!/bin/bash -e

mkdir -p logs
touch logs/sort-sequential.csv
touch logs/master-slave-sort.csv

TRIES=5
# CORES=("1" "2" "4" "8" "16")
DIR=$PWD
OUTPUTFILE_MASTER_SLAVE="$PWD/logs/sort-sequential.csv"
OUTPUTFILE_SEQUENTIAL="$PWD/logs/master-slave-sort.csv"

number_vectors=10
array_size=10

cd "$DIR/sort-sequential"
make
echo "TRIES;ARRAY_SIZE;NUMBER_VECTORS;RUNTIME" > "$OUTPUTFILE_SEQUENTIAL"
for i in $(seq 1 $TRIES); do
    row=$(make run ARRAY_SIZE=$array_size NUMBER_VECTORS=$number_vectors | grep -i 'RUNTIME' | cut -d "=" -f2)
    echo "$i;$row" >> "$OUTPUTFILE_SEQUENTIAL"
    echo "$i;$array_size;$number_vectors;$row"
done

cd "$DIR/master-slave-sort"
make
echo "TRIES;NPS;ARRAY_SIZE;NUMBER_VECTORS;RUNTIME" > "$OUTPUTFILE_MASTER_SLAVE"
for i in $(seq 1 $TRIES); do

    processes=($(seq 2 2 32))

    for np in "${processes[@]}"; do
        row=$(make run NP=$np ARRAY_SIZE=$array_size NUMBER_VECTORS=$number_vectors | grep -i 'RUNTIME' | cut -d "=" -f2)
        echo "$i;$np;$array_size;$number_vectors;$row" >> "$OUTPUTFILE_MASTER_SLAVE"
    done
done