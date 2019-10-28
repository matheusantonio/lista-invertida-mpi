// Protótipo para o programa de criação de listas invertidas utilizando MPI

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<mpi.h>

// Esse número de processos será definido dinamicamente a partir da quantidade de palavras no texto
#define NUM_SPAWNS 5

typedef struct chave{
    char valor[30];
    int ordem;
    struct chave *prox;
}Chave;

typedef struct chaveiro{
    Chave *c;
    struct chaveiro *prox;
}Chaveiro;

void tratarDados(char *frase){
    char caracteresIgnorados[] = {'?','!','.',','};
    int reescrever=0;
    int i=0,j;
    while(1){
        frase[i-reescrever] = frase[i];
        for(j=0;j<4;j++){
            if(frase[i] == caracteresIgnorados[j]){
                reescrever++;
            }
        }
        if(frase[i]=='\0'){break;}
        i++;
    }   
}

Chave *copiar(Chave *original){
    if(original == NULL) {return NULL;}
    int i=0;
    Chave *copia = (Chave*) malloc(sizeof(Chave));
    copia -> ordem = original -> ordem;
    do{
        copia -> valor[i] = original -> valor[i];
        i++;
    }
    while(original->valor[i]!='\0');
    copia->prox = copiar(original->prox);
    return copia;
}

Chave *separar(char *frase){
    Chave *head = (Chave*) malloc(sizeof(Chave));
    head -> prox = NULL;
    Chave *atual = head;
    int ordem = 1;
    int i=0,j=0;
    do{
        if(frase[i] == ' '){
            atual -> valor[j]='\0';
            atual -> prox = (Chave*) malloc(sizeof(Chave));
            atual -> ordem = ordem++;
            atual = atual -> prox;
            atual -> prox = NULL;
            i++;
            j=0;
        }
        atual->valor[j++] = frase[i++];
    }
    while(frase[i]!='\0');
    return head;
}


int comparar(char *val1, char *val2){
    int i=-1;
    do{
        i++;
        if(val1[i]!=val2[i])
            return 0;
    }
    while(val1[i]!='\0');
    return 1;
}

Chave *entradasUnicas(Chave *chaves,int *numero){
    Chave *salvar=chaves;
    Chave *atual = NULL;
    Chave *eliminar;
    int num=0;
    while(chaves!=NULL){
        if(chaves->valor[0]!='!'){
            num++;
            atual = chaves -> prox;
            while(atual!=NULL){
                if(comparar(chaves->valor,atual->valor)){
                    atual->valor[0]='!';
                }   
                atual = atual->prox;
            }
        }  
        chaves = chaves -> prox;
    }
    chaves = salvar;
    while(chaves != NULL && chaves->prox!=NULL){
        if(chaves->prox->valor[0]=='!'){
            eliminar = chaves ->prox;
            if(chaves -> prox != NULL)
                chaves -> prox = chaves -> prox -> prox;
            else
                chaves -> prox = NULL;
            free(eliminar);
        }
        else{
            chaves = chaves->prox;
        }
    }
    *numero = num;
    return salvar;
}


int *inverterLista(char *texto,char *palavra, int* tam){
    printf("DENTRO DA FUNCAO: frase: {%s}, palavra:{%s}\n", texto, palavra);
    int i=0,j=0;
    int num=0;
    int total;
    int atual=1;
    int *vetor = (int*) malloc(sizeof(int));
    char aux[30];
    while(texto[i]!='\0'){
        if(texto[i]==palavra[0]){
            while(texto[i]!=' ' && texto[i]!='\0'){
                aux[j] = texto[i];
                i++;
                j++;
            }
            aux[j]='\0';
            j=0;
            if(comparar(palavra,aux)){
                num++;
                vetor = realloc(vetor,num*sizeof(int));
                vetor[num-1]=atual;
            }
            
        }
        else{
            while(texto[i]!=' '){i++;}
        }
        atual++;
        i++;
    }
    num++;
    vetor = realloc(vetor,num*sizeof(int));
    vetor[num-1]=-1;
    *tam = num;
    return vetor;
}


void imprimir(int *vetor){
    int i;
    for(i=0;vetor[i]!=-1;i++){
        printf("%d ",vetor[i]);
        fflush(stdout);
    }
}

