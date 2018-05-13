# Programación con memoria compartida

## Aclaraciones

El presente trabajo ha sido desarrollado en grupo por Franco Emanuel Liptak y Gastón Gustavo Ríos. Consiste en dos ejercicios prácticos pedidos por la cátedra, donde cada uno tiene 3 versiones diferentes (secuencial, resolución con Pthreads, y resolución con OpenMP). El objetivo del trabajo fue resolver los ejercicios con distintas tecnologías, a fin de poder visualizar la mejora del rendimiento entre cada una de las tecnologías de trabajo paralelo (Pthreads y OpenMP), respecto al algoritmo secencial.

Si bien solo se han pedido los archivos de extensión '.c' (es decir, los archivos que serán utilizados por el compilador, para compilar en base a la gramática y sintaxis de C), se ofrece también la posibilidad de descargar los archivos ya compilados desde el siguiente repositorio de GitHub: https://github.com/okason97/sistemasParalelos, el cual por supuesto, tiene como colaboradores a ambos integrantes del grupo. 

Nuestros usuarios de GitHub son:
- okason97 (Gastón)
- FrancoLiptak (Franco)

Las pruebas se hicieron con el procesador AMD Phenom II X6 1100T Black Edition.

## Ejercicio 1

*"Realizar un algoritmo Pthreads y otro OpenMP que resuelva la expresión: 𝑀 = 𝑙.𝐴𝐵𝐶 + 𝑏𝐿𝐵𝐷. Donde A, B, C y D son matrices de NxN. L matriz triangular inferior de NxN. 𝑏 y 𝑙 son los promedios de los valores de los elementos de las matrices B y L, respectivamente.
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

Para esta solución utilizaremos funciones para la suma (`suma`), el producto entre dos matrices (`producto`), el producto entre una matriz triangular y una cuadrada (`productoTriangular`), el producto entre una matriz y un elemento (`productoElemento`), el producto entre una matriz triangular y un elemento (`productoElementoTriangular`) y la inicialización en cero (`zero`).

Dado que las funciones son generales, se requiere un método para poder pasarle las variables sobre las que va a trabajar. Para esto usamos `struct`, lo cual nos permite pasarle múltiples variables de diferentes tipos a las funciones. De esta forma es posible pasarle a las funciones el id del hilo y las matrices sobre las que va a trabajar (se pasan los punteros y trabaja sobre la variable global). Utilizamos 3 `struct`: 

-`arg_struct`: Se utiliza en la multiplicación de matrices y matrices triangulares. Posee un `int` para pasar el id del hilo y tres argumentos más, los cuales son: C = A*B, A en `arg1`, B en `arg2` y C en `arg3`.

-`arg_struct_element`: Se utiliza en la multiplicación de una matriz por un elemento. Posee un `int` para pasar el id del hilo y tres argumentos más, los cuales son: C = a*B, a en `arg1`, B en `arg2` y C en `arg3`.

-`arg_struct_zero`: Se utiliza para inicializar una matriz con todos sus elementos en 0. Posee un `int` para pasar el id del hilo y otro argumento para pasar la matriz que se inicializará con todos sus elementos en 0 (`arg1`).

### Resolución con OpenMP

La solución con OpenMP se basa en nuestra solución secuencial y por ende, se pasa a explicar sus diferencias.

El usuario deberá ingresar, además de la cantidad de bloques por lado y la longitud de cada bloque, el número de hilos con el que quiere trabajar. Recordar que trabajamos con potencias de dos.

Utilizamos la instrucción `#pragma omp parallel` con el fin de paralelizar distintas partes del código. Las partes puntualmente son: resolución del primer término, resolución del segundo término y la suma de los resultados de los otros términos. La cantidad de hilos a utilizar es la cantidad total, es decir, la ingresada por el usuario (que fue configurada con `omp_set_num_threads(NUM_THREADS);`).

