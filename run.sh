#!/bin/bash -e

echo
echo "Running on `hostname`"
echo "Initial Time: `date`"
echo

mkdir -p logs
touch logs/dataset.csv

TRIES=3
ARGS=(1000 5000 10000)
DIR=$PWD
OUTPUTFILE="$PWD/logs/dataset.csv"

number_vectors=1000

CSVHEAD="TRIES;NPS;NUMBER_VECTORS"
for ((i = 0; i < ${#ARGS[@]}; i++)); do
	CSVHEAD="$CSVHEAD;${ARGS[$i]}"
done

echo "$CSVHEAD" > "$OUTPUTFILE"

cd "$DIR/sort-sequential"
make
echo "Runnning Bubble Sort Sequential"
for k in $(seq 1 $TRIES); do
    row=""
    for ((i = 0; i < ${#ARGS[@]}; i++)); do
        row="$row $(make run ARRAY_SIZE=${ARGS[$i]} NUMBER_VECTORS=$number_vectors | grep -i 'RUNTIME' | cut -d "=" -f2);"
    done
    echo "$k; 1; $number_vectors; $row" >> "$OUTPUTFILE"
    echo "$k; 1; $number_vectors; $row"
done

cd "$DIR/master-slave-sort"
make
echo "Runnning Bubble Sort Master Slave"
for k in $(seq 1 $TRIES); do
    processes=($(seq 2 2 32))
    for np in "${processes[@]}"; do
        row=""
        for ((i = 0; i < ${#ARGS[@]}; i++)); do
            row="$row $(make run NP=$np ARRAY_SIZE=${ARGS[$i]} NUMBER_VECTORS=$number_vectors | grep -i 'RUNTIME' | cut -d "=" -f2);"
        done
        echo "$k; $np; $number_vectors; $row" >> "$OUTPUTFILE"
        echo "$k; $np; $number_vectors; $row"
    done
done

echo
echo "Final Time: `date`"
echo