#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include "quark.h"
#include "common.h"

void potrf(Quark *quark) { LOG_TIMER("potrf"); }
void trsm(Quark *quark) { LOG_TIMER("trsm"); }
void syrk(Quark *quark) { LOG_TIMER("syrk"); }
void gemm(Quark *quark) { LOG_TIMER("gemm"); }

double *A;

void compute(LOG_TimeUnit *LOG_start,
             LOG_TimeUnit *LOG_mid,
             LOG_TimeUnit *LOG_stop, int DIM) {
    int k, m, n;
    Quark *quark = QUARK_New(0);

    *LOG_start = LOG_getTimeStart();

    for (k = 0; k < DIM; ++k) {
        QUARK_Insert_Task(quark, potrf, NULL, 
                          1*sizeof(double), &A[k*DIM+k], INOUT,
                          0);

        for (m = k+1; m < DIM; ++m) {
            QUARK_Insert_Task(quark, trsm, NULL, 
                              1*sizeof(double), &A[k*DIM+k], INPUT,
                              1*sizeof(double), &A[m*DIM+k], INOUT,
                              0);
        }

        for (m = k+1; m < DIM; ++m) {
            QUARK_Insert_Task(quark, syrk, NULL, 
                              1*sizeof(double), &A[m*DIM+k], INPUT,
                              1*sizeof(double), &A[m*DIM+m], INOUT,
                              0);

            for (n = k+1; n < m; ++n) {
                QUARK_Insert_Task(quark, gemm, NULL, 
                                  1*sizeof(double), &A[m*DIM+k], INPUT,
                                  1*sizeof(double), &A[n*DIM+k], INPUT,
                                  1*sizeof(double), &A[m*DIM+n], INOUT,
                                  0);
            }
        }
    }

    *LOG_mid = LOG_getTimeStop();

    QUARK_Waitall(quark);

    *LOG_stop = LOG_getTimeStop();

    QUARK_Delete(quark);
}

int main(int argc, char *argv[]) {

    int i;
    int n = 5;

    if (argc > 1)
        n = atoi(argv[1]);
    if (argc > 2)
        delay = atoi(argv[2]);

    A = (double*) malloc(n*n*sizeof(double));

    TIMING_init();

    for (i = 0; i < NUM_ITERATIONS; ++i) {
        LOG_TimeUnit LOG_start, LOG_mid, LOG_stop;
        compute(&LOG_start, &LOG_mid, &LOG_stop, n);
        TIMING_add(LOG_start, LOG_mid, LOG_stop);
        LOG_optional_break
    }

    TIMING_end(n);

    LOG_dump("quark.log");
    free(A);
    return 0;
}
