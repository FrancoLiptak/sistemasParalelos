# Programación con memoria compartida

## Aclaraciones

El presente trabajo ha sido desarrollado en grupo por Franco Emanuel Liptak y Gastón Gustavo Ríos. Consiste en dos ejercicios prácticos pedidos por la cátedra, donde cada uno tiene 3 versiones diferentes (secuencial, resolución con Pthreads, y resolución con OpenMP). El objetivo del trabajo fue resolver los ejercicios con distintas tecnologías, a fin de poder visualizar la mejora del rendimiento entre cada una de las tecnologías de trabajo paralelo (Pthreads y OpenMP), respecto al algoritmo secencial.

Se ofrece también la posibilidad de descargar los archivos de extensión '.c' (es decir, los archivos que serán utilizados por el compilador, para compilar en base a la gramática y sintaxis de C) desde el siguiente repositorio de GitHub: https://github.com/okason97/sistemasParalelos, el cual por supuesto, tiene como colaboradores a ambos integrantes del grupo. 

Nuestros usuarios de GitHub son:

- okason97 (Gastón)
- FrancoLiptak (Franco)

Las pruebas se hicieron con el procesador AMD Phenom II X6 1100T Black Edition.

## Ejercicio 1

*"Realizar un algoritmo Pthreads y otro OpenMP que resuelva la expresión: M = lABC + bLBD. Donde A, B, C y D son matrices de NxN. L matriz triangular inferior de NxN. b y l son los promedios de los valores de los elementos de las matrices B y L, respectivamente.
Evaluar N=512, 1024 y 2048."*

### Resolución secuencial

Para poder ejecutar este ejercicio, el usuario debe ingresar la cantidad de bloques por lado, y la longitud de cada bloque. 

Posteriormente se inicializan las variables necesarias y se aloca espacio para las distintas matrices. Para evitar desperdiciar espacio, las matrices se reutilizarán (será necesario inicializar algunas en 0 después de utilizarlas). Es importante aclarar que para la matriz triangular solo se reserva el espacio correspondiente al triangulo inferior, y por ende solo se inicializan dichas posiciones.

Luego de terminar la inicialización, comenzamos a controlar el tiempo.

Las operaciones en las matrices son por bloques, lo cual es más eficiente. Sin embargo, recordamos que la matriz triangular no está almacenada como una matriz cuadrada. Por ende, para dicha matriz trabajamos con "bloques triangulares". Para diferenciar los tipos de bloques servirá esta explicación:

```
Las matrices cuadradas tienen bloques "cuadrados", por ejemplo:

1 1
1 1

Sin embargo, la matriz triangular puede tener bloques que no sigan esa forma. Básicamente puede tener:

1 1
1 1

O también el bloque triangular:

1 
1 1

Y por último, también hay que tener en cuenta no intentar acceder a posiciones en la matriz triangular, que corresponderían a "cuadrados vacios" (no se les alocó espacio ni tampoco (obviamente) se los inicializó)
```  

Una vez entendido eso, trabajamos con 'bloques cuadrados' siempre, a menos que estemos trabajando con la matriz triangular.

Realizamos las multiplicaciones solicitadas por el enunciado de izquierda a derecha, y separando en términos. Una vez que se tiene el resultado del primer término y el resultado del segundo término, se realiza la suma, guardando finalmente en M el resultado.

Luego informamos el tiempo y verificamos el resultado (informamos si el resultado es correcto o no). 

Finalmente liberamos el espacio ocupado y termina nuestra solución.

### Resolución con Pthreads

La solución con Pthreads se basa en nuestra solución secuencial y por ende, se pasa a explicar sus diferencias.

El usuario deberá ingresar, además de la cantidad de bloques por lado y la longitud de cada bloque, el número de hilos con el que quiere trabajar. Recordar que trabajamos con potencias de dos.

En esta solución la concurrencia se encuentra en que los bloques se repartiran entre los hilos de modo que en cada operacion de una matriz, un hilo tomará N/NUM_THREADS filas de bloques.
Se crearan `NUM_THREADS` hilos, y cada hilo ejecutara la funcion `producto` utilizando su id para saber que filas de bloques le pertenecen.

