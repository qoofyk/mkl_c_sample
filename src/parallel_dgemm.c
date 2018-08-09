/* Copyright 2018 Los Alamos National Laboratory
 * Copyright 2009-2018 Purdue University and Purdue University Research Foundation
 *                      
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

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
#include <string.h>
#include "eval_tools.h"

/* Consider adjusting LOOP_COUNT based on the performance of your computer */
/* to make sure that total run time is at least 1 second */
#define LOOP_COUNT 50  

int main(int argc, char** argv)
{
    double time[128]; 
    int N;
    int threads = atoi(getenv("OMP_NUM_THREADS"));
    printf ("\n This example demonstrates threading impact on computing real matrix product \n"
            " C=alpha*A*B+beta*C using Intel(R) MKL function dgemm, where A, B, and C are \n"
            " matrices and alpha and beta are double precision scalars \n\n");
 
#pragma omp parallel 
{    
    int myid =  omp_get_thread_num();
    printf(" I am thread%d\n", myid); 

    double *A, *B, *C;
    int m, n, p, i, j, r, max_threads, loop_cnt;
    double alpha, beta;
    double s_initial, s_elapsed;

    loop_cnt = LOOP_COUNT;
   
    m =  p =  n = 2560;

    //parse command line
    for (i = 1; i < argc; i++) {
      if (!strcmp(argv[i], "-size")) {
            m =  p =  n = atoi(argv[++i]);
      }
      if (!strcmp(argv[i], "-iter")) {
            loop_cnt = atoi(argv[++i]);
      }
    }    

    N = m;

    if (myid == 0)
    printf (" N=%d, loop_cnt=%d\n", N, loop_cnt);

    if (myid == 0)
    printf (" Initializing data for matrix multiplication C=A*B for matrix \n"
            " A(%ix%i) and matrix B(%ix%i)\n\n", m, p, p, n);
    alpha = 1.0; beta = 0.0;
    
    if (myid == 0)
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

    if (myid == 0)
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

    if (myid == 0)
    printf (" Finding max number of threads Intel(R) MKL can use for parallel runs \n\n");
    max_threads = mkl_get_max_threads();

    if (myid == 0)
    printf (" Running Intel(R) MKL from 1 to %i threads \n\n", max_threads);
    for (i = 1; i <= max_threads; i++) {
        for (j = 0; j < (m*n); j++)
            C[j] = 0.0;
        
        if (myid == 0)
        printf (" Requesting Intel(R) MKL to use %i thread(s) \n\n", i);
        mkl_set_num_threads(1);

        if (myid == 0)
        printf (" Making the first run of matrix product using Intel(R) MKL dgemm function \n"
                " via CBLAS interface to get stable run time measurements \n\n");
        cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, 
                    m, n, p, alpha, A, p, B, n, beta, C, n);
        
        if (myid == 0)
        printf (" Measuring performance of matrix product using Intel(R) MKL dgemm function \n"
                " via CBLAS interface on %i thread(s) \n\n", i);
        s_initial = get_cur_time();
        for (r = 0; r < loop_cnt; r++) {
            cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, 
                        m, n, p, alpha, A, p, B, n, beta, C, n);
        }
        s_elapsed = (get_cur_time() - s_initial) / loop_cnt;
        time[myid] = s_elapsed * 1000;

        printf (" == Matrix multiplication using Intel(R) MKL dgemm completed ==\n"
                " == at %.5f milliseconds, %.2f seconds, using %d thread(s), GFLOPS=%.3f ==\n\n", 
                (s_elapsed * 1000), s_elapsed*loop_cnt, i, 2*(double)N*(double)N*(double)N/s_elapsed*1e-9);
    }
    
    if (myid == 0)
    printf (" Deallocating memory \n\n");
    mkl_free(A);
    mkl_free(B);
    mkl_free(C);
    
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
    printf(" AE= %.3f ms, Each thread uses %.3f MB\n", 
            average, (double)N*(double)N*sizeof(double)/(1024*1024));


    printf (" Example completed. \n\n");
    return 0;
 
}
