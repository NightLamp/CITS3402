# CITS3402 Assignment 2

## About this project
This project implements floyd-warshall in a distributed manner using MPI 
on Linux. It has been tested using Ubuntu 18.04.3 LTS and Open MPI 2.1.1.

## How To Use
The following steps will get you running this project.

### Dependencies:
* some MPI implementation

### Building
To build the project, simply run:
``` 
$ make
```
The compiled binary will be present as ./bin/project.

To test that the code is operational, you can run:
```
$ make run
```

### Running
Running the program requires the command mpirun (or mpiexec) used as follows:
```
$ mpirun ./bin/project -f <input_file>
```


## Command Line Options
There are 5 flags that can be used with this proogram.
- \-s: sequential
- \-t: timer
- \-v: verbose
- \-f: file input
- \-o: file output

## Useful links:

https://parallelcomp.github.io/Floyd.pdf

https://github.com/nuno-azevedo/floyd-warshall-mpi

https://tsakiris.wordpress.com/2013/07/25/parallelization-of-floyd-warshall-with-mpi-c/
