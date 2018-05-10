#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>

#define NUM_THREADS 5

/*
1. 
Realizar un algoritmo Pthreads y otro OpenMP que resuelva la expresión:
	𝑀 = 𝑙.𝐴 𝐵𝐶 + 𝑏𝐿 𝐵𝐷
Donde A, B, C y D son matrices de NxN. L matriz triangular inferior de NxN.
𝑏 y 𝑙 son los promedios de los valores de los elementos de las matrices B y L, respectivamente.
Evaluar N=512, 1024 y 2048.
*/

/*
HILO1 hace l*A
HILO2 hace B*C
HILO3 b*L
HILO4 B*D
HILO1 tiene barrera que espera que termine HILO2 y multiplica resultados
HILO3  tiene barrera que espera que termine HILO4 y multiplica resultados
HILO1 barrera que espera al 3 y suma.

Los hilos los podemos hacer asi:
Crea HILO0_n que inicializa con barrier al final, n es cada matriz
HILO1 hace l*A
 Crea HILOS para hacer la multiplicacion como en la diapositiva
De la misma forma para los HILOS 2 3 4.
Lo subo a git
*/

int sizeMatrix, sizeL, n, b, l, N, r, sizeBlock;
double *A,*B,*C,*D,*L,*M,*O;
sem_t sem_lABC, sem_bLBD;

/* Time in seconds from some point in the past */
double dwalltime();

void producto(double *A,double *B,double *C, int r,int N,int sizeMatrix,int sizeBlock);
void productoPorElemento(double *A,double b,double *C, int n);
void * initialize (void * ptr);
void * lA (void * ptr);
void * BC (void * ptr);
void * bL (void * ptr);
void * BD (void * ptr);

int main(int argc,char* argv[]){

 int i,j,k,ids[NUM_THREADS];
 pthread_attr_t attr;
 pthread_t threads[NUM_THREADS];
 double timetick;

 sem_init(&sem_lABC, 1, 0); 
 sem_init(&sem_bLBD, 1, 0); 

 pthread_attr_init(&attr);
 
 //Controla los argumentos al programa
 if (argc < 3){
   printf("\n Falta un parametro ");
   printf("\n 1. Cantidad de bloques por dimension ");
   printf("\n 2. Dimension de cada bloque \n");
   return 0;
 }

 N = atoi(argv[1]);
 r = atoi(argv[2]);

 n = N*r; //dimension de la matriz
 sizeMatrix=n*n; //cantidad total de datos matriz
 sizeBlock=r*r; //cantidad total de datos del bloque
 sizeL = (n+1)*n/2;

 A=(double*)malloc(sizeof(double)*sizeMatrix);
 B=(double*)malloc(sizeof(double)*sizeMatrix);
 C=(double*)malloc(sizeof(double)*sizeMatrix);
 L=(double*)malloc(sizeof(double)*sizeL);
 M=(double*)malloc(sizeof(double)*sizeMatrix);
 D=(double*)malloc(sizeof(double)*sizeMatrix);
 O=(double*)malloc(sizeof(double)*sizeMatrix);

 for (i = 0; i < NUM_THREADS; i++){
 	ids[i] = i;
 	pthread_create(&threads[i], &attr, initialize, &ids[i]);
 }

 for (i = 0; i<NUM_THREADS;i++)
 	pthread_join(threads[i], NULL);

 //Resuelve la expresion 𝑀 = 𝑙.𝐴𝐵𝐶 + 𝑏𝐿𝐵𝐷
 timetick = dwalltime();

 //𝑙.𝐴𝐵𝐶
 pthread_create(&threads[0], &attr, lA, &ids[0]);
 pthread_create(&threads[1], &attr, BC, &ids[1]);

 //𝑏𝐿𝐵𝐷
 pthread_create(&threads[2], &attr, bL, &ids[2]);
 pthread_create(&threads[3], &attr, BD, &ids[3]);

 for (i = 0; i<4;i++)
 	pthread_join(threads[i], NULL);

 //𝑙.𝐴𝐵𝐶 + 𝑏𝐿𝐵𝐷
 for(i=0;i<sizeMatrix;i++){
  M[i] = C[i]+D[i];
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

 for (i=0; i<sizeMatrix ;i++)
	  C[i]=0.0;
 
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
 for(i=0;i<sizeMatrix;i++){
    C[i]= A[i]*b;
 }
}

void * lA (void * ptr){
	int * p, i, id;
	p = (int *) ptr;
	id = *p;
    productoPorElemento(A,l,A,sizeMatrix);
    sem_wait(&sem_lABC);
    for(i=0;i<sizeMatrix;i++){
 	  C[i]=0;
    }
    producto(M,A,C,r,N,sizeMatrix,sizeBlock);    
    pthread_exit(0);
}
void * BC (void * ptr){
	int * p, i, id;
	p = (int *) ptr;
	id = *p;
    producto(B,C,M,r,N,sizeMatrix,sizeBlock);
    sem_post(&sem_lABC);
    pthread_exit(0);
}
void * bL (void * ptr){
	int * p, i, j, k, id;
	p = (int *) ptr;
	id = *p;
    productoPorElemento(L,b,L,sizeL);
    sem_wait(&sem_bLBD);
    for(i=0;i<sizeMatrix;i++){
 	  D[i]=0;
    }
	for(i=0;i<n;i++){
	 for(j=0;j<n;j++){
	   for(k=n;k>=i;--k){
	    D[i*n+j]=D[i*n+j] + O[i*n+k]*L[k+j+k*(k-1)/2];
	   }
	 }
    }
    pthread_exit(0);
}
void * BD (void * ptr){
	int * p, i, id;
	p = (int *) ptr;
	id = *p;
    producto(B,D,O,r,N,sizeMatrix,sizeBlock);
    sem_post(&sem_bLBD);
    pthread_exit(0);
}

void * initialize (void * ptr){
	int * p, i, id;
	p = (int *) ptr;
	id = *p;

	switch(id) {

	   case 0  :
 		  for(i=0;i<sizeMatrix;i++){
		    A[i]=rand()%10;
 		  }
	      break; /* optional */
		
	   case 1  :
 		  for(i=0;i<sizeMatrix;i++){
		    B[i]=rand()%10;
 		  }
		  //inicializo b
		  for(i=0; i<sizeMatrix; i++){
		    b += B[i];
		  }
		  b /= sizeMatrix;
	      break; /* optional */

	   case 2  :
 		  for(i=0;i<sizeMatrix;i++){
		    C[i]=rand()%10;
 		  }
	      break; /* optional */
	  
 	   case 3  :
		  for(i=0;i<sizeL;i++){
		     L[i]=n;
		  }
		  //inicializo l
		  for(i=0; i<sizeL; i++){
		   l += L[i];
		  }
		  l /= sizeL;
		  break;

	   case 4  :
 		  for(i=0;i<sizeMatrix;i++){
		    M[i]=0;
		    O[i]=0;
 		  }
	      break; /* optional */

	   case 5  :
 		  for(i=0;i<sizeMatrix;i++){
		    D[i]=rand()%10;
 		  }
	      break; /* optional */
	}
    pthread_exit(0);
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

