# Prime Number Generation using  Parallelised Sieve of Eratosthenes Algorithm

## Aim

- To find all primes less than N (an input integer) using a parallelized version of Sieve of Eratosthenes algorithm on a Message Passing model.

## Approach

- Our approach uses the Single Program Multi-Data model of programming.
- Let the number of processes to be used for achieving the aim be P. The range (from 0 to N) is divided into P equal parts each of Ceil(N/p) size, thus each process has roughly equal distribution of load. Such an approach remains 
- 