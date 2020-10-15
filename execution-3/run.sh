#!/bin/bash -e

echo
echo "Running on `hostname`"
echo "Initial Time: `date`"
echo

mkdir -p logs
touch logs/sort-sequential.csv
touch logs/master-slave-sort.csv

TRIES=3
ARGS=(1000 5000 10000)
DIR=$PWD
OUTPUTFILE_SEQUENTIAL="$PWD/logs/sort-sequential.csv"
OUTPUTFILE_MASTER_SLAVE="$PWD/logs/master-slave-sort.csv"

number_vectors=1000

cd "$DIR/sort-sequential"
make
echo "TRIES;ARRAY_SIZE;NUMBER_VECTORS;RUNTIME" > "$OUTPUTFILE_SEQUENTIAL"
echo "Runnning Bubble Sort Sequential"
for k in $(seq 1 $TRIES); do
    for ((i = 0; i < ${#ARGS[@]}; i++)); do
        row=$(make run ARRAY_SIZE=${ARGS[$i]} NUMBER_VECTORS=$number_vectors | grep -i 'RUNTIME' | cut -d "=" -f2)
        echo "$k; ${ARGS[$i]}; $number_vectors; $row" >> "$OUTPUTFILE_SEQUENTIAL"
        echo "$k; ${ARGS[$i]}; $number_vectors; $row"
    done
done

cd "$DIR/master-slave-sort"
make build_lad
echo "TRIES;NPS;ARRAY_SIZE;NUMBER_VECTORS;RUNTIME" > "$OUTPUTFILE_MASTER_SLAVE"
echo "Runnning Bubble Sort Master Slave"
for k in $(seq 1 $TRIES); do
    processes=($(seq 2 2 32))
    for np in "${processes[@]}"; do
        for ((i = 0; i < ${#ARGS[@]}; i++)); do
            row=$(make run_lad NP=$np ARRAY_SIZE=${ARGS[$i]} NUMBER_VECTORS=$number_vectors | grep -i 'RUNTIME' | cut -d "=" -f2)
            echo "$k; $np; ${ARGS[$i]}; $number_vectors; $row" >> "$OUTPUTFILE_MASTER_SLAVE"
            echo "$k; $np; ${ARGS[$i]}; $number_vectors; $row"
        done
    done
done

echo
echo "Final Time: `date`"
echo
