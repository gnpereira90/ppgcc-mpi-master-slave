#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG 1            // comentar esta linha quando for medir tempo
#define ARRAY_SIZE 40      // trabalho final com o valores 10.000, 100.000, 1.000.000
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

void master(int proc_n)
{
    MPI_Status status;
    MPI_Request request; // used for immediate

    int vetor[ARRAY_SIZE][proc_n*2]; // Saco de trabalho
    int done_tasks = 0;
    int total_tasks = proc_n*2;
    int last_task = 0; // Utilizado para controlar qual a posição do vetor que será enviado ao slave
    int i;
    int j;

    for (i=0 ; i<ARRAY_SIZE; i++) {  /* init array with worst case for sorting */
        for (j=0 ; j<proc_n*2; j++) {
            vetor[i][j] = ARRAY_SIZE-i;
        }
    }

    #ifdef DEBUG // Caso a var DEBUG estiver definida como 1, esse trecho abaixo é compilado
    printf("\n[MASTER] Vetor:\n");
    for (i=0 ; i<ARRAY_SIZE; i++) {
        for (j=0 ; j<proc_n*2; j++) {
            printf("   [%03d] ", vetor[i][j]);
        }
        printf("\n");
    }
    printf("\n");
    #endif

    int *message;
    message = (int *) malloc(ARRAY_SIZE+1 * sizeof(int));

    while (1) {
    
        // Recebe mensagem do slave
        // int message[ARRAY_SIZE+1];   // Buffer para as mensagens
        MPI_Recv(message, ARRAY_SIZE, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        #ifdef DEBUG
        printf("\n[MASTER] Recebendo mensagem de solicitação de slave id %d com tag %d", status.MPI_SOURCE, status.MPI_TAG);
        #endif

        // Caso for request de trabalho
        if (status.MPI_TAG == TAG_REQUEST_TASK) {

            #ifdef DEBUG
            printf("\n[MASTER] Recebendo mensagem de solicitação de slave id: %d", status.MPI_SOURCE);
            #endif

            // Envia vetor ao slave
            // int new_vector[ARRAY_SIZE+1];
            for (j=0; j<ARRAY_SIZE; j++) {
                message[j] = vetor[j][last_task];
                // printf("\n [%3d] ", new_vector[j]);
            }

            message[ARRAY_SIZE+1] = last_task; // ulitmo elemento do vector é o indice do saco de trabalho
            // printf("\n[MASTER] new_vector[ARRAY_SIZE+1] %d", message[ARRAY_SIZE+1]);

            #ifdef DEBUG
            printf("\n[MASTER] Enviando trabalho ao slave id: %d", status.MPI_SOURCE);
            #endif

            MPI_Send(message, ARRAY_SIZE, MPI_INT, status.MPI_SOURCE , TAG_JOB_MESSAGES, MPI_COMM_WORLD);

            last_task += 1;

            // #ifdef DEBUG
            // printf("\n[MASTER] Last task: %d", last_task);
            // #endif

        } else if (status.MPI_TAG == TAG_JOB_MESSAGES) { // Caso recebimento de vetor ordenado

            // Recebe vetor ordenado
            #ifdef DEBUG
            printf("\n[MASTER] Recebendo vetor ordenado de slave id: %d", status.MPI_SOURCE);
            #endif
            
            // int new_vector[ARRAY_SIZE+1];
            // MPI_Recv(new_vector, ARRAY_SIZE, MPI_INT, MPI_ANY_SOURCE, TAG_JOB_MESSAGES, MPI_COMM_WORLD, &status);

            // Atualiza saco de trabalho
            #ifdef DEBUG
            printf("\n[MASTER] Atualizando saco de trabalho");
            #endif

            // for (i=0; i<ARRAY_SIZE+1; i++)
            //     printf("\n [%d]", message[i]);
            
            int index = message[ARRAY_SIZE+1]; // Indice da task resolvida pelo slave
            // printf("\n[MASTER] message[ARRAY_SIZE+1] %d", message[ARRAY_SIZE+1]);
            for (j=0; j<ARRAY_SIZE; j++) {
                vetor[j][index] = message[j];
                // printf("\n [%3d] ", vetor[j][index]);
            }

            // Atualiza contador de tasks
            done_tasks += 1;

            // #ifdef DEBUG
            // printf("\n[MASTER] Done tasks: %d", done_tasks);
            // #endif

        }

        // Há mais trabalho a ser realizado?
        if (done_tasks == total_tasks) {

            // Envia mensagem de desligamento
            #ifdef DEBUG
            printf("\n[MASTER] Enviando comando de encerramento aos slaves");
            #endif

            int stop_flag = 1;
            for (int slave_id=1; slave_id<proc_n; slave_id++) {
                MPI_Send(&stop_flag, 1, MPI_INT, slave_id, TAG_KILL_SLAVE, MPI_COMM_WORLD);
            }

            break;
        }

        // free(message);

    }

    // Recebe solicacoes de trabalho dos slaves e envia os vetores
    // int idx;
    // for (idx=0; idx<proc_n*2; idx++) {

    //     printf("\n[MASTER] Recebendo mensagem de solicitação");
    //     // Mensagem de solicitação de trabalho de qualquer (MPI_ANY_SOURCE) slave
    //     int message; // TODO Há alguma forma melhor ? estou mandando apenas um valor int
    //     MPI_Recv(&message, 1, MPI_INT, MPI_ANY_SOURCE, TAG_REQUEST_TASK, MPI_COMM_WORLD, &status);

    //     // Envia vetor ao slave
    //     int new_vector[ARRAY_SIZE+1];
    //     for (j=0 ; j<ARRAY_SIZE; j++)
    //         new_vector[j] = vetor[j][idx];

    //     new_vector[ARRAY_SIZE+1] = idx; // ulitmo elemento do vector é o indice do saco de trabalho

    //     printf("\n[MASTER] Enviando trabalho");
    //     MPI_Send(new_vector, ARRAY_SIZE, MPI_INT, status.MPI_SOURCE , TAG_JOB_MESSAGES, MPI_COMM_WORLD);

    //     // Recebe vetor ordenado
    //     printf("\n[MASTER] Recebendo trabalho");
    //     MPI_Recv(new_vector, ARRAY_SIZE, MPI_INT, MPI_ANY_SOURCE, TAG_JOB_MESSAGES, MPI_COMM_WORLD, &status);

    //     // Atualiza saco de trabalho
    //     printf("\n[MASTER] Atualizando saco de trabalho");
    //     int index = new_vector[ARRAY_SIZE+1];
    //     for (j=0 ; j<ARRAY_SIZE; j++) {
    //         vetor[j][index] = new_vector[j];
    //     }

        // printf("\n[MASTER] loop idx: %d", idx);
    // }

    // Envia comando de encerramento
    // #ifdef DEBUG
    // printf("\n[MASTER] Enviando comando de encerramento\n");
    // #endif

    // int stop_flag = 1;
    // for (int slave_id=1; slave_id<proc_n; slave_id++) {
    //     MPI_Send(&stop_flag, 1, MPI_INT, slave_id, TAG_KILL_SLAVE, MPI_COMM_WORLD);
    // }

    #ifdef DEBUG
    printf("\n\n[MASTER] Saco de trabalho atualizado");
    printf("\n[MASTER] Vetores ordenados:\n");
    for (i=0 ; i<ARRAY_SIZE; i++) {
        for (j=0 ; j<proc_n*2; j++) {
            printf("   [%03d] ", vetor[i][j]);
        }
        printf("\n");
    }
    printf("\n");
    #endif

}

void slave(int my_rank)
{
    MPI_Status status;
    MPI_Request request;

    #ifdef DEBUG
    printf("\n[SLAVE %d] I'm slave number: %d", my_rank, my_rank);
    #endif

    // Comando de encerramento   
    while(1) {

        int i=0;
        int vector[ARRAY_SIZE];

        // Envia mensagem ao master solicitando trabalho
        printf("\n[SLAVE %d] Solicitando trabalho", my_rank);
        MPI_Send(&i, 1, MPI_INT, MASTER, TAG_REQUEST_TASK, MPI_COMM_WORLD);

        // Recebe o trabalho
        printf("\n[SLAVE %d] Recebendo trabalho", my_rank);
        MPI_Recv(vector, ARRAY_SIZE, MPI_INT, MASTER, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        if (status.MPI_TAG == TAG_KILL_SLAVE) {

            #ifdef DEBUG
            printf("\n[SLAVE %d] Goodbye from slave number: %d\n", my_rank, my_rank);
            #endif

            break;
        }

        // Ordena vetor
        printf("\n[SLAVE %d] Ordenando vetor", my_rank);
        bs(ARRAY_SIZE, vector);

        // Retorna vetor ordenado ao master
        printf("\n[SLAVE %d] Enviando trabalho", my_rank);
        MPI_Send(vector, ARRAY_SIZE, MPI_INT, MASTER, TAG_JOB_MESSAGES, MPI_COMM_WORLD);

        #ifdef DEBUG
        printf("\n[SLAVE %d] I'm slave number: %d and send back a vector", my_rank, my_rank);
        #endif
    }

}

int main(int argc, char **argv)
{
    int my_rank;   // Identificador deste processo
    int proc_n;    // Numero de processos disparados pelo usuário na linha de comando (np)
    int source;    // Identificador do proc.origem

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &proc_n);

    printf("%2d ", my_rank);

    if (my_rank == 0) {
        master(proc_n);
    } 
    else {
        slave(my_rank);
    }

    // #ifdef DEBUG // Caso a var DEBUG estiver definida como 1, esse trecho abaixo é compilado
    // printf("\nVetor: ");
    // for (i=0 ; i<ARRAY_SIZE; i++)              /* print unsorted array */
    //     printf("[%03d] ", vetor[i]);
    // printf("\n");
    // #endif

    // bs(ARRAY_SIZE, vetor);                     /* sort array */

    // #ifdef DEBUG
    // printf("\nSort Vetor: ");
    // for (i=0 ; i<ARRAY_SIZE; i++)                              /* print sorted array */
    //     printf("[%03d] ", vetor[i]);
    // printf("\n");
    // #endif

    MPI_Finalize();

    return 0;
}