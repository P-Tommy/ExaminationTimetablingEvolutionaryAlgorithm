# Examination Timetabling Evolutionary Algorithm #
The following software is an implementation of an Evolutionary Algorithm using mutation and Hill-Climbing
to solve the Examination Timetabling problem.

## Dataset ##
To execute the program, you need a DataSet (the dataset used was the one from University of Toronto Benchmark
Data http://www.asap.cs.nott.ac.uk/external/resources/files/Toronto.zip) with .crs and .stu files.

The .crs file need to have the following structure:

```
    0001 123
    0002 113
    ...
    9999 102
```

where the first number is an ID for the exam, and the second is the total number of students taking that exam.
There must be one line per exam.

The .stu file contains the information about the students, such as:

```
    0417
    0316 0317 0318
    0001
```

where each line is a student, and each number is an exam that the student is taking.

## Usage ##
To execute the program, you need to compile it first (use the MAKEFILE), and then run it
by passing the name of the dataset as an argument (without the file extension of the dataset).
For example:

  `$ ./min_timeslots car-f-92`

will run the program for the `car-f-92` dataset.

## Parameters ##
At the beggining of the code there is a set of parameters to adjust the algorithm.
They are:

* `POP_SIZE`: Size of the population
* `MAX_GENERATIONS`: Total number of generations to run (after that the algorithm stops)
* `PROB_MUTATION`: The probability of a mutation ocurring
* `PROB_CLIMB`: The probability of a Hill-Climbing ocurrs
* `HC_ITERATIONS`: The number of iterations for the Hill-Climbing
* `MAX_FEASIBLE_RETRIES`: The maximum amount of retries in the mutation to achieve a feasable solution
* `TOURNAMENT_SIZE`: The size of the tournament for the selection

## Output ##
The program will show the best aptitude for each generation, and the cumulative time of execution.

When the algorithm finishes, it shows the best solution it found, the total time used, and the
aptitude of that solution.