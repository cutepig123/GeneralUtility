#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include "wf_interface.h"
#include "common.h"

typedef obj::inoutdep<double[1]> binout;
typedef obj::indep<double[1]> bin;
typedef obj::outdep<double[1]> bout;

void spotrf(binout ADep) { LOG_TIMER("potrf"); }
void strsm(bin Adep, binout Bdep) { LOG_TIMER("trsm"); }
void ssyrk(bin Adep, binout Cdep) { LOG_TIMER("syrk"); }
void sgemm(bin Adep, bin Bdep, binout Cdep) { LOG_TIMER("gemm"); }

obj::object_t<double[1]> *A;

void compute(LOG_TimeUnit *LOG_start,
             LOG_TimeUnit *LOG_mid,
             LOG_TimeUnit *LOG_stop, int DIM) {

    *LOG_start = LOG_getTimeStart();

    for (int k = 0; k < DIM; ++k) {

        spawn(spotrf, (binout) A[k*DIM+k]);

        for (int m = k+1; m < DIM; ++m) {
            spawn(strsm, (bin) A[k*DIM+k], (binout) A[m*DIM+k]);
        }

        for (int m = k+1; m < DIM; ++m) {
            spawn(ssyrk, (bin) A[m*DIM+k], (binout) A[m*DIM+m]);

            for (int n = k+1; n < m; ++n) {
                spawn(sgemm, (bin) A[m*DIM+k], (bin) A[n*DIM+k], (binout) A[m*DIM+n]);
            }
        }
    }

    *LOG_mid = LOG_getTimeStop();

    ssync();

    *LOG_stop = LOG_getTimeStop();
}

int main(int argc, char *argv[]) {
    int k;
    int n = 5;

    if (argc > 1)
        n = atoi(argv[1]);
    if (argc > 2)
        delay = atoi(argv[2]);

    A = new obj::object_t<double[1]>[n*n];
    for (long i = 0; i < n*n; i++)
        A[i] = obj::object_t<double[1]>(1); // resize

    TIMING_init();

    for (k = 0; k < NUM_ITERATIONS; ++k) {
        LOG_TimeUnit LOG_start, LOG_mid, LOG_stop;
        run ( compute, &LOG_start, &LOG_mid, &LOG_stop, n );
        TIMING_add(LOG_start, LOG_mid, LOG_stop);
        LOG_optional_break
    }

    TIMING_end(n);

    LOG_dump("swan.log");

    delete [] A;
    return 0;
}
