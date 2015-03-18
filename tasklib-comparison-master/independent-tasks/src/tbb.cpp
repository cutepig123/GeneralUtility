#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "tbb/task.h"
#include "tbb/task_group.h"
#include "tbb/task_scheduler_init.h"
#include "common.h"

struct mytask { //: public tbb::task {
    //tbb::task *execute() { LOG_TIMER("task"); return NULL; }
    void operator()() { LOG_TIMER("task"); }
};

LOG_TimeUnit LOG_start, LOG_mid, LOG_stop;

int g_num_threads = -1;

void test(int n) {
    tbb::task_scheduler_init init(g_num_threads);  // Automatic number of threads

    tbb::task_group group;

    LOG_start = LOG_getTimeStart();

    for (int i = 0; i < n; ++i)
        group.run( mytask() );

    LOG_mid = LOG_getTimeStop();

    group.wait();

    LOG_stop = LOG_getTimeStop();
}

int main(int argc, char *argv[]) {

    int n = 1000000;

    if (argc > 1)
        n = atoi(argv[1]);
    if (argc > 2)
        delay = atoi(argv[2]);

    char *tbb_num_threads = getenv("TBB_NUM_THREADS");
    if (tbb_num_threads != 0) {
        g_num_threads = atoi(tbb_num_threads);
    }

    TIMING_init();

    for (int i = 0; i < NUM_ITERATIONS; ++i) {
        test(n);
        TIMING_add(LOG_start, LOG_mid, LOG_stop);
        LOG_optional_break
    }

    TIMING_end(n);
    LOG_dump("tbb.log");

    return 0;
}
