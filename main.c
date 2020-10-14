#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG 1            // comentar esta linha quando for medir tempo
#define ARRAY_SIZE 100      // trabalho final com o valores 10.000, 100.000, 1.000.000
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

    double t1, t2; // Tempo de início - Tempo de término
    t1 = MPI_Wtime(); // inicia a contagem do tempo

    int vetor[ARRAY_SIZE][proc_n*2]; // Saco de trabalho
    int size_message = ARRAY_SIZE+1;
    int slaves_alive = proc_n;
    int done_tasks = 0;
    int total_tasks = proc_n*2;
    int last_task = 0; // Utilizado para controlar qual a posição do vetor que será enviado ao slave
    int i;
    int j;

    printf("\ntotal_tasks %d slaves_alive %d", total_tasks, slaves_alive);

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

    while (done_tasks < total_tasks || slaves_alive > 1) {

        // int *message;
        // message = (int *) malloc(ARRAY_SIZE+1 * sizeof(int));
        int message[size_message];

        // Recebe mensagem do slave
        // int message[ARRAY_SIZE+1];   // Buffer para as mensagens
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

                // Envia vetor ao slave
                // int new_vector[ARRAY_SIZE+1];
                for (j=0; j<ARRAY_SIZE; j++) {
                    message[j] = vetor[j][last_task];
                    // printf("\n [%3d] ", new_vector[j]);
                }

                message[size_message] = last_task; // ulitmo elemento do vector é o indice do saco de trabalho
                printf("\n[MASTER] message[size_message] %d", message[size_message]);

                #ifdef DEBUG
                printf("\n[MASTER] Enviando trabalho ao slave id: %d", status.MPI_SOURCE);
                #endif

                MPI_Send(message, size_message, MPI_INT, status.MPI_SOURCE, TAG_JOB_MESSAGES, MPI_COMM_WORLD);

                last_task += 1;

                // #ifdef DEBUG
                // printf("\n[MASTER] Last task: %d", last_task);
                // #endif

                // free(message);
            
            } else { // Shutdown
                int stop_flag = 1;
                MPI_Send(&stop_flag, 1, MPI_INT, status.MPI_SOURCE, TAG_KILL_SLAVE, MPI_COMM_WORLD);
                slaves_alive -= 1;
                printf("\n[MASTER] KILLING SLAVE slaves_alive %d", slaves_alive);
            }


        } else if (status.MPI_TAG == TAG_JOB_MESSAGES) { // Caso recebimento de vetor ordenado

            // Recebe vetor ordenado e atualiza saco de trabalho
            #ifdef DEBUG
            printf("\n[MASTER] Recebendo vetor ordenado de slave id: %d", status.MPI_SOURCE);
            printf("\n[MASTER] Atualizando saco de trabalho");
            #endif

            // for (i=0; i<ARRAY_SIZE+1; i++)
            //     printf("\n [%d]", message[i]);
            
            // #ifdef DEBUG
            // printf("\n[MASTER] message[size_message] %d", message[size_message]);
            // #endif
            
            int index = message[size_message]; // Indice da task resolvida pelo slave
            for (j=0; j<ARRAY_SIZE; j++) {
                vetor[j][index] = message[j];
                // printf("\n [%3d] ", vetor[j][index]);
            }

            // Atualiza contador de tasks
            done_tasks += 1;

            // #ifdef DEBUG
            // printf("\n[MASTER] Done tasks: %d", done_tasks);
            // #endif

            // free(message);

        }

        // Há mais trabalho a ser realizado?
        printf("\n[MASTER] done_tasks %d, total_tasks %d", done_tasks, total_tasks);
        if (done_tasks == total_tasks) {

            // Envia mensagem de desligamento
            #ifdef DEBUG
            printf("\n[MASTER] Enviando comando de encerramento aos slaves");
            #endif

            printf("\n[MASTER] slaves_alive %d\n", slaves_alive);
            while(slaves_alive > 1) {

                printf("\n[MASTER] MATANDO SLAVE");
                
                // Esperamos receber uma solicitação de trabalho
                int message[size_message];
                MPI_Recv(message, size_message, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

                // Enviando mensagem de encerramento
                int stop_flag = 1;
                MPI_Send(&stop_flag, 1, MPI_INT, status.MPI_SOURCE, TAG_KILL_SLAVE, MPI_COMM_WORLD);

                slaves_alive -= 1;

                printf("\n[MASTER] MATOU SLAVE slaves_alive %d", slaves_alive);

            }

            break;
        }

    }

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

    t2 = MPI_Wtime(); // termina a contagem do tempo
    printf("\nTempo de execucao: %f\n\n", t2-t1);   

}

void slave(int my_rank)
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
        // int *message;
        // message = (int *) malloc(size_message * sizeof(int));
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

        bs(ARRAY_SIZE, message);

        // Retorna vetor ordenado ao master
        #ifdef DEBUG
        printf("\n[SLAVE %d] Enviando trabalho", my_rank);
        #endif
        
        MPI_Send(message, size_message, MPI_INT, MASTER, TAG_JOB_MESSAGES, MPI_COMM_WORLD);

        #ifdef DEBUG
        printf("\n[SLAVE %d] I'm slave number: %d and send back a vector", my_rank, my_rank);
        #endif

        // free(message);
    }

    #ifdef DEBUG
    printf("\n[SLAVE %d] Goodbye from slave number: %d\n", my_rank, my_rank);
    #endif

}

int main(int argc, char **argv)
{
    int my_rank;   // Identificador deste processo
    int proc_n;    // Numero de processos disparados pelo usuário na linha de comando (np)
    int source;    // Identificador do proc.origem

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &proc_n);

    if (my_rank == 0) {
        master(proc_n);
    } 
    else {
        slave(my_rank);
    }

    MPI_Finalize();

    return 0;
}