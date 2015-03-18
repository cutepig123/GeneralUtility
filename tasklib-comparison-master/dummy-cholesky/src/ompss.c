#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include "common.h"

#pragma omp task inout([1]A)
void potrf(double *A) { LOG_TIMER("potrf"); }

#pragma omp task in([1]T) inout([1]B)
void trsm(double *T, double *B) { LOG_TIMER("trsm"); }

#pragma omp task in([1]A) inout([1]C)
void syrk(double *A, double *C) { LOG_TIMER("syrk"); }

#pragma omp task in([1]A, [1]B) inout([1]C)
void gemm(double *A, double *B, double *C) { LOG_TIMER("gemm"); }

double *A;

void compute(LOG_TimeUnit *LOG_start,
             LOG_TimeUnit *LOG_mid,
             LOG_TimeUnit *LOG_stop, int DIM) {

    *LOG_start = LOG_getTimeStart();

    for (int k = 0; k < DIM; ++k) {
        potrf( &A[k*DIM + k] );

        for (int m = k+1; m < DIM; ++m) {
            trsm( &A[k*DIM + k], &A[m*DIM + k] );
        }

        for (int m=k+1; m < DIM; ++m) {
            syrk( &A[m*DIM + k], &A[m*DIM + m] );

            for (int n=k+1; n < m; ++n) {
                gemm( &A[m*DIM + k], &A[n*DIM + k], &A[m*DIM + n] );
            }
        }
    }

    *LOG_mid = LOG_getTimeStop();

#pragma omp taskwait

    *LOG_stop = LOG_getTimeStop();
}

int main(int argc, char *argv[]) {

    int n = 5;

    if (argc > 1)
        n = atoi(argv[1]);
    if (argc > 2)
        delay = atoi(argv[2]);

    A = (double*) malloc(n*n*sizeof(double));

    TIMING_init();

    for (int i = 0; i < NUM_ITERATIONS; ++i) {
        LOG_TimeUnit LOG_start, LOG_mid, LOG_stop;
        compute(&LOG_start, &LOG_mid, &LOG_stop, n);
        TIMING_add(LOG_start, LOG_mid, LOG_stop);
        LOG_optional_break
    }
    TIMING_end(n);

    LOG_dump("ompss.log");

    free(A);
    return 0;
}
