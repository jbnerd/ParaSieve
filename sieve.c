#include <stdio.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <mpi.h>
#include <stdlib.h>
#define UPPER_BOUND 10000000000
#define MODE 1  // 0 to forsake CL-ARG
#define PRINT 0 // 1 to print primes.
#define PAGE 2500


#define SetBit(A,k)     ( A[(k/32)] |= (1 << (k%32)) )
#define ClearBit(A,k)   ( A[(k/32)] &= ~(1 << (k%32)) )  
#define TestBit(A,k)    ( A[(k/32)] & (1 << (k%32)) ) 
#define MAX( a,b)  ( (a > b) ? a : b  )
#define MIN( a,b)  ( (a < b) ? a : b  )


int getRootPrimes(long limit , int** rootPrimes){
    int i =0, count =0;
    long llsqrt = ceil(sqrt(limit));
    int prime =2;
    int *arr = (int *)calloc(ceil(limit/32), sizeof(int));
    while(1){
      while (prime <= llsqrt && TestBit(arr,prime))
            prime ++;
      for (i = prime * prime; i <= limit; i += prime)
                SetBit(arr,i);
      prime ++;
      if(prime > llsqrt) break;
    }
    for(i=2;i<limit;i++){
        if(TestBit(arr,i))
        count+=1;
     }   
     
    (*rootPrimes) = (int *)calloc(count, sizeof(int));
    count =0;
    for(i=2;i<limit;i++){
        if(!TestBit(arr,i)){
        (*rootPrimes)[count] = i;
        count+=1;
        }
     }
     return count;
}

int small(long limit){
    int* rootPrimes ;
    long base;
    return getRootPrimes(limit , &rootPrimes);
}

int removeComposites(long baseIndex, int limit , int* arr, int* rootPrimes, int rootCount)
{       int i =0, j =0, count = 0;
        long start, dprime;
        for( i = 0; i< rootCount; i++)
        {   
            dprime = 2*rootPrimes[i];
            start  = MAX(   (baseIndex+ (rootPrimes[i] - baseIndex%rootPrimes[i]))   ,  rootPrimes[i]*rootPrimes[i]  ) -  baseIndex;
            
            
            if( start%2 == 0 )
                start+=rootPrimes[i];
            if(rootPrimes[i]==2)
                dprime = 2;
            
            for(j = start ; j < limit; j+=dprime )
                 SetBit(arr,j);
        }
        for( i = 0; i< limit; i++){
            if(!TestBit(arr,i)){
                count+=1;
               printf(" %ld \n", (i+baseIndex));  
             }
        }
        return count;
}


int main(int argc, char *argv[]){
    
    long limit, n_hi, n_lo;
    if (argc > 1 && MODE == 1)
        limit = (long)pow(10,atoi(argv[1]));
    else
        limit = UPPER_BOUND;

    int p, id , root  = 0;
    double wtime;
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    if (id == 0)
        wtime = MPI_Wtime();
    if (id != p - 1)
       n_hi = (long)(limit / p) * (id + 1);
    else
       n_hi = limit;
    n_lo = (long)(limit / p) * id;
    

    long lsqrt = (long) ceil(sqrt(limit));
    
    if (lsqrt < 10000){
     if(id == root){  
        int ans = small(limit);
        wtime = MPI_Wtime() - wtime;
        printf("         N        Pi          Time\n");
        printf("  %10ld    %10d  %16f\n", limit, ans, wtime);
        }
        MPI_Finalize();
        exit(0);
    }

    long part_size = n_hi - n_lo;
    int* rootPrimes ;
    long base;
    int rootCount = getRootPrimes(lsqrt , &rootPrimes);
    int d = 0;
    long i;
    int *arr = (int *)calloc((int)ceil((n_hi-n_lo)/32), sizeof(int));
    if(n_lo == 0){
        SetBit(arr,1);
        SetBit(arr,4);       
    }
    SetBit(arr,0);

   long j = 0, count = 0;
   long start, dprime;
    
    for( i = 0; i < rootCount; i++)
    {   
        dprime = 2 * rootPrimes[i];
        if(n_lo == 0) start = ((long)rootPrimes[i])*((long)rootPrimes[i]);
        else start  = MAX(   ( (long)ceil( ((double)n_lo) / rootPrimes[i]) * rootPrimes[i])   ,  ((long)rootPrimes[i])*((long)rootPrimes[i])  ) -  n_lo;  

        if( start % 2 == 0 && rootPrimes[i] > 2)
            start += rootPrimes[i];
        if(rootPrimes[i] == 2)
            dprime = 2;

        for(j = start ; j < part_size; j += dprime )
                SetBit(arr,j);
    }

    int* all_range;
    if(id == root){
        all_range = (int*) calloc((limit/32), sizeof(int));
    }

    MPI_Gather(arr, (part_size/32), MPI_INT, all_range, (part_size/32), MPI_INT, root, MPI_COMM_WORLD);

   long total_count = 1;
    if(id == root){
        for(i = 1; i < limit; i += 2){
            if (!TestBit(all_range,i)){
                    total_count += 1;
                    if(PRINT == 1)
                    {   printf("%ld \n", i);
                    }
            }
        }
   }
    if (id == root) {
        wtime = MPI_Wtime() - wtime;
        printf("         N        Pi          Time\n");
        printf("  %10ld %10ld  %16f\n", limit, total_count, wtime);
    }
       
    MPI_Finalize();
    exit(0);
}        
       
       
       


