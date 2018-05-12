#include<stdio.h>
#include<stdlib.h>
#include<sys/time.h>

// Declaraciones de variables
int N = 2048; // Dimensión por defecto del arreglo
int *vector;
int pares = 0; // Cantidad de números pares

// Para calcular tiempo
double dwalltime(){
    double sec;
    struct timeval tv;

    gettimeofday(&tv,NULL);
    sec = tv.tv_sec + tv.tv_usec/1000000.0;
    return sec;
}

int main(int argc,char*argv[]){

    double timetick;
    int i;

    //Controla los argumentos al programa
    if ((argc != 2) || ((N = atoi(argv[1])) <= 0) ){
        printf("Debe ingresar la longitud del arreglo. \n");
        exit(1);
    }

    // Alocación de memoria, e inicializiación del vector
    vector = (int*)malloc(sizeof(int)*N);
    for(i=0; i<N; i++){
        vector[i] = i;
    }

    timetick = dwalltime(); // Empieza a contar el tiempo

    // Cuenta la cantidad de números pares
    for(i=0; i<N; i++){
        if(vector[i] % 2 == 0) pares++;
    }

    printf("Tiempo en segundos %f\n", dwalltime() - timetick); // Informa el tiempo

    // Verifica el resultado
    if (pares == div(N+1, 2).quot){
        printf("Resultado correcto\n");
        printf("La cantidad de números pares es: %d\n", pares);
    }else{
        printf("Resultado erroneo\n");
    }

    free(vector);
    return(0);
}