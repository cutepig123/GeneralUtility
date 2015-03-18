#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <starpu.h>
#include <assert.h>
#include "common.h"

void mytask(void *buffers[], void *cl_arg) { LOG_TIMER("task"); }

struct starpu_codelet mytask_cl = {
    .where = STARPU_CPU,
    .cpu_funcs = { mytask, NULL },
    .nbuffers = 0,
//    .modes = { STARPU_R, STARPU_R, STARPU_RW }
};

LOG_TimeUnit LOG_start, LOG_mid, LOG_stop;

void test(int n) {
    int i;
    LOG_start = LOG_getTimeStart();
    
    for (i = 0; i < n; ++i)
        starpu_insert_task(&mytask_cl, 0);

    LOG_mid = LOG_getTimeStop();

    starpu_task_wait_for_all();

    LOG_stop = LOG_getTimeStop();
}

int main(int argc, char *argv[]) {
    int i;
    int n = 1000000;

    if (argc > 1)
        n = atoi(argv[1]);
    if (argc > 2)
        delay = atoi(argv[2]);

    TIMING_init();

    assert( starpu_init(NULL) == 0);
    for (i = 0; i < NUM_ITERATIONS; ++i) {
        test(n);
        TIMING_add(LOG_start, LOG_mid, LOG_stop);
        LOG_optional_break
    }
    starpu_shutdown();

    TIMING_end(n);
    LOG_dump(LOGFILENAME);

    return 0;
}
