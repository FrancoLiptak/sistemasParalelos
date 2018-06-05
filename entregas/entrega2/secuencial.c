#include<stdio.h>
#include<stdlib.h>
#include <sys/time.h>

//Dimension por defecto de las matrices
int N = 128;

//Para calcular tiempo
double dwalltime(){
  double sec;
  struct timeval tv;

  gettimeofday(&tv,NULL);
  sec = tv.tv_sec + tv.tv_usec/1000000.0;
  return sec;
}

int main(int argc,char*argv[]){
    int *queens, *col_available, *asc_diagonal, *des_diagonal;
    int i,j;
    int num_solutions = 0;
    int check=1;
    double timetick;
    int not_found = 1;
    int solution_exists = 1;

    //Controla los argumentos al programa
    if ((argc != 2) || ((N = atoi(argv[1])) <= 0) ){
        printf("\nUsar: %s n\n  n: Dimension de la matriz (nxn X nxn)\n", argv[0]);
        exit(1);
    }

    //Aloca memoria para las matrices
    queens=(int*)malloc(sizeof(int)*N);
    col_available=(int*)calloc(N, sizeof(int));
    asc_diagonal=(int*)malloc(sizeof(int)*N);
    des_diagonal=(int*)malloc(sizeof(int)*N);

    // Inicializo queens en -1 
    for(i=0; i<N; i++){
        queens[i] = i;
    }

    timetick = dwalltime(); //Empieza a controlar el tiempo

    //Coloca las reinas
    i = 0;
    while( (solution_exists == 1) && (i < N) ){
        j = 0;
        while( (not_found == 1) && (j < N) ){
            if(col_available[j] == 0){
                if( (asc_diagonal[i+j] == 0) && (des_diagonal[-(i-j-N-1)] == 0) && (queens[i] < j) ){ //Para probar las diagonales
                    queens[i] = j;
                    col_available[j] = 1;
                    asc_diagonal[i+j] = 1;
                    des_diagonal[-(i-j-N-1)] = 1;
                    not_found = 0;
                } 
            }
            j++;
        }

        if( (not_found == 1) ){ // Si encuentra o no encuentra solucion tiene que liberar. si no encuentra la proxima vuelve otro mas para atras
            i--;
        }else{
            if( !(i == N-1) ){
                i++;
            }
        }

        // Falta que el algoritmo pare
    }

    free(queens);
    free(col_available);
    free(asc_diagonal);
    free(des_diagonal);
    return(0);
}