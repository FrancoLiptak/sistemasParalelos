#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <sys/time.h>

// Para calcular el tiempo
double dwalltime(){
	double sec;
	struct timeval tv;

	gettimeofday(&tv,NULL);
	sec = tv.tv_sec + tv.tv_usec/1000000.0;
	return sec;
}

int main(int argc,char* argv[]){

    double *A,*B,*C,*D,*L,*M; // Matrices (M será la matriz resultado)
    int I,J,K,i,j,k; // Índices para los for
    int despA, despB, despC, desp; // Desplazamientos para las multiplicaciones por bloques
    double b,l; // Para los productos escalares
    int check = 1; // Para controlar el resultado del programa
    double timetick;
    int N = 2; // Cantidad de bloques
    int r = 256; // Tamaño de cada bloque

    //Controla los argumentos al programa
    if ((argc != 3) || ((N = atoi(argv[1])) <= 0) || ((r = atoi(argv[2])) <= 0) ){
        printf("Debe ingresar la cantidad de bloques por dimensión, y la dimensión de cada bloque. \n");
        exit(1);
    }

    int n = N*r; // Cantidad de celdas por lado de la matriz
    int sizeMatrix = n*n; // Cantidad total de datos matriz
    int sizeBlock = r*r; // Cantidad total de datos del bloque
    int sizeL = (n+1)*n/2; // Tamaño de la matriz L
    int sizeTriangularBlock = (r+1)*r/2; // Tamaño de bloque triangular

    //Aloca memoria para las matrices
    A=(double*)malloc(sizeof(double)*sizeMatrix);
    B=(double*)malloc(sizeof(double)*sizeMatrix);
    C=(double*)malloc(sizeof(double)*sizeMatrix);
    D=(double*)malloc(sizeof(double)*sizeMatrix);
    L=(double*)malloc(sizeof(double)*sizeL);
    M=(double*)malloc(sizeof(double)*sizeMatrix);

    // Inicialización de matrices
    despA = 0;
    // Con los dos for de afuera, nos movemos de bloque
    for (I= 0; I< N; I++){ // Utilizamos I para movernos en cada fila de bloques
        for(J=0;J<N;J++){
            if(I>=1)
                despA = I*sizeTriangularBlock+(I-1+J)*sizeBlock; // Utilizamos despA para movernos en bloques de L (matriz triangular)
            despB=(I*N+J)*r*r; // Utilizamos despB para movernos en bloques de matrices cuadradas
            // A partir de este for, nos movemos dentro de cada bloque
            for (i= 0; i< r; i++){
                for (j=0;j<r;j++){
                    A[despB+ i*r+j]=1;
                    B[despB+ i*r+j]=1;
                    C[despB+ i*r+j]=1;
                    D[despB+ i*r+j]=1;
                    M[despB+ i*r+j]=0;
                    if(J<=I){ // Para controlar los bloques triangulares (No iteramos los bloques que 'tendrían todos 0')
                        if(I==J){ // Bloques triangulares en la matriz L
                            if(i>=j){
                            L[despA+ i+j+i*(i-1)/2]=1;
                            }            
                        }else{ // Bloques completos de unos en la matriz L
                            L[despA+ i*r+j]=1;            
                        }
                    }
                };
            };
        };
    };

    // La misma estrategia (utilizar bloques), será usada de acá en adelante, por lo cual se omiten comentarios repetidos 
    // (las variables se usan siempre con el mismo fin)

    // Calculamos 'b', el cual es un promedio de la matriz B. Lo calculamos utilizando bloques
    for (I= 0; I< N; I++){
        for(J=0;J<N;J++){
            despB=(I*N+J)*r*r;
            for (i= 0; i< r; i++){
                for (j=0;j<r;j++){
                    b += B[despB+ i*r+j];
                };
            };
         };
    };
    b /= sizeMatrix;

    // Calculamos 'l', el cual es un promedio de la matriz l (triangular). Lo calculamos utilizando bloques triangulares
    despA = 0;
    for (I= 0; I< N; I++){
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
                };
            };
        };
    };
    l /= sizeMatrix;

    timetick = dwalltime(); // Empezamos a controlar el tiempo, dado que empiezan las operaciones

    // Se empieza a resolver la expresion 𝑀 = 𝑙.𝐴𝐵𝐶 + 𝑏𝐿𝐵𝐷

    // Comenzamos con el primer término: 𝑙.𝐴𝐵𝐶

    // Calculamos 𝑙.𝐴 y lo guardamos en L
    for (I= 0; I< N; I++){
        for(J=0;J<N;J++){
            despB=(I*N+J)*r*r;
            for (i= 0; i< r; i++){
                for (j=0;j<r;j++){
                    A[despB+ i*r+j]= A[despB+ i*r+j]*l;
                };
            };
        };
    };
 
    // Calculamos l𝐴𝐵 y lo guardamos en M
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

    // Volvemos a poner la matriz A en 0, a fin de reutilizarla para los cálculos y ahorrar el espacio ocupado
    for (I= 0; I< N; I++){
        for(J=0;J<N;J++){
            despB=(I*N+J)*r*r;
            for (i= 0; i< r; i++){
                for (j=0;j<r;j++){
                    A[despB+ i*r+j]= 0;
                };
            };
        };
    };

    // Terminamos de calcular el primer término calculando lA𝐵𝐶 y lo guardamos en A
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

    // Ya terminamos de calcular el primer término

    // Volvemos a poner la matriz A en 0, a fin de reutilizarla para los cálculos y ahorrar el espacio ocupado
    for (I= 0; I<N; I++){
        for(J=0;J<N;J++){
            despB=(I*N+J)*r*r;
            for (i= 0; i< r; i++){
                for (j=0;j<r;j++){
                    M[despB+ i*r+j]= 0;
                };
            };
        };
    };

    // Calculamos el segundo término: 𝑏𝐿𝐵𝐷

    // Calculamos 𝑏𝐿 y lo guardamos en L. Recordar que L es la matriz triangular y se calcula usando bloques triangulares
    despA = 0;
    for (I= 0; I< N; I++){
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
                };
            };
        };
    };

    // Calculamos 𝑏𝐿𝐵 y lo guardamos en M. Recordar que L es la matriz triangular y se calcula usando bloques triangulares
    for (I=0;I<N;I++){
        for (J=0;J<N;J++){
            despC = (I*N+J)*sizeBlock;
            despA = 0;
            for (K=0;K<=I;K++){ // K<=0 nos permite no movernos hacia 'bloques vacíos'
                if(I>=1){
                    despA = I*sizeTriangularBlock+(I-1+K)*sizeBlock;
                }
                despB = (K*N+J)*sizeBlock;
                for (i=0;i<r;i++){
                    for (j=0;j<r;j++){
                        desp = despC + i*r+j; // Usamos 'desp' para pararnos en la celda correcta para guardar el resultado
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

    // Volvemos a poner la matriz B en 0, a fin de reutilizarla para los cálculos y ahorrar el espacio ocupado
    for (I= 0; I< N; I++){
        for(J=0;J<N;J++){
            despB=(I*N+J)*r*r;
            for (i= 0; i< r; i++){
                for (j=0;j<r;j++){
                    B[despB+ i*r+j]= 0;
                };
            };
        };
    };

    // Calculamos 𝑏𝐿𝐵𝐷 y lo guardamos en B
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

    // Ya terminamos de calcular el segundo término

    printf("Tiempo en segundos %f\n", dwalltime() - timetick); // Informamos el tiempo

    // Finalmente calculamos 𝑙.𝐴𝐵𝐶 + 𝑏𝐿𝐵𝐷 y guardamos el resultado en M. Recordar que A = 𝑙.𝐴𝐵𝐶 y B = 𝑏𝐿𝐵𝐷.
    for (I= 0; I< N; I++){
        for(J=0;J<N;J++){
            despB=(I*N+J)*r*r;
            for (i= 0; i< r; i++){
                for (j=0;j<r;j++){
                    M[despB+ i*r+j] = A[despB+ i*r+j] + B[despB+ i*r+j];
                };
            };
        };
    };

    // Verifica el resultado
    for (I= 0; I< N; I++){
        for (i= 0; i< r; i++){
            for(J=0;J<N;J++){
                despB=(I*N+J)*r*r;
                for (j=0;j<r;j++){
                    check = check && (M[despB+ i*r+j]==(l*n*n+n*(I*r+i+1)));
                };
            };
        };
    };

    if(check){
        printf("Resultado correcto. \n");
    }else{
        printf("Resultado incorrecto. \n");
    }

    // Liberamos el espacio ocupado por las matrices
    free(A);
    free(B);
    free(C);
    free(D);
    free(L);
    free(M);

    return(0);
}