#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// #define DEBUG 1            // comentar esta linha quando for medir tempo
// #define ARRAY_SIZE 40      // trabalho final com o valores 10.000, 100.000, 1.000.000
// #define NUMBER_VECTORS 20   // Quantidade de vetores na matriz
#define MASTER 0
#define TAG_REQUEST_TASK 1
#define TAG_KILL_SLAVE 2
#define TAG_JOB_MESSAGES 3

// Bubble Sort
void bs(int n, int * vetor)
{
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

void initialize_matrix(int ARRAY_SIZE, int NUMBER_VECTORS, int matrix[ARRAY_SIZE][NUMBER_VECTORS]) {
    
    for (int i=0 ; i<ARRAY_SIZE; i++) {  /* init array with worst case for sorting */
        for (int j=0 ; j<NUMBER_VECTORS; j++) {
            matrix[i][j] = ARRAY_SIZE-i;
        }
    }

    #ifdef DEBUG // Caso a var DEBUG estiver definida como 1, esse trecho abaixo é compilado
    printf("\nMatrix:\n");
    for (int i=0 ; i<ARRAY_SIZE; i++) {
        for (int j=0 ; j<NUMBER_VECTORS; j++) {
            printf(" [%03d] ", matrix[i][j]);
        }
        printf("\n");
    }
    #endif
}

void master(int proc_n, int ARRAY_SIZE, int NUMBER_VECTORS)
{
    MPI_Status status;
    MPI_Request request; // used for immediate

    double t1, t2; // Tempo de início - Tempo de término
    t1 = MPI_Wtime(); // inicia a contagem do tempo

    int matrix[ARRAY_SIZE][NUMBER_VECTORS]; // Saco de trabalho
    int size_message = ARRAY_SIZE+1;
    int slaves_alive = proc_n;
    int done_tasks = 0;
    int total_tasks = proc_n*2;
    int last_task = 0; // Utilizado para controlar qual a posição da matriz que será enviado ao slave
    int i;
    int j;

    initialize_matrix(ARRAY_SIZE, NUMBER_VECTORS, matrix);

    while (done_tasks < total_tasks || slaves_alive > 1) {

        // Recebe mensagem do slave
        int message[size_message];
        MPI_Recv(message, size_message, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        #ifdef DEBUG
        printf("\n[MASTER] Recebendo mensagem de solicitação de slave id %d com tag %d", status.MPI_SOURCE, status.MPI_TAG);
        #endif

        // Caso for request de trabalho
        if (status.MPI_TAG == TAG_REQUEST_TASK) {

            #ifdef DEBUG
            printf("\n[MASTER] Recebendo mensagem de solicitação de slave id: %d", status.MPI_SOURCE);
            #endif

            if (last_task < total_tasks) {

                // Envia matrix ao slave
                // int new_vector[ARRAY_SIZE+1];
                for (j=0; j<ARRAY_SIZE; j++) {
                    message[j] = matrix[j][last_task];
                    // printf("\n [%3d] ", new_vector[j]);
                }

                message[ARRAY_SIZE] = last_task; // ultimo elemento do vector é o indice do saco de trabalho
                
                #ifdef DEBUG
                printf("\n[MASTER] Sending task: %d\n", last_task);
                for (i=0; i<size_message; i++)
                    printf(" [%d] ", message[i]);
                printf("\n");
                #endif

                #ifdef DEBUG
                printf("\n[MASTER] message[size_message] %d", message[size_message]);
                printf("\n[MASTER] Enviando trabalho ao slave id: %d", status.MPI_SOURCE);
                #endif

                MPI_Send(message, size_message, MPI_INT, status.MPI_SOURCE, TAG_JOB_MESSAGES, MPI_COMM_WORLD);

                last_task += 1;
            
            } else { // Shutdown
                int stop_flag = 1;
                MPI_Send(&stop_flag, 1, MPI_INT, status.MPI_SOURCE, TAG_KILL_SLAVE, MPI_COMM_WORLD);
                slaves_alive -= 1;
                
                #ifdef DEBUG
                printf("\n[MASTER] KILLING SLAVE slaves_alive %d", slaves_alive);
                #endif
            }


        } else if (status.MPI_TAG == TAG_JOB_MESSAGES) { // Caso recebimento de vetor ordenado

            // Recebe vetor ordenado e atualiza saco de trabalho
            #ifdef DEBUG
            printf("\n[MASTER] Recebendo vetor ordenado de slave id: %d", status.MPI_SOURCE);
            printf("\n[MASTER] Atualizando saco de trabalho");
            #endif

            #ifdef DEBUG
            printf("\n[MASTER] Receive task: %d\n", message[ARRAY_SIZE]);
            for (i=0; i<size_message; i++)
                printf(" [%d] ", message[i]);
            printf("\n");
            #endif

            int index = message[ARRAY_SIZE]; // Indice da task resolvida pelo slave;
            for (j=0; j<ARRAY_SIZE; j++) {
                matrix[j][index] = message[j];
                // printf("\n [%3d] ", vetor[j][index]);
            }

            // Atualiza contador de tasks
            done_tasks += 1;

            #ifdef DEBUG
            printf("\n[MASTER] Done tasks: %d", done_tasks);
            #endif

        }

    }

    #ifdef DEBUG
    printf("\n\n[MASTER] Saco de trabalho atualizado");
    printf("\n[MASTER] Vetores ordenados:\n");
    for (i=0 ; i<ARRAY_SIZE; i++) {
        for (j=0 ; j<proc_n*2; j++) {
            printf("   [%07d] ", matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
    #endif

    t2 = MPI_Wtime(); // termina a contagem do tempo
    printf("\nTempo de execucao: %f\n\n", t2-t1);   

}

void slave(int my_rank, int ARRAY_SIZE, int NUMBER_VECTORS)
{
    MPI_Status status;
    MPI_Request request;
    int size_message = ARRAY_SIZE+1;

    #ifdef DEBUG
    printf("\n[SLAVE %d] I'm slave number: %d", my_rank, my_rank);
    #endif

    // Comando de encerramento   
    while(1) {

        int i=0;
        int message[size_message];

        // Envia mensagem ao master solicitando trabalho
        #ifdef DEBUG
        printf("\n[SLAVE %d] Solicitando trabalho", my_rank);
        #endif

        MPI_Send(message, size_message, MPI_INT, MASTER, TAG_REQUEST_TASK, MPI_COMM_WORLD);

        // Recebe o trabalho
        #ifdef DEBUG
        printf("\n[SLAVE %d] Recebendo trabalho", my_rank);
        #endif

        MPI_Recv(message, size_message, MPI_INT, MASTER, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        if (status.MPI_TAG == TAG_KILL_SLAVE) {
            break;
        }

        // Ordena vetor
        #ifdef DEBUG
        printf("\n[SLAVE %d] Ordenando vetor", my_rank);
        #endif

        #ifdef DEBUG
        printf("\n[SLAVE] Receive task: %d\n", message[ARRAY_SIZE]);
        for (i=0; i<size_message; i++)
            printf(" [%d] ", message[i]);
        printf("\n");
        #endif

        int index = message[ARRAY_SIZE]; // Indice da task resolvida pelo slave
        int vector[size_message];
        for (int j=0; j<ARRAY_SIZE; j++) {
            vector[j] = message[j];
        }

        bs(ARRAY_SIZE, vector);

        // Retorna vetor ordenado ao master
        #ifdef DEBUG
        printf("\n[SLAVE %d] Enviando trabalho", my_rank);
        #endif
        
        vector[ARRAY_SIZE] = index;
        MPI_Send(vector, size_message, MPI_INT, MASTER, TAG_JOB_MESSAGES, MPI_COMM_WORLD);

        #ifdef DEBUG
        printf("\n[SLAVE] Sending task: %d\n", vector[ARRAY_SIZE]);
        for (i=0; i<size_message; i++)
            printf(" [%d] ", vector[i]);
        printf("\n");
        #endif

        #ifdef DEBUG
        printf("\n[SLAVE %d] I'm slave number: %d and send back a vector", my_rank, my_rank);
        #endif

    }

    #ifdef DEBUG
    printf("\n[SLAVE %d] Goodbye from slave number: %d\n", my_rank, my_rank);
    #endif

}

int main(int argc, char **argv)
{
    int my_rank;   // Identificador deste processo
    int proc_n;    // Numero de processos disparados pelo usuário na linha de comando (np)

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &proc_n);

    const int ARRAY_SIZE = atoi(argv[1]); // In C, the atoi() function converts a string to an integer
    const int NUMBER_VECTORS = proc_n*2;

    if (my_rank == 0) {
        master(proc_n, ARRAY_SIZE, NUMBER_VECTORS);
    } 
    else {
        slave(my_rank, ARRAY_SIZE, NUMBER_VECTORS);
    }

    MPI_Finalize();

    return 0;
}