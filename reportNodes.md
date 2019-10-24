
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
