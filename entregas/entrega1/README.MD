# Programación con memoria compartida

## Aclaraciones

El presente trabajo ha sido desarrollado en grupo por Franco Emanuel Liptak y Gastón Gustavo Ríos. Consiste en dos ejercicios prácticos pedidos por la cátedra, donde cada uno tiene 3 versiones diferentes (secuencial, resolución con Pthreads, y resolución con OpenMP). El objetivo del trabajo fue resolver los ejercicios con distintas tecnologías, a fin de poder visualizar la mejora del rendimiento entre cada una de las tecnologías de trabajo paralelo (Pthreads y OpenMP), respecto al algoritmo secencial.

Si bien solo se han pedido los archivos de extensión '.c' (es decir, los archivos que serán utilizados por el compilador, para compilar en base a la gramática y sintaxis de C), se ofrece también la posibilidad de descargar los archivos ya compilados desde el siguiente repositorio de GitHub: https://github.com/okason97/sistemasParalelos, el cual por supuesto, tiene como colaboradores a ambos integrantes del grupo. 

Nuestros usuarios de GitHub son:
- okason97 (Gastón)
- FrancoLiptak (Franco)

## Ejercicio 1

## Mediciones

##### Tiempo con un solo hilo en las soluciones con Pthreads y OpenMP:

| Longitud del vector | Tiempo secuencial | Tiempo con Pthreads | Tiempo con OpenMP |
| --------------------|:-----------------:|:-------------------:|:-----------------:|

##### Tiempo con dos hils en las soluciones con Pthreads y OpenMP:

| Longitud del vector | Tiempo secuencial | Tiempo con Pthreads | Tiempo con OpenMP |
| --------------------|:-----------------:|:-------------------:|:-----------------:|

##### Tiempo con cuatro hilos en las soluciones con Pthreads y OpenMP:

| Longitud del vector | Tiempo secuencial | Tiempo con Pthreads | Tiempo con OpenMP |
| --------------------|:-----------------:|:-------------------:|:-----------------:|

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

- `parallel`: especifica que el bloque de código será ejecutado en paralelo. Cada hilo tendrá un ID único. Finalizada la región paralela, solo el hilo master continúa con la ejecución.
- `for`: indica que se va a paralelizar la ejecución de un 'for'. En nuestro caso, recordar que el recorrido del vector se realiza con este iterador.
- `reduction(+:pares)`: en nuestro caso, indica a cada hilo que se debe realizar una suma sobre la variable indicada.

Con eso terminan las diferencias respecto a la solución secuencial.

## Mediciones

##### Tiempo con un solo hilo en las soluciones con Pthreads y OpenMP:

| Longitud del vector | Tiempo secuencial | Tiempo con Pthreads | Tiempo con OpenMP |
| --------------------|:-----------------:|:-------------------:|:-----------------:|

##### Tiempo con dos hils en las soluciones con Pthreads y OpenMP:

| Longitud del vector | Tiempo secuencial | Tiempo con Pthreads | Tiempo con OpenMP |
| --------------------|:-----------------:|:-------------------:|:-----------------:|

##### Tiempo con cuatro hilos en las soluciones con Pthreads y OpenMP:

| Longitud del vector | Tiempo secuencial | Tiempo con Pthreads | Tiempo con OpenMP |
| --------------------|:-----------------:|:-------------------:|:-----------------:|