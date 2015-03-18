#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <starpu.h>
#include "common.h"

void potrf(void *buffers[], void *cl_arg) { LOG_TIMER("potrf"); }
struct starpu_codelet potrf_cl = {
    .where = STARPU_CPU,
    .cpu_funcs = { potrf, NULL },
    .nbuffers = 1,
    .modes = { STARPU_RW }
};

void trsm(void *buffers[], void *cl_arg) { LOG_TIMER("trsm"); }
struct starpu_codelet trsm_cl = {
    .where = STARPU_CPU,
    .cpu_funcs = { trsm, NULL },
    .nbuffers = 2,
    .modes = { STARPU_R, STARPU_RW }
};

void syrk(void *buffers[], void *cl_arg) { LOG_TIMER("syrk"); }
struct starpu_codelet syrk_cl = {
    .where = STARPU_CPU,
    .cpu_funcs = { syrk, NULL },
    .nbuffers = 2,
    .modes = { STARPU_R, STARPU_RW }
};

void gemm(void *buffers[], void *cl_arg) { LOG_TIMER("gemm"); }
struct starpu_codelet gemm_cl = {
    .where = STARPU_CPU,
    .cpu_funcs = { gemm, NULL },
    .nbuffers = 3,
    .modes = { STARPU_R, STARPU_R, STARPU_RW }
};

double *A;
starpu_data_handle_t *h;

void compute(LOG_TimeUnit *LOG_start,
             LOG_TimeUnit *LOG_mid,
             LOG_TimeUnit *LOG_stop, int DIM) {
    int k, m, n;

    *LOG_start = LOG_getTimeStart();

    for (k = 0; k < DIM; ++k) {
        starpu_insert_task(&potrf_cl,
                           STARPU_RW, h[k*DIM+k],
                           0);

        for (m = k+1; m < DIM; ++m) {
            starpu_insert_task(&trsm_cl,
                               STARPU_R, h[k*DIM+k],
                               STARPU_RW, h[m*DIM+k],
                               0);
        }

        for (m=k+1; m < DIM; ++m) {
            starpu_insert_task(&syrk_cl,
                               STARPU_R, h[m*DIM+k],
                               STARPU_RW, h[m*DIM+m],
                               0);

            for (n=k+1; n < m; ++n) {
                starpu_insert_task(&gemm_cl,
                                   STARPU_R, h[m*DIM+k],
                                   STARPU_R, h[n*DIM+k],
                                   STARPU_RW, h[m*DIM+n],
                                   0);
            }
        }
    }

    *LOG_mid = LOG_getTimeStop();

    starpu_task_wait_for_all();

    *LOG_stop = LOG_getTimeStop();
}

int main(int argc, char *argv[]) {
    int i, j, k;
    int n = 5;

    if (argc > 1)
        n = atoi(argv[1]);
    if (argc > 2)
        delay = atoi(argv[2]);

    A = (double*) malloc(n*n*sizeof(double));
    h = (starpu_data_handle_t*) malloc(n*n*sizeof(starpu_data_handle_t));

    TIMING_init();

    assert(starpu_init(NULL) == 0);
    for (i = 0; i < n; ++i)
        for (j = 0; j < n; ++j)
            starpu_void_data_register(&h[i*n+j]);
    for (k = 0; k < NUM_ITERATIONS; ++k) {
        LOG_TimeUnit LOG_start, LOG_mid, LOG_stop;
        compute(&LOG_start, &LOG_mid, &LOG_stop, n);
        TIMING_add(LOG_start, LOG_mid, LOG_stop);
        LOG_optional_break
    }
    starpu_shutdown();

    TIMING_end(n);

    LOG_dump("starpu.log");
    free(h);
    free(A);
    return 0;
}
