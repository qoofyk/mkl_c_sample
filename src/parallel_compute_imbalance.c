//==============================================================
//
// SAMPLE SOURCE CODE - SUBJECT TO THE TERMS OF SAMPLE CODE LICENSE AGREEMENT,
// http://software.intel.com/en-us/articles/intel-sample-source-code-license-agreement/
//
// Copyright 2016-2017 Intel Corporation
//
// THIS FILE IS PROVIDED "AS IS" WITH NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT
// NOT LIMITED TO ANY IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE, NON-INFRINGEMENT OF INTELLECTUAL PROPERTY RIGHTS.
//
// =============================================================

/*******************************************************************************
*   This example demonstrates threading impact on load imbalance kernel 
*   where A, B, and C 
*   are matrices and alpha and beta are double precision scalars. 
*
*   In this simple example, practices such as memory management, data alignment, 
*   and I/O that are necessary for good programming style and high Intel(R) MKL 
*   performance are omitted to improve readability.
********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "mkl.h"
#include <omp.h>
#include <math.h>
#include <limits.h>
#include <string.h>
#include "eval_tools.h"

/* Consider adjusting LOOP_COUNT based on the performance of your computer */
/* to make sure that total run time is at least 1 second */
#define LOOP_COUNT 200 

int main(int argc, char** argv)
{
    double time[128]; 
    int threads = atoi(getenv("OMP_NUM_THREADS"));
    printf ("\n This example demonstrates threading impact on load imbalance kernel \n"
            " A[i]= A[i]^1 + ... + A[i]^bound\n"
            " double precision scalars \n\n");
 
#pragma omp parallel 
{    
    int myid = omp_get_thread_num();
    printf("I am thread%02d\n", myid); 

    double *A, *B, *C;
    int m, n, p, i, j, k, r, max_threads, loop_cnt;
    double alpha, beta;
    double sum;
    double s_initial, s_elapsed;

    loop_cnt = LOOP_COUNT;

    m =  p =  n = 2560;
    int bound=20;

    //parse command line
    for (i = 1; i < argc; i++) {
      if (!strcmp(argv[i], "-size")) {
            m =  p =  n = atoi(argv[++i]);
      }
      if (!strcmp(argv[i], "-cnt")) {
            loop_cnt = atoi(argv[++i]);
      }
      if (!strcmp(argv[i], "-bound")) {
            bound = atoi(argv[++i]);
      }
    }    

    int N = m;

    // Use current time as seed for random generator
    srand((dsecnd()+myid)); 
    bound = rand() % bound;

    printf (" N=%d, loop_cnt=%d, bound=%d\n", N, loop_cnt, bound);

    if(myid==1)
        printf (" Initializing data for matrix multiplication C=A*B for matrix \n"
            " A(%ix%i) and matrix B(%ix%i)\n\n", m, p, p, n);
    alpha = 1.0; beta = 0.0;
    
    if(myid==1)
        printf (" Allocating memory for matrices aligned on 64-byte boundary for better \n"
            " performance \n\n");
    A = (double *)mkl_malloc( m*p*sizeof( double ), 64 );
    if (A == NULL) {
        printf( "\n ERROR: Can't allocate memory for matrices. Aborting... \n\n");
        mkl_free(A);
    }

    if(myid==1)
        printf (" Intializing matrix data \n\n");
    for (i = 0; i < (m*p); i++) {
        A[i] = (double)(i+1);
    }

    if(myid==1)
        printf (" Finding max number of threads Intel(R) MKL can use for parallel runs \n\n");
    max_threads = mkl_get_max_threads();

    if(myid==1)
        printf (" Running Intel(R) MKL from 1 to %i threads \n\n", max_threads);
    for (int num_threads = 1; num_threads <= max_threads; num_threads++) {

        // my computer kernel
        double temp;
        if(myid==1)
            printf (" Making the first run of Compute Bound kernel\n"
                    " to get stable run time measurements \n\n");
        for (i = 0; i < m*p; i++) {
            temp=A[i];
            sum = temp;
            for (j=0; j<bound; j++){
                    temp *=temp;
                    sum += temp;
                }    
            A[i] = sum;
        }

        if(myid==1)
            printf (" Measuring performance of Compute Bound kernel\n\n");

        s_initial = get_cur_time();

        for (r = 0; r < loop_cnt; r++) {
             for (i = 0; i < m*p; i++) {
                temp=A[i];
                sum = temp;
                for (j=0; j<bound; j++){
                    temp *=temp;
                    sum += temp;
                }    
                A[i] = sum;
            }    
        }

        s_elapsed = (get_cur_time() - s_initial) / loop_cnt;
        time[myid] = s_elapsed * 1000;
        
        printf (" == Compute Bound kernel completed ==\n"
                " == at %.5f milliseconds, %.2f seconds, using %d thread(s) MFLOPS=%.3f ==\n\n", 
                (s_elapsed * 1000), s_elapsed*loop_cnt, num_threads, 2*(double)N*bound / s_elapsed*1e-6);
    }
    
    if(myid==1)
        printf (" Deallocating memory \n\n");
    mkl_free(A);
    
    if (s_elapsed < 0.9/loop_cnt) {
        s_elapsed=1.0/loop_cnt/s_elapsed;
        i=(int)(s_elapsed*loop_cnt)+1;
        printf(" It is highly recommended to define LOOP_COUNT for this example on your \n"
               " computer as %i to have total execution time about 1 second for reliability \n"
               " of measurements\n\n", i);
    }
}
    // compute average
    double average=0.0;
    for (int i=0; i < threads; i++)
        average += time[i]; 
    average = average / threads;
    printf(" AE= %.3f ms\n", average);

    printf (" Example completed. \n\n");
    return 0;
 
}