Tambien se ha cambiado el orden de algunas operaciones para favorecer al parelelismo, cuando en la secuencial se hacian los calculos de la forma lA, lAB, lABC, bL, bLB, bLBD y finalmente lABC + bLBD, en la solucion con pthreads se realiza de la siguiente forma:

1. lA, BC, bL y BD
2. lABC y bLBD
3. lABC + bLBD

Donde los calculos de cada punto (1, 2 y 3) se realizan en forma paralela, ya que los threads no interfieren entre si, el acceso a las variables compartidas es solo para lectura.
Se utilizan barreras entre cada uno de estos puntos para sincronizar los threads, ya que los resultados de cada punto dependen de los del punto anterior, entonces los threads deben de terminar de calcular los resultados para poder proseguir.

Al finalizar, se destruye la barrera creada y se libera la memoria ocupada por los vectores. 

### Resolución con OpenMP

La solución con OpenMP es similar a la de Pthreads y por ende, se pasa a explicar sus diferencias.

El usuario deberá ingresar, además de la cantidad de bloques por lado y la longitud de cada bloque, el número de hilos con el que quiere trabajar. Recordar que trabajamos con potencias de dos.

La cantidad de hilos a utilizar es la cantidad total, es decir, la ingresada por el usuario (que fue configurada con `omp_set_num_threads(NUM_THREADS);`).

Utilizamos la instrucción `#pragma omp parallel` con el fin de paralelizar distintas partes del código. Las partes puntualmente son: 

1. lA, BC, bL, BD
2. lABC y bLBD
3. lABC + bLBD

Se ponen como privadas variables que serán utilizadas por todos los hilos que no sean los vectores (indices y desplazamientos), con el fin de que un hilo no vea las modificaciones hechas por otro, y no haya conflicto entre sus procesamientos de matrices. 

Luego de esta primera gran división en partes de la resolución del problema, cada una de las operaciones (que recordemos, se resuelven con `for`) cuenta con un encabezado `#pragma omp for`, el cual indica que se va a paralelizar la ejecución de un `for`. 
También se utiliza la claúsula `collapse(N)`, la cual nos garantiza el uso de Threads para los valores I y J, utilizados en los `for` "mas de afuera", los cuales son utilizados para elegir el bloque a computar por cada hilo.
Por último,se utiliza la clausula `nowait` en ya que los fors de cada parte del problema pueden ejecutarse en paralelos, la barrera implicita al final de cada for es innecesaria. 

### Mediciones

##### Tiempo en la solución secuencial:

| Tamaño de la matriz | Tiempo secuencial |
| --------------------|:-----------------:|
| 16 bloques de 32    | 1.330058          |
| 32 bloques de 32    | 10.632906         |
| 64 bloques de 32    | 84.822689         |

##### Tiempo con dos hilos en las soluciones con Pthreads y OpenMP:

| Tamaño de la matriz | Tiempo con Pthreads | Tiempo con OpenMP |
| --------------------|:-------------------:|:-----------------:|
| 16 bloques de 32    | 0.703056            | 0.741981          |
| 32 bloques de 32    | 5.568404            | 5.806306          |
| 64 bloques de 32    | 44.539011           | 47.942854         |

##### Speedup con dos hilos en las soluciones con Pthreads y OpenMP:

| Tamaño de la matriz | Speedup con Pthreads | Speedup con OpenMP |
| --------------------|:--------------------:|:------------------:|
| 16 bloques de 32    | 1.891824             | 1.792578           |
| 32 bloques de 32    | 1.909507             | 1.831269           |
| 64 bloques de 32    | 1.904458             | 1.769246           |

##### Eficiencia con dos hilos en las soluciones con Pthreads y OpenMP:

| Tamaño de la matriz | Eficiencia con Pthreads | Eficiencia con OpenMP |
| --------------------|:-----------------------:|:---------------------:|
| 16 bloques de 32    | 0.945912                | 0.896289              |
| 32 bloques de 32    | 0.954753                | 0.915634              |
| 64 bloques de 32    | 0.952229                | 0.884623              |

