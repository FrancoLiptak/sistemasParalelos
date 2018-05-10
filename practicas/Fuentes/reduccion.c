#include<stdio.h>
#include<stdlib.h>
#include <math.h>
#include <stdbool.h>

int N=100;

//Para calcular tiempo
double dwalltime(){
        double sec;
        struct timeval tv;

        gettimeofday(&tv,NULL);
        sec = tv.tv_sec + tv.tv_usec/1000000.0;
        return sec;
}

 /* Function to check if x is power of 2*/
 bool isPowerOfTwo(int n)
 {
    return (ceil(log2(n)) == floor(log2(n)));
 }
 
int main(int argc,char* argv[]){

 double *A;
 int i,j;
 int row;
 double timetick;

 //Controla los argumentos al programa
 if ((argc != 2) || ((N = atoi(argv[1])) <= 0) || (!isPowerOfTwo(N))) {
    printf("\nUsar: %s n\n  n: Dimension del vector (n) potencia de 2\n", argv[0]);
    exit(1);
  }
 
 //Aloca memoria para el vector
 A=(double*)malloc(sizeof(double)*N);

 //Inicializa el vector con numeros potencia de 2
 for(i=0;i<N;i++){
  A[i]=pow(2,rand()%10);
  printf("%f ", A[i]);
 }
    printf("\n");

 //Realiza la reduccion 
 timetick = dwalltime();

 for(i=N/2;i>=1;i=i/2){ 
  printf("Vuelta \n");
  for(j=0;j<i;j++){
   row = j*2;
   A[j]=A[row]/A[row+1];
   printf("A[%i]", j);
   printf("%f \n", A[j]);
  }
 }

 printf("Tiempo en segundos %f\n", dwalltime() - timetick);

 //Verifica el resultado
 //for(i=0;i<N;i++){
 // for(j=0;j<N;j++){
 //  check = check && (C[i*N+j]==N);
 // }
 //}

 printf("Reduccion del vector: %f\n", A[0]);
 
 free(A);

 return(0);
}
