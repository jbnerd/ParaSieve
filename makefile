
compile : sieve.c
	mpicc sieve.c -lm -o exe

run :
	mpirun -np $(n) ./exe $(limit)