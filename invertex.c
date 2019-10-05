// Protótipo para o programa de criação de listas invertidas utilizando MPI

#include<stdio.h>
#include<string.h>
#include<mpi.h>

// Esse número de processos será definido dinamicamente a partir da quantidade de palavras no texto
#define NUM_SPAWNS 5

int main(int argc, char *argv[]){
    int np = NUM_SPAWNS; // número de processos que serão criados
    MPI_Comm parentcomm, intercomm;
    MPI_Status st; // estrutura padrão do MPI, não é relevante para esse exemplo mas precisa ser atribuída.

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
            // Os inteiros representam as palavras que serão encontradas pelo mestre
            MPI_Send(&i, 1, MPI_INT, i, 0, intercomm);

            int res;
            // O mestre irá receber, de cada palavra, a quantidade de vezes em que ela ocorre,
            // aqui representada pelo valor res.
            MPI_Recv(&res, 1, MPI_INT, i, 0, intercomm, &st);
            printf("Master received %d from %d.\n", res, i);
        }
    } else{ // Caso haja um comunicador para o pai, significa que é um processo filho (escravo)
        int val; // variável que receberá o valor do mestre
        
        // O processo escravo recebe um valor do processo pai
        // No caso da lista invertida, esse valor seria uma palavra
        MPI_Recv(&val, 1, MPI_INT, 0, 0, parentcomm, &st);

        // Aqui, a operação de contar a quantidade de vezes em que a palavra
        // ocorre no texto é dada pela multiplicação de val por 10
        val = val*10;

        // O processo filho envia o valor alterado para o mestre
        MPI_Send(&val, 1, MPI_INT, 0, 0, parentcomm);
        
        printf("Sent value: %d\n", val);
        fflush(stdout);
    }

    MPI_Finalize();
}