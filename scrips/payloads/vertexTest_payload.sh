#! /bin/bash

processor_count=4

# Note: 256.in doesn't exist

for v in {256,512,1024,2048,4096}; do
	mpirun -n $processor_count bin/project -tf data/examples/"$v".in -o a;
done;
