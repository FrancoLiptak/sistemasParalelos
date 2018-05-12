#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <sys/time.h>
#include <pthread.h>

int N = 2; // Cantidad de bloques
int r = 256; // Tamaño de cada bloque
int NUM_THREADS = 4; // Número de threads por defecto
int elementos_por_thread; // Cantidad de elementos que procesa cada thread
double *A,*B,*C,*D,*L,*M; // Matrices (M será la matriz resultado)
double b,l; // Para los productos escalares
int sizeBlock, sizeTriangularBlock;

struct arg_struct {
    int id;
    double *arg1;
    double *arg2;
    double *arg3;
};

struct arg_struct_element {
    int id;
    double arg1;
    double *arg2;
    double *arg3;
};

struct arg_struct_zero {
    int id;
    double *arg1;
};

// Para calcular el tiempo
double dwalltime(){
	double sec;
	struct timeval tv;

	gettimeofday(&tv,NULL);
	sec = tv.tv_sec + tv.tv_usec/1000000.0;
	return sec;
}

void * suma(void * args){
    struct arg_struct *arguments = (struct arg_struct *)args;
    int id_thread = arguments -> id;
    int I,J,despB,i,j;
    int base =  elementos_por_thread * id_thread;
    int limite = elementos_por_thread * (id_thread+1); 

    for (I=base; I<limite; I++){
        for(J=0;J<N;J++){
            despB=(I*N+J)*r*r;
            for (i= 0; i< r; i++){
                for (j=0;j<r;j++){
                    arguments -> arg3[despB+ i*r+j] = arguments -> arg1[despB+ i*r+j] + arguments -> arg2[despB+ i*r+j];
                };
            };
        };
    };
    pthread_exit(0);
}

void * producto(void * args){
    struct arg_struct *arguments = (struct arg_struct *)args;
    int id_thread = arguments -> id;
    int I,J,despB,despA,desp,despC,K,k,i,j;
    int base =  elementos_por_thread * id_thread;
    int limite = elementos_por_thread * (id_thread+1); 

    for (I=base; I<limite; I++){
        for (J=0;J<N;J++){
            despC = (I*N+J)*sizeBlock;
            for (K=0;K<N;K++){
                despA = (I*N+K)*sizeBlock;
                despB = (K*N+J)*sizeBlock;
                for (i=0;i<r;i++){
                    for (j=0;j<r;j++){
                        desp = despC + i*r+j;
                        for (k=0;k<r;k++){
                            arguments -> arg3[desp] += arguments -> arg1[despA + i*r+k]*arguments -> arg2[despB + k*r+j]; 
                        };
                    }
                };
            };
        };  
    }; 
    pthread_exit(0);
}

void * productoTriangular(void * args){
    struct arg_struct *arguments = (struct arg_struct *)args;
    int id_thread = arguments -> id;
    int I,J,despB,despA,desp,despC,K,k,i,j;
    int base =  elementos_por_thread * id_thread;
    int limite = elementos_por_thread * (id_thread+1); 

    for (I=base; I<limite; I++){
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
                                arguments -> arg3[desp] += arguments -> arg1[despA + i+k+i*(i-1)/2] * arguments -> arg2[despB + k*r+j];
                            };
                        }else{
                            for (k=0;k<r;k++){
                                arguments -> arg3[desp] += arguments -> arg1[despA + i*r+k] * arguments -> arg2[despB + k*r+j];
                            };            
                        }
                    }
                };
            };
        };  
    }; 
    pthread_exit(0);
}

void * zero(void * args){
    struct arg_struct *arguments = (struct arg_struct *)args;
    int id_thread = arguments -> id;
    int I,J,despB,i,j;
    int base =  elementos_por_thread * id_thread;
    int limite = elementos_por_thread * (id_thread+1); 

    for (I=base; I<limite; I++){
        for(J=0;J<N;J++){
            despB=(I*N+J)*r*r;
            for (i= 0; i< r; i++){
                for (j=0;j<r;j++){
                    arguments -> arg1[despB+ i*r+j]= 0;
                };
            };
        };
    };

    pthread_exit(0);
}

void * productoElemento(void * args){
    struct arg_struct_element *arguments = (struct arg_struct_element *)args;
    int id_thread = arguments -> id;
    int I,J,despB,i,j;
    int base =  elementos_por_thread * id_thread;
    int limite = elementos_por_thread * (id_thread+1); 

    for (I=base; I<limite; I++){
        for(J=0;J<N;J++){
            despB=(I*N+J)*r*r;
            for (i= 0; i< r; i++){
                for (j=0;j<r;j++){
                    arguments -> arg3[despB+ i*r+j]= arguments -> arg2[despB+ i*r+j]*arguments -> arg1;
                };
            };
        };
    };
    pthread_exit(0);
}

