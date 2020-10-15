#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// #define DEBUG 1             // comentar esta linha quando for medir tempo
// #define ARRAY_SIZE 1000000      // trabalho final com o valores 10.000, 100.000, 1.000.000
// #define NUMBER_VECTORS 20   // Quantidade de vetores na matriz

// Bubble Sort
void bs(int n, int * vetor) {
    int c=0, d, troca, trocou =1;

    while (c < (n-1) & trocou )
        {
        trocou = 0;
        for (d = 0 ; d < n - c - 1; d++)
            if (vetor[d] > vetor[d+1])
                {
                troca      = vetor[d];
                vetor[d]   = vetor[d+1];
                vetor[d+1] = troca;
                trocou = 1;
                }
        c++;
        }
}

void initialize_matrix(int ARRAY_SIZE, int NUMBER_VECTORS, int *matrix) {

    #ifdef DEBUG
    printf("\n[MASTER] Inicializando matriz");
    #endif

    for (int i=0 ; i<ARRAY_SIZE; i++) {  /* init array with worst case for sorting */
        for (int j=0 ; j<NUMBER_VECTORS; j++) {
            matrix[i*NUMBER_VECTORS+j] = ARRAY_SIZE-i;
        }
    }

    #ifdef DEBUG // Caso a var DEBUG estiver definida como 1, esse trecho abaixo é compilado
    printf("\nMatriz:\n");
    for (int i=0 ; i<ARRAY_SIZE; i++) {
        for (int j=0 ; j<NUMBER_VECTORS; j++) {
            printf(" [%03d] ", matrix[i*NUMBER_VECTORS+j]);
        }
        printf("\n");
    }
    #endif
}

int main(int argc, char **argv) {

    const int ARRAY_SIZE = atoi(argv[1]);
    const int NUMBER_VECTORS = atoi(argv[2]);
    // const int DEBUG = argv[3];

    clock_t start = clock(); // inicia a contagem do tempo

    int *matrix = (int *)malloc(ARRAY_SIZE * NUMBER_VECTORS * sizeof(int));

    initialize_matrix(ARRAY_SIZE, NUMBER_VECTORS, matrix);

    int *vector = (int*) malloc(sizeof(int)*ARRAY_SIZE);

    for (int i=0; i<NUMBER_VECTORS; i++) {

        for (int j=0; j<ARRAY_SIZE; j++) {
            vector[j] = matrix[j*NUMBER_VECTORS+i]; // matrix[j][i];
            // printf("\nvector[j] %d", vector[j]);
        }
        // printf("\n");

        bs(ARRAY_SIZE, vector);

        for (int j=0; j<ARRAY_SIZE; j++) {
            // matrix[j][i] = vector[j];
            matrix[j*NUMBER_VECTORS+i] = vector[j];
            // printf("\nvector[j] %d", vector[j]);
        }
        // printf("\n");

    }

    #ifdef DEBUG
    printf("\nMatrix sorted:\n");
    for (int i=0 ; i<ARRAY_SIZE; i++) {
        for (int j=0 ; j<NUMBER_VECTORS; j++) {
            printf(" [%03d] ", matrix[i][j]);
        }
        printf("\n");
    }
    #endif

    free(matrix);
    free(vector);

    clock_t end = clock(); // termina a contagem do tempo
    float seconds = (float)(end - start) / CLOCKS_PER_SEC;

    printf("\nARRAY_SIZE=%d", ARRAY_SIZE);
    printf("\nNUMBER_VECTORS=%d", NUMBER_VECTORS);
    printf("\nRUNTIME=%f\n", seconds);

    return 0;
}
