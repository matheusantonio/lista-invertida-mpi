// Protótipo para o programa de criação de listas invertidas utilizando MPI

#include<stdio.h>
#include<string.h>
#include<mpi.h>

// Esse número de processos será definido dinamicamente a partir da quantidade de palavras no texto
#define NUM_SPAWNS 5

int main(int argc, char *argv[]){
    int np = NUM_SPAWNS; // número de processos que serão criados
    MPI_Comm parentcomm, intercomm;

    MPI_Init(&argc, &argv);
    MPI_Comm_get_parent(&parentcomm); // verifica qual é o comunicador para o processo pai

    if(parentcomm == MPI_COMM_NULL){ // se não houver comunicador para o pai, ele é o pai
        printf("Master process.\n"); // exibe que é o processo mestre
        fflush(stdout);

        // Cria várias instâncias de processos do arquivo invertex.e
        // intercomm é o comunicador que o pai utilizará para comunicar-se com os processos filhos.
        MPI_Comm_spawn("./invertex.e", MPI_ARGV_NULL, np, MPI_INFO_NULL, 0, MPI_COMM_WORLD, &intercomm, MPI_ERRCODES_IGNORE);

        for(int i=0;i<np;i++){
            // Para cada processo filho, envia um valor (nesse caso, é o número do próprio processo)
            MPI_Send(&i, 1, MPI_INT, i, 0, intercomm);
        }
    } else{ // Caso haja um comunicador para o pai, significa que é um processo filho (escravo)
        int val; // variável que receberá o valor do mestre
        MPI_Status st; // estrutura padrão do MPI, não é relevante para esse exemplo mas precisa ser atribuída.

        // O processo escravo recebe um valor do processo pai
        MPI_Recv(&val, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, parentcomm, &st);
        
        printf("My value: %d\n", val);
        fflush(stdout);
    }

    MPI_Finalize();
}