#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include "sg/superglue.hpp"
#include "common.h"

struct Options : public DefaultOptions<Options> {};

struct potrf : public Task<Options, 1> {
    potrf(double *A, Handle<Options> &h) {
        register_access(ReadWriteAdd::write, h);
    }
    void run() { LOG_TIMER("potrf"); }
};

struct gemm : public Task<Options, 3> {
    gemm(double *A, double *B, double *C,
         Handle<Options> &hA, Handle<Options> &hB, Handle<Options> &hC) {
        register_access(ReadWriteAdd::read, hA);
        register_access(ReadWriteAdd::read, hB);
        register_access(ReadWriteAdd::write, hC);
    }
    void run() { LOG_TIMER("gemm"); }
};

struct trsm : public Task<Options, 2> {
    trsm(double *T, double *B,
         Handle<Options> &hT, Handle<Options> &hB) {
        register_access(ReadWriteAdd::read, hT);
        register_access(ReadWriteAdd::write, hB);
    }
    void run() { LOG_TIMER("trsm"); }
};

struct syrk : public Task<Options, 2> {
    syrk(double *A, double *C,
         Handle<Options> &hA, Handle<Options> &hC) {
        register_access(ReadWriteAdd::read, hA);
        register_access(ReadWriteAdd::write, hC);
    }
    void run() { LOG_TIMER("syrk"); }
};

SuperGlue<Options> tm;
double *A;
Handle<Options> *h;

void compute(LOG_TimeUnit *LOG_start,
             LOG_TimeUnit *LOG_mid,
             LOG_TimeUnit *LOG_stop, int DIM) {

    *LOG_start = LOG_getTimeStart();

    for (int k = 0; k < DIM; ++k) {
        tm.submit(new potrf(&A[k*DIM+k], h[k*DIM+k]));

        for (int m = k+1; m < DIM; ++m) {
            tm.submit(new trsm(&A[k*DIM+k], &A[m*DIM+k],
                               h[k*DIM+k], h[m*DIM+k]));
        }

        for (int m=k+1; m < DIM; ++m) {
            tm.submit(new syrk(&A[m*DIM+k], &A[m*DIM+m],
                                h[m*DIM+k], h[m*DIM+m]));

            for (int n=k+1; n < m; ++n) {
                tm.submit(new gemm(&A[m*DIM+k], &A[n*DIM+k], &A[m*DIM+n],
                                   h[m*DIM+k], h[n*DIM+k], h[m*DIM+n]) );
            }
        }
    }

    *LOG_mid = LOG_getTimeStop();

    tm.barrier();

    *LOG_stop = LOG_getTimeStop();
}

int main(int argc, char *argv[]) {

    int n = 5;

    if (argc > 1)
        n = atoi(argv[1]);
    if (argc > 2)
        delay = atoi(argv[2]);

    A = (double*) malloc(n*n*sizeof(double));
    h = new Handle<Options>[n*n];

    TIMING_init();

    for (int i = 0; i < NUM_ITERATIONS; ++i) {
        LOG_TimeUnit LOG_start, LOG_mid, LOG_stop;
        compute(&LOG_start, &LOG_mid, &LOG_stop, n);
        TIMING_add(LOG_start, LOG_mid, LOG_stop);
        LOG_optional_break
    }

    TIMING_end(n);

    LOG_dump("superglue.log");
    free(A);
    return 0;
}