int main(int argc, char *argv[]){

    char frase[1000];
    int tamFrase, tamWord;
    
    
    MPI_Comm parentcomm, intercomm;
    MPI_Status st; // estrutura padrão do MPI, não é relevante para esse exemplo mas precisa ser atribuída.

    MPI_Init(&argc, &argv);
    MPI_Comm_get_parent(&parentcomm); // verifica qual é o comunicador para o processo pai

    if(parentcomm == MPI_COMM_NULL){ // se não houver comunicador para o pai, ele é o pai
        printf("Master process.\n"); // exibe que é o processo mestre
        fflush(stdout);

        Chave *palavras;
        int qtde;
        //printf("Digite frase: ");
        //scanf("%[^\n]",frase);
        strcpy(frase, "uma frase legal");
        //tratarDados(frase);
        //printf("%s",frase);
        tamFrase = strlen(frase);
        palavras = separar(frase);
        Chave *copy = entradasUnicas(copiar(palavras),&qtde);
        //imprimir(copy);
        free(palavras);
        int np = qtde; // número de processos que serão criados
        printf("quantidade: %d\n", np);

        // Cria várias instâncias de processos do arquivo invertex.e
        // intercomm é o comunicador que o pai utilizará para comunicar-se com os processos filhos.
        MPI_Comm_spawn("./invertex.e", MPI_ARGV_NULL, np, MPI_INFO_NULL, 0, MPI_COMM_WORLD, &intercomm, MPI_ERRCODES_IGNORE);

        //int i=0;
        //while(copy!=NULL)
        for(int i=0;i<np;i++){
            
            //imprimir(inverterLista(frase,copy->valor));
            
            char word[30];
            strcpy(word, copy->valor);
            //fflush(stdout);
            tamWord = strlen(word);
            printf("\n%s(%d): ",word, tamWord);
            fflush(stdout);
            
            MPI_Send(&tamWord, 1, MPI_INT, i, 0, intercomm); // passa o tamanho da palavra
            MPI_Send(&word, tamWord, MPI_CHAR,i, 0, intercomm); // passa a palavra
            MPI_Send(&tamFrase, 1, MPI_INT, i, 0, intercomm); // passa o tamanho da frase
            MPI_Send(&frase, tamFrase, MPI_CHAR, i, 0, intercomm); // passa a frase

            int tam, val[30];

            MPI_Recv(&tam, 1, MPI_INT, i, 0, intercomm, &st); // recebe o tamanho do vetor
            MPI_Recv(&val, tam, MPI_INT, i, 0, intercomm, &st); // recebe o vetor

            imprimir(val);
            printf("\n");
            fflush(stdout);

            copy = copy->prox;
        }

        /*for(int i=0;i<np;i++){
            // Para cada processo filho, envia um valor (nesse caso, é o número do próprio processo)
            // Os inteiros representam as palavras que serão encontradas pelo mestre
            MPI_Send(&i, 1, MPI_INT, i, 0, intercomm);

            int res;
            // O mestre irá receber, de cada palavra, a quantidade de vezes em que ela ocorre,
            // aqui representada pelo valor res.
            MPI_Recv(&res, 1, MPI_INT, i, 0, intercomm, &st);
            printf("Master received %d from %d.\n", res, i);
        
        }*/
    } else{ // Caso haja um comunicador para o pai, significa que é um processo filho (escravo)
        
        char word[30];

        MPI_Recv(&tamWord, 1, MPI_INT, 0, 0, parentcomm, &st); // recebe o tamanho da palavra
        MPI_Recv(&word, tamWord, MPI_CHAR, 0,0, parentcomm, &st); // recebe a palavra
        
        MPI_Recv(&tamFrase, 1, MPI_INT, 0, 0, parentcomm, &st); // recebe o tamanho da frase
        MPI_Recv(&frase, tamFrase, MPI_CHAR, 0, 0, parentcomm, &st); //recebe a frase
        
        //printf("\nValues received.\nRecebido: {%s} - %d \nFrase: {%s}\n",word, tamWord, frase);
        //fflush(stdout);

        int tam;
        int *res = inverterLista(frase, word, &tam);
        //imprimir(inverterLista(frase, word, &tam));
        
        printf("(slave) pos: ");
        imprimir(res);
        printf("\n");
        fflush(stdout);


        MPI_Send(&tam, 1, MPI_INT, 0, 0, parentcomm); // passa o tamanho do vetor
        MPI_Send(res, tam, MPI_INT, 0, 0, parentcomm); // passa o vetor
        
        /*
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
        */
    }

    MPI_Finalize();

    return 0;
}