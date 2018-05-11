#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

/*
1. 
Realizar un algoritmo Pthreads y otro OpenMP que resuelva la expresión:
	𝑀 = 𝑙.𝐴𝐵𝐶 + 𝑏𝐿𝐵𝐷 se que hay una multiplicacion por cada elemento y otra que es diferente para sets
Donde A, B, C y D son matrices de NxN. L matriz triangular inferior de NxN.
𝑏 y 𝑙 son los promedios de los valores de los elementos de las matrices B y L, respectivamente.
Evaluar N=512, 1024 y 2048.
*/

/* Time in seconds from some point in the past */
double dwalltime();

void producto(double *A,double *B,double *C, int r,int N,int sizeMatrix,int sizeBlock);
void productoPorElemento(double *A,double b,double *C, int n);

int main(int argc,char* argv[]){

 double *A,*B,*C,*D,*L,*M;
 int I,J,K,i,j,k;
 int despA, despB, despC,desp;
 double b,l;
 int check = 1;
 double timetick;

 //Controla los argumentos al programa
 if (argc < 3){
   printf("\n Falta un parametro ");
   printf("\n 1. Cantidad de bloques por dimension ");
   printf("\n 2. Dimension de cada bloque \n");
   return 0;
 }

 int N = atoi(argv[1]);
 int r = atoi(argv[2]);

 int n = N*r; //dimension de la matriz
 int sizeMatrix=n*n; //cantidad total de datos matriz
 int sizeBlock=r*r; //cantidad total de datos del bloque
 int sizeL = (n+1)*n/2;

 //Aloca memoria para las matrices
 A=(double*)malloc(sizeof(double)*sizeMatrix);
 B=(double*)malloc(sizeof(double)*sizeMatrix);
 C=(double*)malloc(sizeof(double)*sizeMatrix);
 D=(double*)malloc(sizeof(double)*sizeMatrix);
 L=(double*)malloc(sizeof(double)*sizeL);
 M=(double*)malloc(sizeof(double)*sizeMatrix);

 //Inicializa las matrices
 for(i=0;i<n;i++){
  for(j=0;j<n;j++){

   A[i*n+j]=1;
   B[i*n+j]=1;
   C[i*n+j]=1;
   D[i*n+j]=1;

   if(i>=j){
    L[i+j+i*(i-1)/2]=1;
   }

   M[i*n+j]=0;
  }
 }

 //inicializo b
 for(i=0; i<n; i++){
  for(j=0; j<n; j++){
   b += B[i*n+j];
  }
 }
 b /= sizeMatrix;

 //inicializo l
 for(i=0;i<n;i++){
  for(j=0;j<=i;j++){
    l += L[i+j+i*(i-1)/2];
  }
 }
 l /= sizeMatrix;

 //Resuelve la expresion 𝑀 = 𝑙.𝐴𝐵𝐶 + 𝑏𝐿𝐵𝐷
 timetick = dwalltime();

 //𝑙.𝐴𝐵𝐶
 //𝑙.𝐴 -> L
 for(i=0; i<n; i++){
  for(j=0; j<n; j++){
    A[i*n+j]= A[i*n+j]*l;
  }
 }
 //l𝐴𝐵 -> M
 for (I=0;I<N;I++){
   for (J=0;J<N;J++){
     despC = (I*N+J)*sizeBlock;
     for (K=0;K<N;K++){
       despA = (I*N+K)*sizeBlock;
       despB = (K*N+J)*sizeBlock;
       for (i=0;i<r;i++){
         for (j=0;j<r;j++){
           desp = despC + i*r+j;
           for (k=0;k<r;k++){
             M[desp] += A[despA + i*r+k]*B[despB + k*r+j]; 
           };
         }
       };
     };
   };  
 }; 
 //volver a poner a 0 o crear nueva matriz y inicializar en 0
 for(i=0; i<n; i++){
  for(j=0; j<n; j++){
  	A[i*n+j]=0;
  }
 }
 //lA𝐵𝐶 -> A
 for (I=0;I<N;I++){
   for (J=0;J<N;J++){
     despC = (I*N+J)*sizeBlock;
     for (K=0;K<N;K++){
       despA = (I*N+K)*sizeBlock;
       despB = (K*N+J)*sizeBlock;
       for (i=0;i<r;i++){
         for (j=0;j<r;j++){
           desp = despC + i*r+j;
           for (k=0;k<r;k++){
             A[desp] += M[despA + i*r+k]*C[despB + k*r+j]; 
           };
         }
       };
     };
   };  
 }; 
 //volver a poner a 0 o crear nueva matriz y inicializar en 0
 for(i=0; i<n; i++){
  for(j=0; j<n; j++){
  	M[i*n+j]=0;
  }
 }

 //𝑏𝐿𝐵𝐷
 //𝑏𝐿 -> L
 for(i=0; i<n; i++){
  for(j=0;j<=i;j++){
    L[i+j+i*(i-1)/2]= L[i+j+i*(i-1)/2]*b;
  }
 }
 //𝐿𝐵 -> M
 for(i=0;i<n;i++){
  for(j=0;j<n;j++){
   for(k=0;k<=i;k++){
    M[i*n+j]=M[i*n+j] + L[i+k+i*(i-1)/2] * B[j+k*n];
   }
  }
 }
 for(i=0; i<n; i++){
  for(j=0; j<n; j++){
  	B[i*n+j]=0;
  }
 }
 //𝐵𝐷 -> B
 for (I=0;I<N;I++){
   for (J=0;J<N;J++){
     despC = (I*N+J)*sizeBlock;
     for (K=0;K<N;K++){
       despA = (I*N+K)*sizeBlock;
       despB = (K*N+J)*sizeBlock;
       for (i=0;i<r;i++){
         for (j=0;j<r;j++){
           desp = despC + i*r+j;
           for (k=0;k<r;k++){
             B[desp] += M[despA + i*r+k]*D[despB + k*r+j]; 
           };
         }
       };
     };
   };  
 }; 

 //𝑙.𝐴𝐵𝐶 + 𝑏𝐿𝐵𝐷
 for(i=0; i<n; i++){
  for(j=0; j<n; j++){
   M[i*n+j] = A[i*n+j]+B[i*n+j];
  }
 }

 printf("Tiempo en segundos %f\n", dwalltime() - timetick);

  printf("Contenido de la matriz M: \n" );
  for (I= 0; I< N; I++){
    //para cada fila de bloques (I)
    for (i= 0; i< r; i++){
       for(J=0;J<N;J++){
       despB=(I*N+J)*r*r;
    for (j=0;j<r;j++){
       printf("%f ",M[despB+ i*r+j]);
  
     };//end for j
  };//end for J
        printf("\n ");
     };//end for i

  };//end for I
  printf(" \n\n");

 //Verifica el resultado
 for(i=0;i<n;i++){
  for(j=0;j<n;j++){
   check = check && (M[i*n+j]==(l*n*n+n*(i+1)));
  }
  printf("\n");
 }

 if(check){
  printf("nancy/10\n");
 }else{
  printf("Re mal\n");
 }

 free(A);
 free(B);
 free(C);
 free(D);
 free(L);
 free(M);

 return(0);
}



/*****************************************************************/

#include <sys/time.h>

double dwalltime()
{
	double sec;
	struct timeval tv;

	gettimeofday(&tv,NULL);
	sec = tv.tv_sec + tv.tv_usec/1000000.0;
	return sec;
}

