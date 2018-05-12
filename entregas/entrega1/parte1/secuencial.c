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

//cantidad de bloques
 int N = atoi(argv[1]);
//tamaño de bloques
 int r = atoi(argv[2]);

 int n = N*r; //dimension de la matriz
 int sizeMatrix=n*n; //cantidad total de datos matriz
 int sizeBlock=r*r; //cantidad total de datos del bloque
 int sizeL = (n+1)*n/2;
 int sizeTriangularBlock = (r+1)*r/2; //tamaño de bloque triangular

 //Aloca memoria para las matrices
 A=(double*)malloc(sizeof(double)*sizeMatrix);
 B=(double*)malloc(sizeof(double)*sizeMatrix);
 C=(double*)malloc(sizeof(double)*sizeMatrix);
 D=(double*)malloc(sizeof(double)*sizeMatrix);
 L=(double*)malloc(sizeof(double)*sizeL);
 M=(double*)malloc(sizeof(double)*sizeMatrix);

  //inicializacion de matrices
  despA = 0;
  for (I= 0; I< N; I++){
    //para cada fila de bloques (I)
    for(J=0;J<N;J++){
      if(I>=1)
        //para moverse en bloques de L
        despA = I*sizeTriangularBlock+(I-1+J)*sizeBlock;
      //para moverse en bloques de otros
      despB=(I*N+J)*r*r;
      //adentro del bloque
      for (i= 0; i< r; i++){
        for (j=0;j<r;j++){
          A[despB+ i*r+j]=1;
          B[despB+ i*r+j]=1;
          C[despB+ i*r+j]=1;
          D[despB+ i*r+j]=1;
          M[despB+ i*r+j]=0;
          //para no llegar a bloques con 0
          if(J<=I){
            if(I==J){
              if(i>=j){
                L[despA+ i+j+i*(i-1)/2]=1;
              }            
            }else{
              L[despA+ i*r+j]=1;            
            }
          }
        };//end for j
      };//end for J
    };//end for i
  };//end for I

 //inicializo b
  for (I= 0; I< N; I++){
    //para cada fila de bloques (I)
    for(J=0;J<N;J++){
      despB=(I*N+J)*r*r;
      for (i= 0; i< r; i++){
        for (j=0;j<r;j++){
          b += B[despB+ i*r+j];
        };//end for j
      };//end for J
    };//end for i
  };//end for I
  b /= sizeMatrix;

 //inicializo l
  despA = 0;
  for (I= 0; I< N; I++){
    //para cada fila de bloques (I)
    for(J=0;J<=I;J++){
      if(I>=1)
        despA = I*sizeTriangularBlock+(I-1+J)*sizeBlock;
      for (i= 0; i< r; i++){
        for (j=0;j<r;j++){
          if(I==J){
            if(i>=j){
              l += L[despA+ i+j+i*(i-1)/2];
            }            
          }else{
            l += L[despA+ i*r+j];            
          }
        };//end for j
      };//end for J
    };//end for i
  };//end for I
  l /= sizeMatrix;

 //Resuelve la expresion 𝑀 = 𝑙.𝐴𝐵𝐶 + 𝑏𝐿𝐵𝐷
 timetick = dwalltime();

 //𝑙.𝐴𝐵𝐶
 //𝑙.𝐴 -> L
  for (i= 0; i<sizeMatrix; i++){
    A[i] *= l;
  }
/*
  for (I= 0; I< N; I++){
    //para cada fila de bloques (I)
    for(J=0;J<N;J++){
      despB=(I*N+J)*r*r;
      for (i= 0; i< r; i++){
        for (j=0;j<r;j++){
          A[despB+ i*r+j]= A[despB+ i*r+j]*l;
        };//end for j
      };//end for J
    };//end for i
  };//end for I
*/
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
  
  for (i= 0; i<sizeMatrix; i++){
    A[i]= 0;
  }

/*
  for (I= 0; I< N; I++){
    //para cada fila de bloques (I)
    for(J=0;J<N;J++){
      despB=(I*N+J)*r*r;
      for (i= 0; i< r; i++){
        for (j=0;j<r;j++){
          A[despB+ i*r+j]= 0;
        };//end for j
      };//end for J
    };//end for i
  };//end for I
*/

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
  for (i= 0; i<sizeMatrix; i++){
    M[i]= 0;
  }
