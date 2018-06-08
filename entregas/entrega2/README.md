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
