# README #

## Dataset ##
Para ejecutar el programa, se necesita un dataset (se ha probado con el dataset University of Toronto Benchmark
Data http://www.asap.cs.nott.ac.uk/external/resources/files/Toronto.zip) que tiene un archivo .crs y .stu.
El archivo .crs debe tener la estructura:

```
    0001 123
    0002 113
    ...
    9999 102
```

donde el primer número es un identificador del examen, y el segundo es el total de estudiantes
tomando ese examen.
Debe haber una línea por examen.

El archivo .stu contiene la información de los estudiantes, por ejemplo:

```
    0417
    0316 0317 0318
    0001
```

donde cada línea representa a un estudiante, y cada número es un examen que el estudiante está tomando.

## Uso ##
Para ejecutar el programa, se debe compilar el ejecutable y pasarle el nombre del dataset (sin extensión)
que se queire probar (se debe tener ambos archivos en la misma carpeta). Por ejemplo:

  `$ ./min_timeslots car-f-92`

## Parámetros ##
En el comienzo del archivo min_timeslots.cpp hay un conjunto de parámetros que pueden ser modificados
para mejorar el comportamiento del algoritmo en algunos casos. Son:

* `POP_SIZE`: Tamaño de la población
* `MAX_GENERATIONS`: Total de generaciones a ejecutar (después de ellas se detiene el algoritmo)
* `PROB_MUTATION`: La probabilidad que ocurra una mutación
* `PROB_CLIMB`: La probabilidad que ocurra un Hill-Climbing
* `HC_ITERATIONS`: La cantidad de iteraciones que se llevan a cabo en el Hill-Climbing
* `MAX_FEASIBLE_RETRIES`: La máxima cantidad de reintentos en la mutación para alcanzar una solución factible
* `TOURNAMENT_SIZE`: El tamaño del torneo para la selección

Además, al comienzo de la rutina main se da una semilla para generar los números aleatorios. Para las pruebas
se ocuparon las siguientes:

  srand(10000);
  srand(123456);
  srand(854234);

## Output ##
El programa mostrará la mejor aptitud de cada generación y el tiempo acumulado de ejecución.

Cuando el algoritmo termina, muestra la mejor solución, el tiempo total utilizado, y la aptitud
de ésta solución.

El formato de la solución es que cada número representa el id del timeslot asignado a un examen.