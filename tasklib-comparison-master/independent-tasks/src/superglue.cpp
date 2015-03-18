#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include "sg/superglue.hpp"
#include "common.h"

struct Options : public DefaultOptions<Options> {};

struct mytask : public Task<Options, 0> {
    void run() { LOG_TIMER("task"); }
};

SuperGlue<Options> tm;

LOG_TimeUnit LOG_start, LOG_mid, LOG_stop;

void test(int n) {

    LOG_start = LOG_getTimeStart();

    for (int i = 0; i < n; ++i)
        tm.submit(new mytask());

    LOG_mid = LOG_getTimeStop();

    tm.barrier();

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
    LOG_dump("superglue.log");

    return 0;
}
