
# Parallel Performance Metrics

## Execution Time
parallel run time for code (run time of all processes to finish their work)

## Parallel Overhead
To = pc x Tp - Ts
	- To: overhead time
	- pc: processor count
	- Tp: Parallel execution time
	- Tp: Serial execution time

## Speedup
ratio of serial to parallel execution time
S = Ts / Tp

- should be limited by S =  Ts / p 

## Efficiency
fraction of execution that is us usefully parallel. ie, speed up per processor
E = s/p = Ts / (p x Tp)

## Cost
quantifies resource needed to achieve performance
- irrelevant to project?


# Notes:
- doesnt check if more processors than vertices, all processors with rank higher than vertec count will do the same operation as the last valid processor
	- could have fixed by making a new communicator or checking for it


# Parallel Complexity:
'''
for k in m
	bcast if k is local, recv bcast if k not local
	for j in sm
		for i in sm
			do comparison
'''

- k for loop has O(V) 
	- bcast has const m 
	- j for loop has O(V)
		- i for loop has O(V)

therefore: O(V^3 + (V x m)) = O(V(V^2 + m))
