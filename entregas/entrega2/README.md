# Programación con memoria distribuida

## Aclaraciones

El presente trabajo ha sido desarrollado en grupo por Franco Emanuel Liptak y Gastón Gustavo Ríos. Consiste en un ejercicio práctico pedido por la cátedra, con dos soluciones diferentes (secuencial y con OpenMPI). El objetivo del trabajo fue resolver el ejercicio con distintas tecnologías, a fin de poder visualizar la mejora del rendimiento de OpenMPI, respecto al algoritmo secencial.

Si bien solo se han pedido los archivos de extensión '.c' (es decir, los archivos que serán utilizados por el compilador, para compilar en base a la gramática y sintaxis de C), se ofrece también la posibilidad de descargar los archivos ya compilados desde el siguiente repositorio de GitHub: https://github.com/okason97/sistemasParalelos, el cual por supuesto, tiene como colaboradores a ambos integrantes del grupo. 

Nuestros usuarios de GitHub son:
- okason97 (Gastón)
- FrancoLiptak (Franco)

Las pruebas se hicieron en las computadoras del aula donde cursamos la materia.

## Ejercicio - Juego de las N-Reinas

*"El juego de las N-Reinas consiste en ubicar sobre un tablero de ajedrez N reinas sin que estas se amenacen entre ellas. Una reina amenaza a aquellas reinas que se encuentren en su misma fila, columna o diagonal.
La solución al problema de las N-Reinas consiste en encontrar todas las posibles soluciones para un tablero de tamaño NxN."*

### Resolución secuencial

Para ejecutar el script el usuario debe ingresar como parámetro un valor N, siendo N el tamaño del tablero de NxN donde se ubicarán las reinas.

A continuación describiremos el funcionamiento del script:

Primero se declaran e inicializan las variables necesarias, y se alocan los vectores a utilizar.

Los vectores que utilizaremos serán `queens`, `col_available`, `asc_diagonal` y `des_diagonal`:

- En el vector `queens` se almacenarán las posiciones de las reinas, dado que según las reglas impuestas solo podrá haber una reina por fila. Se utiliza el índice del vector de `queens` como indicador de fila y el valor como indicador de columna. De este modo, `queens[i] = j` indicaría que la reina está posicionada en la posición (i,j).

- Los vectores `col_available`, `asc_diagonal` y `des_diagonal` aseguran de forma eficiente que las reinas no se interfieran entre si. Cada vez que una reina se posiciona reservará las 2 diagonales y la columna para que ninguna reina pueda posicionarse en éstas. Esto permite verificar que una columna para una reina dada sea válida en 1 acceso por vector. 

Se puede pensar el problema como N árboles, con N siendo la cantidad de filas o columnas del tablero. Cada posición de fila de una reina es un nivel del árbol, comenzando con la posición de fila 0 como la raíz, la cual tendrá en cada árbol un valor `j` diferente, siendo `j` la columna en la que se posiciona la reina.
De esta raíz se desprenden las posibles variaciones de `j` válidas de la reina posicionada en la siguiente fila, y esto se repite hasta llegar a la fila N, en cuyo caso se ha encontrado una solución, o hasta llegar a una fila en la cual no existen posiciones de columna válidas.

Nuestra solución realiza el recorrido de este árbol construyendo y deconstruyendo el árbol en cada iteración y contando la cantidad de veces en las que se llegó a la fila N (cantidad de soluciones posibles).

Para el posicionamiento de las reinas, posicionamos a cada reina `i` en una posición de columna `j` válida. La válidez de esta posición `j` está dada por la disponibilidad de una columna (`col_available`), una diagonal ascendente (`asc_diagonal`) y una diagonal descendente (`des_diagonal`) que no esté siendo utilizada por ninguna otra reina. Si la reina ya fue posicionada en un paso anterior, esta buscada un valor de `j` superior. De esta forma se evita que la reina pase por valores que ya había utilizado.

Es posible que se llegue a una reina a la cual no se le puede asignar ningón valor de `j`. Esto se debe a que no existe una solución al problema en esa rama o a que ya se han recorrido todos los valores válidos para el nivel del árbol. En ambos casos se debe subir un nivel para buscar otra rama válida. En caso de haberle podido asignar un valor de `j` a la reina en la última posición entonces se aumenta el número de soluciones posibles, ya que se le ha asignado una posición a cada reina.

Finalmente imprimimos el resultado, el tiempo de ejecución y liberamos los vectores alocados.

### Resolución con MPI


Para ejecutar el script el usuario debe ingresar como parámetro un valor N, siendo N el tamaño del tablero de NxN donde se ubicarán las reinas.

