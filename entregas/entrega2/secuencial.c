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
    int backtrack = 0;
    int check=1;
    int queens_final = 0;
    double timetick;
    int not_found = 1;

    //Controla los argumentos al programa
    if ((argc != 2) || ((N = atoi(argv[1])) <= 0) ){
        printf("\nUsar: %s n\n  n: Dimension de la matriz (nxn X nxn)\n", argv[0]);
        exit(1);
    }

    //Aloca memoria para las matrices
    queens=(int*)malloc(sizeof(int)*N);
    col_available=(int*)calloc(N, sizeof(int));
    asc_diagonal=(int*)malloc(sizeof(int)*(N-1)*2+1);
    des_diagonal=(int*)malloc(sizeof(int)*(N-1)*2+1);

    // Inicializo queens en -1 
    for(i=0; i<N; i++){
        queens[i] = -1;
    }

    timetick = dwalltime(); //Empieza a controlar el tiempo

    //Coloca las reinas
    i = 0;
    while( queens_final == 0 ){
        j = 0;
        not_found = 1;
        while( (not_found == 1) && (j < N) ){
            if ( (col_available[j] == 0) // la columna no tiene otra reina
                && (asc_diagonal[i+j] == 0) // la diagonal ascendente no tiene otra reina
                && (des_diagonal[(N-1)-(i-j)] == 0) // la diagonal descendente no tiene otra reina
                && (queens[i] < j) ){ // no vuelve a un casillero ya elejido
                    if ( backtrack == 1 ){
                        // libero lo que tenia asignado la vuelta pasada
                        col_available[queens[i]] = 0;
                        asc_diagonal[i+queens[i]] = 0;
                        des_diagonal[(N-1)-(i-queens[i])] = 0;
                    }
                    queens[i] = j;
                    col_available[j] = 1;
                    asc_diagonal[i+j] = 1;
                    des_diagonal[(N-1)-(i-j)] = 1;
                    not_found = 0;
                    backtrack = 0;
            }
            j++;
        }

        // Si no encuentra solucion entonces no tiene solucion el problema o llego al ultimo valido de esa reina en esa rama
        // debe ir a la reina anterior
        if( not_found == 1 ){ 
            // Si es el ultimo valido de la rama entonces tiene que liberar antes de subir
            if (queens[i] != -1){
                // Libero lo que tenia asignado la vuelta pasada
                col_available[queens[i]] = 0;
                asc_diagonal[i+queens[i]] = 0;
                des_diagonal[(N-1)-(i-queens[i])] = 0;
                queens[i] = -1;
                if (i == 0){
                    queens_final = 1;
                }
            }
            backtrack = 1;
            i--;
        }else{
            // Si no termino entonces sigue avanzando
            // Si termino entonces prueba con otra columna para la reina (se repite la busqueda para la misma reina)
            if( i != N-1 ){
                i++;
            }else{
                // imprimir
                num_solutions++;
            }
        }

    }

    printf("Tiempo en segundos %f\n", dwalltime() - timetick); // Informamos el tiempo

    printf("Resultado: %d\n", num_solutions);

    free(queens);
    free(col_available);
    free(asc_diagonal);
    free(des_diagonal);
    return(0);
}