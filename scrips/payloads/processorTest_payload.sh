#! /bin/bash

vertex_count=16

for p in {1,2,4,8,16}; do
	mpirun --hostfile host -n $p bin/project -tf data/examples/"$vertex_count".in -o a;
done;