A continuación describiremos el funcionamiento del script:

Primero se declaran e inicializan las variables necesarias, y se alocan los vectores a utilizar.
Los vectores que utilizaremos serán `queens`, `col_available`, `asc_diagonal` y `des_diagonal`, los cuales cumplirán la misma función que en la solución secuencial.

La resolución con MPI está basada en nuestra solución secuencial. La diferencia es que esta vez tenemos que distribuir el trabajo en distintos procesos que van a colaborar entre sí para lograr el resultado esperado en el menor tiempo posible. Para eso, usamos el modelo 'Master-Worker'. 

El proceso 0 será nuestro 'Master'/'Coordinador', y se encargará de repartir el trabajo (bajo demanda) a los procesos restantes ('Workers'). Cada proceso 'Worker' va a solitar trabajo al 'Master', el cual puede responder con un valor de columna para la primer reina de una solución. Así, cada proceso trabaja con uno de los árboles que representaría todas las soluciones posibles para un determinado valor de la primer reina. Vale aclarar que el 'Master', además de distribuir el trabajo entre los 'Workers', también buscará soluciones. Podemos resumir el trabajo del 'Coordinador' de la siguiente manera:

- Luego de la distribución de trabajo inicial, por cada valor de columna aún no asignado el 'coordinador' recibirá de forma no bloqueante a los procesos que hayan terminado su trabajo y requieran de otro valor de columna para la primer reina. Mientras espera que algún worker le solicite trabajo, el 'coordinador' realizará procesamiento utilizando un valor de columna para la primer reina no asignado a un 'worker', lo cual disminuye el desbalance de carga entre los procesos.

- Al finalizar el procesamiento de todos los valores de columna posibles para la primer reina, se le envia a cada proceso el valor -1, el cual utilizarán para determinar que se han terminado de procesar todos los valores de columna posibles para la primer reina.

Los 'workers' realizarán un procesamiento similar al secuencial pero sólo para un valor de columna para la primer reina, y al finalizar el procesamiento requerirán al 'master' (coordinador) otro valor de columna para la primer reina.

Una vez terminado el procesamiento y con los workers avisados de esto, se procede a realizar la reducción de la cantidad de soluciones encontradas (local a cada proceso) a una sola variable.

Finalmente imprimimos el resultado, el tiempo de ejecución local a cada proceso y también el total, y liberamos los vectores alocados.

### Mediciones

##### Tabla con tiempos de ejecucion en secuencial

| Tamaño del tablero | Tiempo de ejecución | Cantidad de soluciones |
| -------------------|:-------------------:|:----------------------:|
|         8          |            0.000390 |                     92 |
|         9          |            0.001637 |                    352 |
|        10          |            0.007326 |                    724 |
|        11          |            0.029500 |                   2680 |
|        12          |            0.124517 |                  14200 |
|        13          |            0.551008 |                  73712 |
|        14          |            3.312708 |                 365596 |
|        15          |           21.080764 |               2279184 |

##### Tablas con tiempo de ejecucion en mpi con 1 computadora y 4 procesos

| Tamaño del tablero | Tiempo de ejecución | Cantidad de soluciones | Tiempo proceso 0 | Tiempo proceso 1 |  Tiempo proceso 2 |  Tiempo proceso 3 | Desbalance de carga |   Speedup    | Eficiencia |
| -------------------|:-------------------:|:----------------------:|:----------------:|:----------------:|:-----------------:|:-----------------:|:-------------------:|:------------:|:---------:|
|         8          |            0.000223 |                     92 |         0.000080 |         0.000112 |          0.000146 |          0.000039 |            0.700265 |     1.748878 |    0.437219 |
|         9          |            0.000698 |                    352 |         0.000256 |         0.000454 |          0.000364 |          0.000312 |            0.571428 | 2.3452722063 | 0.586318 |
|        10          |            0.001644 |                    724 |         0.001366 |         0.000931 |          0.001376 |          0.000966 |            0.383703 |     4.456204 |  1.114051 |
|        11          |            0.008698 |                   2680 |         0.006630 |         0.008386 |          0.004440 |          0.004562 |            0.657173 |     3.391584 |  0.847896 |
|        12          |            0.042872 |                  14200 |         0.032806 |         0.030036 |          0.031008 |          0.031314 |            0.088523 |     2.904389 |  0.726097 |
|        13          |            0.194765 |                  73712 |         0.194546 |         0.144610 |          0.157997 |          0.136911 |            0.363591 |     2.829091 |  0.707272 |
|        14          |            1.237127 |                 365596 |         1.014228 |         0.884690 |          0.780851 |          0.853057 |            0.264238 |     2.677742 |  0.669435 |
|        15          |            7.445144 |                2279184 |         6.086316 |         5.719995 |          4.381050 |          5.981710 |            0.307683 |     2.831478 |  0.707869 |

