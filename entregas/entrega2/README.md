# Programación con memoria compartida

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

Para ejecutar el script el usuario debe ingresar como parametro un valor de N siendo N el tamaño del tablero de NxN donde se ubicaran las reinas.

A continuacion describiremos el funcionamiento del script.

Primero se declaran e inicializan las variables necesarias, y se alocan los vectores a utilizar.
Los vectores que utilizaremos seran `queens`, `col_available`, `asc_diagonal` y `des_diagonal`.
En el vector `queens` se almacenaran las posiciones de las reinas, dado que segun las reglas impuestas solo podra haber una reina por fila, se utiliza el indice del vector de `queens` como indicador de fila y el valor como indicador de columna, de este modo `queens[i] = j` indicaria que la reina esta posicionada en la posicion (i,j).
Los vectores `col_available`, `asc_diagonal` y `des_diagonal` aseguran de forma eficiente que las reinas no se coman entre si. Cada vez que una reina se posiciona reservara las 2 diagonales y la columna para que ninguna reina pueda posicionarse en estas. Esto permite verificar que una columna para una reina sea valida en 1 acceso por vector. 

Se puede pensar el problema como N arboles con N siendo la cantidad de filas o columnas del tablero, en el que cada posicion de fila de una reina es un nivel del arbol, comenzando con la posicion de fila 0 como la raiz la cual tendrá, en cada arbol, un valor j diferente, siendo j la columna en la que se posiciona la reina.
De esta raiz se desprenden las posibles variaciones de j validas de la reina posicionada en la siguiente fila y se repite esto hasta llegar a la fila N, en cuyo caso se ha encontrado una solucion, o hasta llegar a una fila en la cual no existen posiciones de columna validas.

Nuestra solucion realiza el recorrido de este arbol construyendo y deconstruyendo el arbol en cada iteracion y contando la cantidad de veces en las que se llego a la fila N (cantidad de soluciones posibles).
Para el posicionamiento de las reinas, posicionamos a cada reina i en una posicion de columna j valida. La validez de esta posicion j esta dada por la disponibilidad de una columna (`col_available`), una diagonal ascendente (`asc_diagonal`) y una diagonal descendente (`des_diagonal`) que no este siendo utilizada por ningun otra reina. Si la reina ya fue posicionada en un paso anterior, esta buscada un valor de j superior, de esta forma se evita que la reina pase por valores que ya habia utilizado.
Si se llega a una reina a la cual no se le puede asignar ningun valor de j, esto se debe a que no existe una solucion al problema en esa rama o a que ya se han recorrido todos los valores validos para el nivel del arbol. En ambos casos se debe subir un nivel para buscar otra rama valida.
En caso de haberle podido asignar un valor de j a la reina en la ultima posicion entonces se aumenta el numero de soluciones posibles, ya que se le ha asignado una posicion a cada reina.

Finalmente imprimimos el resultado, el tiempo de ejecucion y liberamos los vectores alocados.

### Resolución con MPI


Para ejecutar el script el usuario debe ingresar como parametro un valor de N siendo N el tamaño del tablero de NxN donde se ubicaran las reinas.

A continuacion describiremos el funcionamiento del script.


Primero se declaran e inicializan las variables necesarias, y se alocan los vectores a utilizar.
Los vectores que utilizaremos seran `queens`, `col_available`, `asc_diagonal` y `des_diagonal`, los cuales cumpliran la misma funcion que en la solucion secuencial.

Basados en lo planteado en la resolucion secuencial, asignaremos a cada proceso bajo demanda uno de los arboles que representaria cada valor de columna de la primer reina.
Utilizaremos el modelo master-worker, con el proceso 0 llamado coordinador funcionando como maestro y repartiendo trabajo bajo demanda a los demas procesos worker.

El coordinador repartira a cada proceso un valor de columna para la primer reina. Luego, por cada valor de columna aun no asignado el coordinador recibira de forma no bloqueante a los procesos que hayan terminado su trabajo y requieran de otro valor de columna para la primer reina. Mientras espera que algun worker le solicite trabajo, el coordinador realizara procesamiento utilizando un valor de columna para la primer reina no asignado a un worker, lo cual disminuye el desbalance de carga entre los procesos.
Al finalizar el procesamiento de todos los valores de columna posibles para la primer reina, se le envia a cada proceso el valor -1, el cual utilizaran para determinar que se han terminado de procesar todos los valores de columna posibles para la primer reina.
Los workers realizaran un procesamiento similar al secuencial pero solo para un valor de columna para la primer reina, y al finalizar el procesamiento requeriran al master (coordinador) otro valor de columna para la primer reina.
Una vez terminado el procesamiento y con los workers avisados de esto, se procede a realizar la reduccion de la cantidad de soluciones encontradas local a cada proceso a una sola variable.

Finalmente imprimimos el resultado, el tiempo de ejecucion local y total, y liberamos los vectores alocados.

### Mediciones

