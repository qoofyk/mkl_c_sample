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
*   This example demonstrates threading impact on computing real matrix product 
*   C=alpha*A*B+beta*C using Intel(R) MKL subroutine DGEMM, where A, B, and C 
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

/* Consider adjusting LOOP_COUNT based on the performance of your computer */
/* to make sure that total run time is at least 1 second */
#define LOOP_COUNT 1000  

int main()
{

#pragma omp parallel 
{    
    printf("I am thread%d\n",omp_get_thread_num()); 

    double *A, *B, *C;
    int m, n, p, i, j, r, max_threads;
    double alpha, beta;
    double s_initial, s_elapsed;

    printf ("\n This example demonstrates threading impact on computing real matrix product \n"
            " C=alpha*A*B+beta*C using Intel(R) MKL function dgemm, where A, B, and C are \n"
            " matrices and alpha and beta are double precision scalars \n\n");
    
    m =  p =  n = 2560;
    int N = m;
    printf (" Initializing data for matrix multiplication C=A*B for matrix \n"
            " A(%ix%i) and matrix B(%ix%i)\n\n", m, p, p, n);
    alpha = 1.0; beta = 0.0;
    
    printf (" Allocating memory for matrices aligned on 64-byte boundary for better \n"
            " performance \n\n");
    A = (double *)mkl_malloc( m*p*sizeof( double ), 64 );
    B = (double *)mkl_malloc( p*n*sizeof( double ), 64 );
    C = (double *)mkl_malloc( m*n*sizeof( double ), 64 );
    if (A == NULL || B == NULL || C == NULL) {
        printf( "\n ERROR: Can't allocate memory for matrices. Aborting... \n\n");
        mkl_free(A);
        mkl_free(B);
        mkl_free(C);
        /*return 1;*/
    }

    printf (" Intializing matrix data \n\n");
    for (i = 0; i < (m*p); i++) {
        A[i] = (double)(i+1);
    }

    for (i = 0; i < (p*n); i++) {
        B[i] = (double)(-i-1);
    }

    for (i = 0; i < (m*n); i++) {
        C[i] = 0.0;
    }

    printf (" Finding max number of threads Intel(R) MKL can use for parallel runs \n\n");
    max_threads = mkl_get_max_threads();

    printf (" Running Intel(R) MKL from 1 to %i threads \n\n", max_threads);
    for (i = 1; i <= max_threads; i++) {
        for (j = 0; j < (m*n); j++)
            C[j] = 0.0;
        
        printf (" Requesting Intel(R) MKL to use %i thread(s) \n\n", i);
        mkl_set_num_threads(1);

        printf (" Making the first run of DAXPY\n"
                " via CBLAS interface to get stable run time measurements \n\n");
        cblas_daxpy(N*N, alpha, A, 1, B, 1);

        printf (" Measuring performance of DAXPY\n"
                " via CBLAS interface on %i thread(s) \n\n", i);
        s_initial = dsecnd();
        for (r = 0; r < LOOP_COUNT; r++) {
            cblas_daxpy(N*N, alpha, A, 1, B, 1);
        }
        s_elapsed = (dsecnd() - s_initial) / LOOP_COUNT;

        printf (" == DAXPY completed ==\n"
                " == at %.5f milliseconds using %d thread(s) FLOPS=%.3f ==\n\n", (s_elapsed * 1000), i, 2*(double)N*(double)N/s_elapsed*1e-9);
    }
    
    printf (" Deallocating memory \n\n");
    mkl_free(A);
    mkl_free(B);
    mkl_free(C);
    
    if (s_elapsed < 0.9/LOOP_COUNT) {
        s_elapsed=1.0/LOOP_COUNT/s_elapsed;
        i=(int)(s_elapsed*LOOP_COUNT)+1;
        printf(" It is highly recommended to define LOOP_COUNT for this example on your \n"
               " computer as %i to have total execution time about 1 second for reliability \n"
               " of measurements\n\n", i);
    }
}
    printf (" Example completed. \n\n");
    return 0;
 
}