Luego de esta primera gran división en partes de la resolución del problema, cada una de las operaciones (que recordemos, se resuelven con `for`) cuenta con un encabezado `#pragma omp for`, el cual indica que se va a paralelizar la ejecución de un `for`. Además, se ponen como privadas variables que serán utilizadas por todos los hilos, con el fin de que un hilo no vea las modificaciones hechas por otro, y no haya conflicto entre sus procesamientos. Por último, también se utiliza la claúsula `collapse(N)`, la cual nos garantiza el uso de Threads para los valores I y J, utilizados en los `for` "mas de afuera", los cuales son utilizados para elegir el bloque a computar por cada hilo.

No hay mas diferencias con la solución secuencial.

### Mediciones

##### Tiempo con un solo hilo en las soluciones con Pthreads y OpenMP:

| Tamaño de la matriz | Tiempo secuencial | Tiempo con Pthreads | Tiempo con OpenMP |
| --------------------|:-----------------:|:-------------------:|:-----------------:|
| 4 bloques de 128    | 4,6964404         | 4,866993667         | 4.766518          |
| 8 bloques de 128    | 37,3414172        | 38,871684           | 38.231288         |


##### Tiempo con dos hilos en las soluciones con Pthreads y OpenMP:

| Tamaño de la matriz | Tiempo secuencial | Tiempo con Pthreads | Tiempo con OpenMP |
| --------------------|:-----------------:|:-------------------:|:-----------------:|
| 4 bloques de 128    | 4,6964404         | 2.693269            | 2.651285          |
| 8 bloques de 128    | 37,3414172        | 21.589481           | 21.196580         |

##### Tiempo con cuatro hilos en las soluciones con Pthreads y OpenMP:

| Tamaño de la matriz | Tiempo secuencial | Tiempo con Pthreads | Tiempo con OpenMP |
| --------------------|:-----------------:|:-------------------:|:-----------------:|
| 4 bloques de 128    | 4,6964404         | 1.528396            | 1.453902          |
| 8 bloques de 128    | 37,3414172        | 11.664970           | 11.401058         |

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

##### Tiempo con un solo hilo en las soluciones con Pthreads y OpenMP:

| Longitud del vector | Tiempo secuencial | Tiempo con Pthreads | Tiempo con OpenMP |
| --------------------|:-----------------:|:-------------------:|:-----------------:|
| 2^28 = 268435456    | 1.380244          | 1.383570            | 1.388685          |
| 2^30 = 1073741824   | 5,3868828         | 5,6646752           | 5,6232478         |

##### Speedup con un solo hilo en las soluciones con Pthreads y OpenMP:

| Longitud del vector | Speedup con Pthreads | Speedup con OpenMP |
| --------------------|:--------------------:|:------------------:|
| 2^28 = 268435456    | 0,997596074          | 0,993921588        |
| 2^30 = 1073741824   | 0,950960578          | 0,957966462        |

##### Eficiencia con un solo hilo en las soluciones con Pthreads y OpenMP:

| Longitud del vector | Eficiencia con Pthreads | Eficiencia con OpenMP |
| --------------------|:-----------------------:|:---------------------:|
| 2^28 = 268435456    | 0,997596074             | 0,993921588           |
| 2^30 = 1073741824   | 0,950960578             | 0,957966462           |


##### Tiempo con dos hilos en las soluciones con Pthreads y OpenMP:

| Longitud del vector | Tiempo secuencial | Tiempo con Pthreads | Tiempo con OpenMP |
| --------------------|:-----------------:|:-------------------:|:-----------------:|
| 2^28 = 268435456    | 1.380244          | 0.724567            | 0,7225472         |
| 2^30 = 1073741824   | 5,3868828         | 2,8348112           | 2,8872884         |

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

| Longitud del vector | Tiempo secuencial | Tiempo con Pthreads | Tiempo con OpenMP |
| --------------------|:-----------------:|:-------------------:|:-----------------:|
| 2^28 = 268435456    | 1.380244          | 0,380976            | 0.392383          |
| 2^30 = 1073741824   | 5,3868828         | 1,503635            | 1.535451          |

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