##### Tiempo con cuatro hilos en las soluciones con Pthreads y OpenMP:

| Tamaño de la matriz | Tiempo con Pthreads | Tiempo con OpenMP |
| --------------------|:-------------------:|:-----------------:|
| 16 bloques de 32    | 0.373581            | 0.389785          |
| 32 bloques de 32    | 2.978698            | 3.094061          |
| 64 bloques de 32    | 24.000719           | 24.719251         |

##### Speedup con cuatro hilos en las soluciones con Pthreads y OpenMP:

| Tamaño de la matriz | Speedup con Pthreads | Speedup con OpenMP |
| --------------------|:--------------------:|:------------------:|
| 16 bloques de 32    | 3.560293             | 3.412283           |
| 32 bloques de 32    | 3.569648             | 3.436554           |
| 34 bloques de 32    | 3.534173             | 3.431443           |

##### Eficiencia con cuatro hilos en las soluciones con Pthreads y OpenMP:

| Tamaño de la matriz | Eficiencia con Pthreads | Eficiencia con OpenMP |
| --------------------|:-----------------------:|:---------------------:|
| 16 bloques de 32    | 0.890073                | 0.853071              |
| 32 bloques de 32    | 0.892412                | 0.859138              |
| 64 bloques de 32    | 0.883543                | 0.857861              |

## Ejercicio 2

*"Paralelizar con Pthreads y OpenMP un algoritmo que cuente la cantidad de número pares en un vector de N elementos. Al finalizar, el total debe quedar en una variable llamada pares.
Evaluar con valores de N donde el algoritmo paralelo represente una mejora respecto al algoritmo secuencial."*

### Resolución secuencial

La idea del ejercicio es que el usuario pueda ingresar la longitud del vector con el cual desea trabajar (N). Se controla que se ingrese el argumento.

Posteriormente se aloca el espacio necesario y se lo inicializa. Cada posición del vector almacenará el valor que se corresponde con su índice (La posición 0 almacena el valor 0). Esto nos ayudará después a comprobar que el resultado sea correcto.

Para verificar la cantidad de números pares recorremos todo el vector, y el valor de la posición actual del vector lo dividimos por 2. Si el resto es 0, entonces tenemos un nuevo número par, por lo cual incrementamos la variable 'pares'.

Gracias a la manera en la cual se inicializa el arreglo, podemos asegurar que siempre la cantidad de números pares será la parte entera del resultado de (N+1)/2, donde recordamos que N es la longitud del arreglo que inició el usuario.

Si el resultado es correcto entonces lo informamos e informamos la cantidad de pares. Caso contrario, informamos que el resultado ha sido incorrecto.

Finalmente liberamos la memoria ocupada por el vector y termina la resolución.

### Resolución con Pthreads

La solución con Pthreads se basa en nuestra solución secuencial y por ende, se pasa a explicar sus diferencias.

El usuario deberá ingresar, además de la longitud del vector con el cual desea trabajar, el número de hilos con el que quiere trabajar. Recordar que trabajamos con potencias de dos.

El bloque de código que para el algoritmo secuencial obtenía la cantidad de números pares ahora es una función llamada "contador_pares" (con las modificaciones necesarias al código), la cual le pasamos a cada hilo al momento de crearlo. Lo destacable, es que cada hilo trabajará solo con una porción del arreglo (la que le corresponde en base a su número de hilo) y contará en una variable local la cantidad de números pares de su porción del arreglo. Una vez que haya terminado, utilizamos las herramientas para asegurar exclusión mutua de Pthreads para que cada hilo pueda sumar la cantidad de números pares que encontró con la cantidad total.

Al final de la solución, además de liberar la memoria ocupada por el vector, también destruimos la variable que nos permitía usar la exclusión mutua (cuya inicialización se puede ver al principio de la solución).

Con eso terminan las diferencias respecto a la solución secuencial.

### Resolución con OpenMP

La solución con OpenMP se basa en nuestra solución secuencial y por ende, se pasa a explicar sus diferencias.

