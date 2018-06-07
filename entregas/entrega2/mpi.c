#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <mpi.h>

#define COORDINATOR 0

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
    int i,j,N,k,rank,numProcs,flag;
		int end = -1;
    int num_solutions_local = 0;
    int num_solutions = 0;
    int backtrack = 0;
    int check=1;
    int queens_final;
    double timetick;
    double timetick_local;
    double local_time = 0;
    int not_found = 1;
    MPI_Status status;
		MPI_Request req;

    MPI_Init(&argc,&argv);

    MPI_Comm_size(MPI_COMM_WORLD,&numProcs);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);

    //Controla los argumentos al programa
    if ((argc != 2) || ((N = atoi(argv[1])) <= 0) ){
        printf("\nUsar: %s n\n  n: Dimension de la matriz (nxn X nxn)\n", argv[0]);
        exit(1);
    }


		//Aloca memoria para las matrices
		queens=(int*)malloc(sizeof(int)*N);
		col_available=(int*)calloc(N, sizeof(int));
    asc_diagonal=(int*)calloc((N-1)*2+1, sizeof(int));
    des_diagonal=(int*)calloc((N-1)*2+1, sizeof(int));

    // Inicializo queens en -1 
    for(i=0; i<N; i++){
        queens[i] = -1;
    }
	
    if (rank == COORDINATOR) {
	    timetick = dwalltime(); //Empieza a controlar el tiempo

    	int id;
    	for (i = 1; i < numProcs; i++) {
				k = i-1;
    		MPI_Recv(&id, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    		MPI_Send(&k, 1, MPI_INT, id, 0, MPI_COMM_WORLD);
    	}
			MPI_Irecv(&id, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &req);
    	for (k = numProcs-1; k < N; k++) {
				MPI_Test(&req,&flag,&status);
				if (flag){
	    		MPI_Send(&k, 1, MPI_INT, id, 0, MPI_COMM_WORLD);											
					if (k != N-1){
						MPI_Irecv(&id, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &req);
					}					
				}else{
			    timetick_local = dwalltime(); //Empieza a controlar el tiempo
					queens[0] = k;
		      col_available[k] = 1;
		      asc_diagonal[0+k] = 1;
		      des_diagonal[(N-1)-(0-k)] = 1;
					i = 1;
					queens_final = 0;
				  while( queens_final == 0 ){
			        j = queens[i] + 1;
				      not_found = 1;
				      while( (not_found == 1) && (j < N) ){
				          if ( (col_available[j] == 0) // la columna no tiene otra reina
				              && (asc_diagonal[i+j] == 0) // la diagonal ascendente no tiene otra reina
				              && (des_diagonal[(N-1)-(i-j)] == 0)){ // la diagonal descendente no tiene otra reina
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
				          if ((queens[i] != -1) || (i == 1)){
				              // Libero lo que tenia asignado la vuelta pasada
				              col_available[queens[i]] = 0;
				              asc_diagonal[i+queens[i]] = 0;
				              des_diagonal[(N-1)-(i-queens[i])] = 0;
				              queens[i] = -1;
				          }
		              if (i == 1){
		                  queens_final = 1;
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
				              num_solutions_local++;
				          }
				      }
				  }
		      col_available[k] = 0;
		      asc_diagonal[0+k] = 0;
		      des_diagonal[(N-1)-(0-k)] = 0;
			    local_time += dwalltime() - timetick_local; //Empieza a controlar el tiempo
				}
			}
			if (!flag){
				while(!flag){
					MPI_Test(&req,&flag,&status);
				}
    		MPI_Send(&end, 1, MPI_INT, id, 0, MPI_COMM_WORLD);
				k = 2;
			}else{
				k = 1;			
			}
    	for (i = k; i < numProcs; i++) {
    		MPI_Recv(&id, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    		MPI_Send(&end, 1, MPI_INT, id, 0, MPI_COMM_WORLD);
    	}
    }else{
			MPI_Send(&rank, 1, MPI_INT, COORDINATOR, 0, MPI_COMM_WORLD);
			MPI_Recv(&k, 1, MPI_INT, COORDINATOR, 0, MPI_COMM_WORLD, &status);
	    //Coloca las reinas
			while( k != -1 ){
		    timetick_local = dwalltime(); //Empieza a controlar el tiempo
				queens[0] = k;
        col_available[k] = 1;
        asc_diagonal[0+k] = 1;
        des_diagonal[(N-1)-(0-k)] = 1;
				i = 1;
				queens_final = 0;
		    while( queens_final == 0 ){
		        j = queens[i] + 1;
		        not_found = 1;
		        while( (not_found == 1) && (j < N) ){
		            if ( (col_available[j] == 0) // la columna no tiene otra reina
		                && (asc_diagonal[i+j] == 0) // la diagonal ascendente no tiene otra reina
		                && (des_diagonal[(N-1)-(i-j)] == 0)){ // la diagonal descendente no tiene otra reina
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
		            if ((queens[i] != -1) || (i == 1)){
		                // Libero lo que tenia asignado la vuelta pasada
		                col_available[queens[i]] = 0;
		                asc_diagonal[i+queens[i]] = 0;
		                des_diagonal[(N-1)-(i-queens[i])] = 0;
		                queens[i] = -1;
		            }
                if (i == 1){
                    queens_final = 1;
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
		                num_solutions_local++;
		            }
		        }
		    }
        col_available[k] = 0;
        asc_diagonal[0+k] = 0;
        des_diagonal[(N-1)-(0-k)] = 0;
		    local_time += dwalltime() - timetick_local; //Empieza a controlar el tiempo
				MPI_Send(&rank, 1, MPI_INT, COORDINATOR, 0, MPI_COMM_WORLD);
				MPI_Recv(&k, 1, MPI_INT, COORDINATOR, 0, MPI_COMM_WORLD, &status);
			}
    }

		MPI_Reduce(&num_solutions_local, &num_solutions, 1, MPI_INT, MPI_SUM, COORDINATOR, MPI_COMM_WORLD);

    if (rank == COORDINATOR) {
	    printf("Resultado: %d\n", num_solutions);
	    printf("Tiempo en segundos %f\n", dwalltime() - timetick); // Informamos el tiempo
		}

    printf("Tiempo local %f rank: %d\n", local_time, rank); // Informamos el tiempo

    free(queens);
    free(col_available);
    free(asc_diagonal);
    free(des_diagonal);

    MPI_Finalize();

    return(0);
}