##### Tabla con tiempo de ejecucion en mpi con 2 computadoras con 2 procesos cada una, 4 procesos total

| Tamaño del tablero | Tiempo de ejecución | Cantidad de soluciones | Tiempo proceso 0 | Tiempo proceso 1 |  Tiempo proceso 2 |  Tiempo proceso 3 | Desbalance de carga | Speedup | Eficiencia |
| -------------------|:-------------------:|:----------------------:|:----------------:|:----------------:|:-----------------:|:-----------------:|:-------------------:|:-------:|:---------:|
|         8          |            0.007364 |                     92 |         0.000120 |         0.000203 |          0.000000 |          0.000000 |  |  |
|         9          |            0.006572 |                    352 |         0.000457 |         0.000919 |          0.000000 |          0.000000 |  |  |
|        10          |            0.006524 |                    724 |         0.002935 |         0.003093 |          0.000000 |          0.000000 |  |  |
|        11          |            0.015640 |                   2680 |         0.008088 |         0.013081 |          0.003531 |          0.002011 |  |  |
|        12          |            0.050287 |                  14200 |         0.036216 |         0.025657 |          0.027985 |          0.035003 |  |  |
|        13          |            0.201711 |                  73712 |         0.155578 |         0.151941 |          0.127898 |          0.164201 |  |  |
|        14          |            0.981952 |                 365596 |         0.960347 |         0.725137 |          0.916139 |          0.720085 |  |  |
|        15          |            6.958466 |                2279184 |         5.782346 |         4.242574 |          5.502807 |          5.664581 |  |  |

##### Tabla con tiempo de ejecucion en mpi con 2 computadoras con 4 procesos cada una, 8 procesos total

| Tamaño del tablero | Tiempo de ejecución | Cantidad de soluciones | Tiempo proceso 0 | Tiempo proceso 1 |  Tiempo proceso 2 |  Tiempo proceso 3 |  Tiempo proceso 4 |  Tiempo proceso 5 |  Tiempo proceso 6 |  Tiempo proceso 7 | Desbalance de carga | Speedup | Eficiencia |
| -------------------|:-------------------:|:----------------------:|:----------------------:|:----------------------:|:----------------------:|:----------------------:|:----------------------:|:----------------------:|:----------------------:|:----------------------:|:----------------------:|:----------------------:|:----------------------:|
|         8          | 0.006645 | 92 | 0.000041 | 0.000091 | 0.000130 | 0.000087 | 0.000000 | 0.000000 | 0.000000 | 0.000000 |  | |  |
|         9          | 0.007079 | 352 | 0.000159 | 0.000503 | 0.000294 | 0.000414 | 0.000000 | 0.000000 | 0.000000 | 0.000000 |  |  |  |
|        10          | 0.006692 | 724 | 0.000598 | 0.001761 | 0.001836 | 0.001871 | 0.000000 | 0.000000 | 0.000000 | 0.000000 |  |  |  |
|        11          | 0.010496 | 2680 | 0.004000 | 0.006244 | 0.006655 | 0.006682 | 0.000000 | 0.000000 | 0.000000 | 0.000000 |  |  |  |
|        12          | 0.030248 | 14200 | 0.012300 | 0.023600 | 0.023285 | 0.011136 | 0.008506 | 0.014534 | 0.017146 | 0.017006 |  |  |  |
|        13          | 0.106956 | 73712 | 0.054408 | 0.098133 | 0.093598 | 0.098900 | 0.089788 | 0.082675 | 0.045242 | 0.045746 |  |  |  |
|        14          | 0.559115 | 365596 | 0.278269 | 0.502483 | 0.502787 | 0.482148 | 0.289791 | 0.551848 | 0.494386 | 0.498609 |  |  | |
|        15          | 3.172357 | 2279184 | 1.619737 | 2.814372 | 2.903924 | 2.988094 | 2.832957 | 2.998604 | 2.712363 | 3.060810 |  |  |  |

No calculamos desbalance de carga, speedup ni eficiencia de algunos casos de prueba debido a un bug en el programa, el cual ya fue solucionado.
El bug se encontraba en el coordinador, el cual que esperaba un mensaje para devolver un valor inicial de columna para la primer reina de cualquier otro proceso. Esto podía llevar a que algunos procesos se queden sin recibir ningún valor de columna inicial.
Será necesario realizar las pruebas nuevamente con la resolución correcta, la cual ya fue subida al repositorio y entregada a los profesores de la cátedra.