El usuario deberá ingresar, además de la longitud del vector con el cual desea trabajar, el número de hilos con el que quiere trabajar. Recordar que trabajamos con potencias de dos.

Al momento de contar la cantidad de números pares, simplemente agregamos la siguiente linea de código: ` #pragma omp parallel for reduction(+:pares) `, la cual se explica a continuación:

- `parallel`: especifica que el bloque de código será ejecutado en paralelo. Cada hilo tendrá un ID único. Finalizada la región paralela, solo el hilo master continúa con la ejecución. La cantidad de hilos a utilizar es la cantidad total, es decir, la ingresada por el usuario (que fue configurada con `omp_set_num_threads(NUM_THREADS);`).
- `for`: indica que se va a paralelizar la ejecución de un `for`. En nuestro caso, recordar que el recorrido del vector se realiza con este iterador.
- `reduction(+:pares)`: en nuestro caso, indica a cada hilo que se debe realizar una suma sobre la variable indicada.

Con eso terminan las diferencias respecto a la solución secuencial.

### Mediciones (Los resultados mostrados son un promedio de 5 pruebas)

##### Tiempo en la solución secuencial:

| Longitud del vector | Tiempo secuencial |
| --------------------|:-----------------:|
| 2^28 = 268435456    | 1.380244          |
| 2^30 = 1073741824   | 5.3868828         |

##### Tiempo con dos hilos en las soluciones con Pthreads y OpenMP:

| Longitud del vector | Tiempo con Pthreads | Tiempo con OpenMP |
| --------------------|:-------------------:|:-----------------:|
| 2^28 = 268435456    | 0.724567            | 0.7225472         |
| 2^30 = 1073741824   | 2.8348112           | 2.8872884         |

##### Speedup con dos hilos en las soluciones con Pthreads y OpenMP:

| Longitud del vector | Speedup con Pthreads | Speedup con OpenMP |
| --------------------|:--------------------:|:------------------:|
| 2^28 = 268435456    | 1.90492252614        | 1.91024752431      |
| 2^30 = 1073741824   | 1.9002615765         | 1.86572383971      |

##### Eficiencia con dos hilos en las soluciones con Pthreads y OpenMP:

| Longitud del vector | Eficiencia con Pthreads | Eficiencia con OpenMP |
| --------------------|:-----------------------:|:---------------------:|
| 2^28 = 268435456    | 0.95246126307           | 0.95512376215         |
| 2^30 = 1073741824   | 0.95013078825           | 0.93286191985         |

##### Tiempo con cuatro hilos en las soluciones con Pthreads y OpenMP:

| Longitud del vector | Tiempo con Pthreads | Tiempo con OpenMP |
| --------------------|:-------------------:|:-----------------:|
| 2^28 = 268435456    | 0.380976            | 0.392383          |
| 2^30 = 1073741824   | 1.503635            | 1.535451          |

##### Speedup con cuatro hilos en las soluciones con Pthreads y OpenMP:

| Longitud del vector | Speedup con Pthreads | Speedup con OpenMP |
| --------------------|:--------------------:|:------------------:|
| 2^28 = 268435456    | 3.62291587922        | 3.51759377955      |
| 2^30 = 1073741824   | 3.58257343039        | 3.50833911339      |

##### Eficiencia con cuatro hilos en las soluciones con Pthreads y OpenMP:

| Longitud del vector | Eficiencia con Pthreads | Eficiencia con OpenMP |
| --------------------|:-----------------------:|:---------------------:|
| 2^28 = 268435456    | 0.9057289698            | 0.87939844488         |
| 2^30 = 1073741824   | 0.89564335759           | 0.87708477834         |

## Conclusiones

Gracias al paralelismo de los problemas utilizando Pthreads y OpenMP hemos logrado reducir en gran medida el tiempo requerido para el procesamiento de estos. Pthreads logró mejor speedup y eficiencia.
El reordenamiento del problema en el ejercicio 1 para maximizar el paralelismo nos otorgo un gran incremento en la velocidad.