void * productoElementoTriangular(void * args){
    struct arg_struct_element *arguments = (struct arg_struct_element *)args;
    int id_thread = arguments -> id;
    int I,J,despA,i,j;
    int base =  elementos_por_thread * id_thread;
    int limite = elementos_por_thread * (id_thread+1); 

    despA = 0;
    for (I= base; I< limite; I++){
        for(J=0;J<N;J++){
            if(I>=1)
                despA = I*sizeTriangularBlock+(I-1+J)*sizeBlock;
            for (i= 0; i< r; i++){
                for (j=0;j<r;j++){
                    if(I==J){
                        if(i>=j){
                            arguments -> arg3[despA+ i+j+i*(i-1)/2] = arguments -> arg2[despA+ i+j+i*(i-1)/2] * arguments -> arg1;
                        }            
                    }else{
                        arguments -> arg3[despA+ i*r+j] = arguments -> arg2[despA+ i*r+j] * arguments -> arg1;            
                    }
                };
            };
        };
    };

    pthread_exit(0);
}

int main(int argc,char* argv[]){

    int I,J,K,i,j,k; // Índices para los for
    int despA, despB, despC, desp; // Desplazamientos para las multiplicaciones por bloques
    int check = 1; // Para controlar el resultado del programa
    double timetick;

    // Controla los argumentos al programa
    if ((argc != 4) || ((N = atoi(argv[1])) <= 0) || ((r = atoi(argv[2])) <= 0) || ((NUM_THREADS = atoi(argv[3])) <= 0)){
        printf("Debe ingresar la cantidad de bloques por dimensión, la dimensión de cada bloque y el número de Threads a usar. \n");
        exit(1);
    }
    if (NUM_THREADS > N){
        printf("No debe ingresar mayor cantidad de threads que de bloques. \n");
        exit(1);
    }

    int n = N*r; // Cantidad de celdas por lado de la matriz
    int sizeMatrix = n*n; // Cantidad total de datos matriz
    sizeBlock = r*r; // Cantidad total de datos del bloque
    int sizeL = (n+1)*n/2; // Tamaño de la matriz L
    sizeTriangularBlock = (r+1)*r/2; // Tamaño de bloque triangular

    pthread_t threads[NUM_THREADS];
    struct arg_struct args[NUM_THREADS];
    struct arg_struct_element args_element[NUM_THREADS];
    struct arg_struct_zero args_zero[NUM_THREADS];
    elementos_por_thread = N / NUM_THREADS;

    // Aloca memoria para las matrices
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

    // Calculamos 𝑙.𝐴 y lo guardamos en A
    //Crea los threads 

    for(i = 0; i < NUM_THREADS ; i++){
        args_element[i].arg1 = l;
        args_element[i].arg2 = A;
        args_element[i].arg3 = A;
        args_element[i].id = i;
        pthread_create(&threads[i], NULL, &productoElemento, (void *)&args_element[i]);
    }
    // El hilo llamador espera a que todos terminen de buscar la cantidad de pares en su sub-arreglo.
    for(i = 0; i < NUM_THREADS; i++){
        pthread_join(threads[i], NULL);
    }

    // Calculamos l𝐴𝐵 y lo guardamos en M
    for(i = 0; i < NUM_THREADS ; i++){
        args[i].arg1 = A;
        args[i].arg2 = B;
        args[i].arg3 = M;
        args[i].id = i;
        pthread_create(&threads[i], NULL, &producto, (void *)&args[i]);
    }
    // El hilo llamador espera a que todos terminen de buscar la cantidad de pares en su sub-arreglo.
    for(i = 0; i < NUM_THREADS; i++){
        pthread_join(threads[i], NULL);
    }

    // Volvemos a poner la matriz A en 0, a fin de reutilizarla para los cálculos y ahorrar el espacio ocupado
    for(i = 0; i < NUM_THREADS ; i++){
        args_zero[i].arg1 = A;
        args_zero[i].id = i;
        pthread_create(&threads[i], NULL, &zero, (void *)&args_zero[i]);
    }
    // El hilo llamador espera a que todos terminen de buscar la cantidad de pares en su sub-arreglo.
    for(i = 0; i < NUM_THREADS; i++){
        pthread_join(threads[i], NULL);
    }

    // Terminamos de calcular el primer término calculando lA𝐵𝐶 y lo guardamos en A
    for(i = 0; i < NUM_THREADS ; i++){
        args[i].arg1 = M;
        args[i].arg2 = C;
        args[i].arg3 = A;
        args[i].id = i;
        pthread_create(&threads[i], NULL, &producto, (void *)&args[i]);
    }
    // El hilo llamador espera a que todos terminen de buscar la cantidad de pares en su sub-arreglo.
    for(i = 0; i < NUM_THREADS; i++){
        pthread_join(threads[i], NULL);
    }

    // Ya terminamos de calcular el primer término

    // Volvemos a poner la matriz M en 0, a fin de reutilizarla para los cálculos y ahorrar el espacio ocupado
    for(i = 0; i < NUM_THREADS ; i++){
        args_zero[i].arg1 = M;
        args_zero[i].id = i;
        pthread_create(&threads[i], NULL, &zero, (void *)&args_zero[i]);
    }
    // El hilo llamador espera a que todos terminen de buscar la cantidad de pares en su sub-arreglo.
    for(i = 0; i < NUM_THREADS; i++){
        pthread_join(threads[i], NULL);
    }

    // Calculamos el segundo término: 𝑏𝐿𝐵𝐷

    // Calculamos 𝑏𝐿 y lo guardamos en L. Recordar que L es la matriz triangular y se calcula usando bloques triangulares
    for(i = 0; i < NUM_THREADS ; i++){
        args_element[i].arg1 = b;
        args_element[i].arg2 = L;
        args_element[i].arg3 = L;
        args_element[i].id = i;
        pthread_create(&threads[i], NULL, &productoElementoTriangular, (void *)&args_element[i]);
    }
    // El hilo llamador espera a que todos terminen de buscar la cantidad de pares en su sub-arreglo.
    for(i = 0; i < NUM_THREADS; i++){
        pthread_join(threads[i], NULL);
    }

    // Calculamos 𝑏𝐿𝐵 y lo guardamos en M. Recordar que L es la matriz triangular y se calcula usando bloques triangulares
    for(i = 0; i < NUM_THREADS ; i++){
        args[i].arg1 = L;
        args[i].arg2 = B;
        args[i].arg3 = M;
        args[i].id = i;
        pthread_create(&threads[i], NULL, &productoTriangular, (void *)&args[i]);
    }
    // El hilo llamador espera a que todos terminen de buscar la cantidad de pares en su sub-arreglo.
    for(i = 0; i < NUM_THREADS; i++){
        pthread_join(threads[i], NULL);
    }

    // Volvemos a poner la matriz B en 0, a fin de reutilizarla para los cálculos y ahorrar el espacio ocupado
    for(i = 0; i < NUM_THREADS ; i++){
        args_zero[i].arg1 = B;
        args_zero[i].id = i;
        pthread_create(&threads[i], NULL, &zero, (void *)&args_zero[i]);
    }
    // El hilo llamador espera a que todos terminen de buscar la cantidad de pares en su sub-arreglo.
    for(i = 0; i < NUM_THREADS; i++){
        pthread_join(threads[i], NULL);
    }

    // Calculamos 𝑏𝐿𝐵𝐷 y lo guardamos en B
    for(i = 0; i < NUM_THREADS ; i++){
        args[i].arg1 = M;
        args[i].arg2 = D;
        args[i].arg3 = B;
        args[i].id = i;
        pthread_create(&threads[i], NULL, &producto, (void *)&args[i]);
    }
    // El hilo llamador espera a que todos terminen de buscar la cantidad de pares en su sub-arreglo.
    for(i = 0; i < NUM_THREADS; i++){
        pthread_join(threads[i], NULL);
    }

    // Ya terminamos de calcular el segundo término

    // Finalmente calculamos 𝑙.𝐴𝐵𝐶 + 𝑏𝐿𝐵𝐷 y guardamos el resultado en M. Recordar que A = 𝑙.𝐴𝐵𝐶 y B = 𝑏𝐿𝐵𝐷.
    for(i = 0; i < NUM_THREADS ; i++){
        args[i].arg1 = A;
        args[i].arg2 = B;
        args[i].arg3 = M;
        args[i].id = i;
        pthread_create(&threads[i], NULL, &suma, (void *)&args[i]);
    }
    // El hilo llamador espera a que todos terminen de buscar la cantidad de pares en su sub-arreglo.
    for(i = 0; i < NUM_THREADS; i++){
        pthread_join(threads[i], NULL);
    }

    printf("Tiempo en segundos %f\n", dwalltime() - timetick); // Informamos el tiempo

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