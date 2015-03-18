#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include "quark.h"
#include "common.h"

void mytask(Quark *quark) { LOG_TIMER("task"); }

LOG_TimeUnit LOG_start, LOG_mid, LOG_stop;

void test(int n) {
    int i;
    Quark *quark = QUARK_New(0);

    LOG_start = LOG_getTimeStart();

    for (i = 0; i < n; ++i)
        QUARK_Insert_Task(quark, mytask, NULL, 0);

    LOG_mid = LOG_getTimeStop();

    QUARK_Waitall(quark);

    LOG_stop = LOG_getTimeStop();

    QUARK_Delete(quark);
}

int main(int argc, char *argv[]) {
    int i;
    int n = 1000000;

    if (argc > 1)
        n = atoi(argv[1]);
    if (argc > 2)
        delay = atoi(argv[2]);

    TIMING_init();
    for (i = 0; i < NUM_ITERATIONS; ++i) {
        test(n);
        TIMING_add(LOG_start, LOG_mid, LOG_stop);
        LOG_optional_break
    }
    TIMING_end(n);

    LOG_dump("quark.log");

    return 0;
}
