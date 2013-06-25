# README #

## Requirements ##
This program requires the BOOST library (http://www.boost.org/ tested with 1.53.0). A copy is included
in the boost folder with this project.

## Compilation ##
You can use the included makefile to do the compilation, in that case, just do

  `$ make`

in the project's folder and a min_timeslots executable will be generated.

If you want to do a manual compilation, just use g++ or c++ and be sure
to include boost

  `$ g++ -W -I /boost min_timeslots.cpp -o min_timeslots`

  `$ g++ -W -I /boost min_cost.cpp -o min_cost`

Note: If boost is installed in your system, you don't need to include it in the compilation
process.

## Dataset ##
To execute the program, you need a dataset (it was tested with the University of Toronto Benchmark
Data http://www.asap.cs.nott.ac.uk/external/resources/files/Toronto.zip) that has a .crs and a .stu
files. The .crs file should have the structure:

```
    0001 123
    0002 113
    ...
    9999 102
```

(only whitespace is a single space between the first number and the second)
where the first is an identifier for the exam, and the second is the total of students taking that exam.
There should be one line per exam.
The other file, .stu, has the students information, for example:

```
    0417
    0316 0317 0318
    0001
```

where each line represents a student, and each number an exam that the student is taking. If there is
more than one exam in the line, they are in conflict.

## Usage ##
To execute the program, you just need to execute the compiled executable and pass it the name
of the set (no extension) you want to test (the two files need to be in the same folder).

For example, to use the car-f-92.crs and car-f-92.stu files:

  `$ ./min_timeslots car-f-92`

For the `min_cost` program, you need to also
specify the total number of timeslots to be used

  `$ ./min_cost car-f-92 35`

## Parameters ##
In the beggining of the file min_timeslots.cpp there are a set of parameters that can be changed
to tune the performance of the algorithm. They are:

* `POP_SIZE`: The size of the population.
* `MAX_GENERATIONS`: The total number of generations to execute. After that, the algorithm will stop.
* `PROB_MUTATION`: The probability that a mutation happens to one gene of a genotype.
* `PROB_CLIMB`: The probability that a solution is passed through the Hill Climb algorithm.
* `HC_ITERATIONS`: The total iterations for the Hill Climbing algorithm.
* `MAX_FEASIBLE_RETRIES`: The maximum retries to do to try to get a feasible solution.
* `TOURNAMENT_SIZE`: The size of the tournament selection algorithm.

in the case of the `min_cost` program, there
are these additional parameters:

* `WX`: Penalization for two conflicting exams separated
by a distance of X

## Output ##
The program will output in STDOUT the aptitude of the best solution of each generation, and the (acumulative)
time elapsed since the beggining of the algorithm.

When the algorithm is done (it has generated all the generations it was set to do), it will output the
best solution, the total time it took to get it, and the aptitude (the total timeslots used or the cost
per student) of that solution.
The format of the solution is the timeslot assigned for each exam, ordered by exam id, separated by a space.