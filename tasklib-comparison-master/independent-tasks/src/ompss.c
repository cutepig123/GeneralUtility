#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include "common.h"

#pragma omp task
void mytask() { LOG_TIMER("task"); }

LOG_TimeUnit LOG_start, LOG_mid, LOG_stop;

void test(int n) {

    LOG_start = LOG_getTimeStart();
    
    for (int i = 0; i < n; ++i)
        mytask();

    LOG_mid = LOG_getTimeStop();

#pragma omp taskwait

    LOG_stop = LOG_getTimeStop();
}

int main(int argc, char *argv[]) {

    int n = 1000000;

    if (argc > 1)
        n = atoi(argv[1]);
    if (argc > 2)
        delay = atoi(argv[2]);

    TIMING_init();
    for (int i = 0; i < NUM_ITERATIONS; ++i) {
        test(n);
        TIMING_add(LOG_start, LOG_mid, LOG_stop);
        LOG_optional_break
    }
    TIMING_end(n);

    LOG_dump("ompss.log");

    return 0;
}
