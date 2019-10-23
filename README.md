# lista-invertida-mpi

Trabalho para a disciplina de Sistemas Distribuídos
A implementação da lista invertida ainda não está sendo feita, apenas a base para a utilização do MPI.

Para compilar:
mpicc invertex.c -o invertex.e

Para executar:
mpirun ./invertex.e

Multiplicação de matrizes: Implementação em MPI de uma multiplicação distribuída de matrizes.

Para compilar:
mpicc matrixmul.c -o matrixmul.e

Para executar:
mpirun -n [numero de processos] ./matrixmul.e