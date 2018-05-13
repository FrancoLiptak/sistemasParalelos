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