/*
  for (I= 0; I< N; I++){
    //para cada fila de bloques (I)
    for(J=0;J<N;J++){
      despB=(I*N+J)*r*r;
      for (i= 0; i< r; i++){
        for (j=0;j<r;j++){
          M[despB+ i*r+j]= 0;
        };//end for j
      };//end for J
    };//end for i
  };//end for I
*/

 //𝑏𝐿𝐵𝐷
 //𝑏𝐿 -> L
  despA = 0;
  for (I= 0; I< N; I++){
    //para cada fila de bloques (I)
    for(J=0;J<N;J++){
      if(I>=1)
        despA = I*sizeTriangularBlock+(I-1+J)*sizeBlock;
      for (i= 0; i< r; i++){
        for (j=0;j<r;j++){
          if(I==J){
            if(i>=j){
              L[despA+ i+j+i*(i-1)/2] = L[despA+ i+j+i*(i-1)/2] * b;
            }            
          }else{
            L[despA+ i*r+j] = L[despA+ i*r+j] * b;            
          }
        };//end for j
      };//end for J
    };//end for i
  };//end for I

 //𝐿𝐵 -> M
 for (I=0;I<N;I++){
   for (J=0;J<N;J++){
     despC = (I*N+J)*sizeBlock;
     despA = 0;
     //para no ir a bloques vacios K<=I
     for (K=0;K<=I;K++){
       if(I>=1){
        despA = I*sizeTriangularBlock+(I-1+K)*sizeBlock;
       }
       despB = (K*N+J)*sizeBlock;
       for (i=0;i<r;i++){
         for (j=0;j<r;j++){
            //donde se para para ir guardando
            desp = despC + i*r+j;
            if(I==K){
             for(k=0;k<=i;k++){
               M[desp] += L[despA + i+k+i*(i-1)/2] * B[despB + k*r+j];
             };
            }else{
             for (k=0;k<r;k++){
               M[desp] += L[despA + i*r+k] * B[despB + k*r+j];
             };            
            }
         }
       };
     };
   };  
 }; 

  for (i= 0; i<sizeMatrix; i++){
    B[i]= 0;
  }
/*
  for (I= 0; I< N; I++){
    //para cada fila de bloques (I)
    for(J=0;J<N;J++){
      despB=(I*N+J)*r*r;
      for (i= 0; i< r; i++){
        for (j=0;j<r;j++){
          B[despB+ i*r+j]= 0;
        };//end for j
      };//end for J
    };//end for i
  };//end for I
*/

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



 printf("Tiempo en segundos %f\n", dwalltime() - timetick);

 //𝑙.𝐴𝐵𝐶 + 𝑏𝐿𝐵𝐷
  for (i= 0; i<sizeMatrix; i++){
    M[i] = A[i] + B[i];
  }
/*
  for (I= 0; I< N; I++){
    //para cada fila de bloques (I)
    for(J=0;J<N;J++){
      despB=(I*N+J)*r*r;
      for (i= 0; i< r; i++){
        for (j=0;j<r;j++){
          M[despB+ i*r+j] = A[despB+ i*r+j] + B[despB+ i*r+j];
        };//end for j
      };//end for J
    };//end for i
  };//end for I
*/

/* para imprimir matrices sacar el comentario
  printf("Contenido de la matriz lA𝐵𝐶: \n" );
  for (I= 0; I< N; I++){
    //para cada fila de bloques (I)
    for (i= 0; i< r; i++){
       for(J=0;J<N;J++){
       despB=(I*N+J)*r*r;
    for (j=0;j<r;j++){
       printf("%f ",A[despB+ i*r+j]);
  
     };//end for j
  };//end for J
        printf("\n ");
     };//end for i

  };//end for I
  printf(" \n\n");

  printf("%f \n",l);
  printf("Contenido de la matriz 𝑏𝐿𝐵𝐷: \n" );
  for (I= 0; I< N; I++){
    //para cada fila de bloques (I)
    for (i= 0; i< r; i++){
       for(J=0;J<N;J++){
          despB=(I*N+J)*r*r;
          for (j=0;j<r;j++){
             printf("%f ",B[despB+ i*r+j]);
        
           };//end for j
        };//end for J
        printf("\n ");
    };//end for i

  };//end for I
  printf(" \n\n");

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
*/

 //Verifica el resultado
  for (I= 0; I< N; I++){
    //para cada fila de bloques (I)
    for (i= 0; i< r; i++){
      for(J=0;J<N;J++){
        despB=(I*N+J)*r*r;
        for (j=0;j<r;j++){
          check = check && (M[despB+ i*r+j]==(l*n*n+n*(I*r+i+1)));
        };//end for j
      };//end for J
    };//end for i

  };//end for I

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

