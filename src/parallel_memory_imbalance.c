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
*   This example demonstrates threading impact on memory kernel 
*   C=alpha*A+beta*B, where A, B, and C 
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
#define LOOP_COUNT 100 

int main(int argc, char** argv)
{
    double time[128]; 
    int threads = atoi(getenv("OMP_NUM_THREADS"));

    printf ("\n This example demonstrates threading impact on memory kernel\n"
            " C=alpha*A+beta*B where A, B, and C are \n"
            " matrices and alpha and beta are double precision scalars \n\n");
 
#pragma omp parallel default(shared)
{    
    int myid = omp_get_thread_num();
    printf("I am thread%02d\n", myid); 

    double *A, *B, *C;
    int m, n, p, i, j, k, r, max_threads, loop_cnt;
    double alpha, beta;
    double sum;
    double s_initial, s_elapsed;
   
    m =  p =  n = 2560;
    int step = 1024;
    loop_cnt = LOOP_COUNT;

    //parse command line
    for (i = 1; i < argc; i++) {
      if (!strcmp(argv[i], "-size")) {
            m =  p =  n = atoi(argv[++i]);
      }
      if (!strcmp(argv[i], "-cnt")) {
            loop_cnt = atoi(argv[++i]);
      }
      if (!strcmp(argv[i], "-step")) {
            step = atoi(argv[++i]);
      }
    }    

    int N = m;

    printf (" N=%d, loop_cnt=%d, step=%d\n", N, loop_cnt, step);

    if(myid==1)
        printf (" Initializing data for matrix multiplication C=A*B for matrix \n"
                " A(%ix%i) and matrix B(%ix%i)\n\n", m, p, p, n);
    alpha = 1.0; beta = 2.0;
    
    if(myid==1)
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
    }

    if(myid==1)
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

    if(myid==1)
        printf (" Finding max number of threads Intel(R) MKL can use for parallel runs \n\n");
    max_threads = mkl_get_max_threads();

    if(myid==1)
        printf (" Running Intel(R) MKL from 1 to %i threads \n\n", max_threads);
    for (int num_threads = 1; num_threads <= max_threads; num_threads++) {
        
        for (j = 0; j < (m*n); j++)
            C[j] = 0.0;
        
        /*printf (" Requesting Intel(R) MKL to use %i thread(s) \n\n", i);*/
        /*mkl_set_num_threads(1);*/

        if(myid==1)
            printf (" Making the first run of memory bound kernel\n"
                " to get stable run time measurements \n\n");
        for (i = 0; i < step; i++) {
           for (j = 0; j < (N*N/step); j++) {
               k = (i+j*step) % (N*N);
               /*printf("i=%d, j=%d, k=%d\n", i, j, k);*/
               C[k] = alpha * A[k] + beta * B[k]; 
           }    
        }

        if(myid==1)
            printf (" Measuring performance of memory bound kernel\n\n");
        
        s_initial = get_cur_time();
        for (r = 0; r < loop_cnt; r++) {
            for (i = 0; i < step; i++) {
                for (j = 0; j < (N*N/step); j++) {
                   k = (i+j*step) % (N*N);
                   C[k] = alpha * A[k] + beta * B[k]; 
                }
            }   
        }

        s_elapsed = (get_cur_time() - s_initial) / loop_cnt;
        time[myid] = s_elapsed * 1000;

        printf (" == Memory bound kernel completed ==\n"
                " == at %.5f milliseconds, T=%.3f, using %d thread(s) FLOPS=%.3f ==\n\n", 
                (s_elapsed * 1000), s_elapsed*loop_cnt, num_threads, 4*(double)N / s_elapsed);
    }
    
    if(myid==1)
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
    printf(" AE= %.3f ms\n", average);

    printf (" Example completed. \n\n");
    return 0;
 
}
