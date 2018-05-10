#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <omp.h>

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
 int i,j,k;
 int b,l;
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
 #pragma omp parallel for schedule(static) default(shared) private(i)
   for(i=0;i<n;i++){
    #pragma omp parallel for schedule(static) default(shared) private(j)
    for(j=0;j<n;j++){

     A[i*n+j]=rand()%10;
     B[i*n+j]=rand()%10;
     C[i*n+j]=rand()%10;
     D[i*n+j]=rand()%10;

     if(i>=j){
      L[i+j+i*(i-1)/2]=n;
     }

     M[i*n+j]=0;	
    }
   }
 
 //inicializo b
 #pragma omp parallel for schedule(static) default(shared) private(i) reduction(+:b)
 for(i=0; i<sizeMatrix; i++){
   b += B[i];
 }
 b /= sizeMatrix;

 //inicializo l
 #pragma omp parallel for schedule(static) default(shared) private(i) reduction(+:l)
 for(i=0; i<sizeL; i++){
  l += L[i];
 }
 l /= sizeL;

 //Resuelve la expresion 𝑀 = 𝑙.𝐴𝐵𝐶 + 𝑏𝐿𝐵𝐷
 timetick = dwalltime();

 //𝑙.𝐴𝐵𝐶
 productoPorElemento(A,l,A,sizeMatrix);
 producto(A,B,M,r,N,sizeMatrix,sizeBlock);
 //volver a poner a 0 o crear nueva matriz y inicializar en 0
 #pragma omp parallel for schedule(static) default(shared) private(i)
 for(i=0;i<sizeMatrix;i++){
 	A[i]=0;
 }
 producto(M,C,A,r,N,sizeMatrix,sizeBlock);
 //volver a poner a 0 o crear nueva matriz y inicializar en 0
 #pragma omp parallel for schedule(static) default(shared) private(i)
 for(i=0;i<sizeMatrix;i++){
 	M[i]=0;
 }

 //𝑏𝐿𝐵𝐷
 productoPorElemento(L,b,L,sizeL);
 #pragma omp parallel for schedule(static) default(shared) private(i)
 for(i=0;i<n;i++){
  #pragma omp parallel for schedule(static) default(shared) private(j)
  for(j=0;j<n;j++){
   #pragma omp parallel for schedule(static) default(shared) private(k)
   for(k=n;k>=i;--k){
    M[i*n+j]=M[i*n+j] + B[i*n+k]*L[k+j+k*(k-1)/2];
   }
  }
 }
 #pragma omp parallel for schedule(static) default(shared) private(i)
 for(i=0;i<sizeMatrix;i++){
 	B[i]=0;
 }
 producto(M,D,B,r,N,sizeMatrix,sizeBlock);

 //𝑙.𝐴𝐵𝐶 + 𝑏𝐿𝐵𝐷
 #pragma omp parallel for schedule(static) default(shared) private(i)
 for(i=0;i<sizeMatrix;i++){
  M[i] = A[i]+B[i];
 } 

 printf("Tiempo en segundos %f\n", dwalltime() - timetick);

 free(A);
 free(B);
 free(C);
 free(D);
 free(L);
 free(M);

 return(0);
}

//SOLO PARA MATRICES DE IGUAL DIMENSION DE BLOQUES (N)
void producto(double *A,double *B,double *C, int r,int N,int sizeMatrix, int sizeBlock){
   int I,J,K,i,j,k;
   int despA, despB, despC,desp;

  #pragma omp parallel for schedule(static,N/3) default(shared) private(I)
	for (I=0;I<N;I++){
    #pragma omp parallel for schedule(static,N/3) default(shared) private(J)
		for (J=0;J<N;J++){
			despC = (I*N+J)*sizeBlock;
      #pragma omp parallel for schedule(static,N/3) default(shared) private(K)
			for (K=0;K<N;K++){
				despA = (I*N+K)*sizeBlock;
				despB = (K*N+J)*sizeBlock;
        #pragma omp parallel for schedule(static,r/3) default(shared) private(i)
				for (i=0;i<r;i++){
          #pragma omp parallel for schedule(static,r/3) default(shared) private(j)
					for (j=0;j<r;j++){
						desp = despC + i*r+j;
            #pragma omp parallel for schedule(static,r/3) default(shared) private(k)
						for (k=0;k<r;k++){
							C[desp] += A[despA + i*r+k]*B[despB + k*r+j]; 
						};
					}
				};
			};
		};	
	}; 
}

void productoPorElemento(double *A,double b,double *C, int sizeMatrix){
 int i;
 #pragma omp parallel for schedule(static,sizeMatrix/3) default(shared) private(i)
 for(i=0;i<sizeMatrix;i++){
    C[i]= A[i]*b;
 }
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

