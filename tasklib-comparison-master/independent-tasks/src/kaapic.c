#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include "kaapic.h"
#include "common.h"

void body() {
    LOG_TIMER("task");
}

LOG_TimeUnit LOG_start, LOG_mid, LOG_stop;

struct kaapic_spawn_attr_t attr;

void test(int n) {
    int i;

    LOG_start = LOG_getTimeStart();

    for (i = 0; i < n; ++i) {
        kaapic_spawn(&attr, 
            0,     /* number of arguments */
            body   /* the entry point for the task */
            //KAAPIC_MODE_V, KAAPIC_TYPE_INT, 1, (int)129,
            //KAAPIC_MODE_W, KAAPIC_TYPE_INT, 1, &result,
            //KAAPIC_MODE_S, KAAPIC_TYPE_INT, 10, 0
        );
    }

    LOG_mid = LOG_getTimeStop();

    kaapic_sync();

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
      
    kaapic_init(KAAPIC_START_ONLY_MAIN);


    /* initialize default attribut */
    kaapic_spawn_attr_init(&attr);

    for (i = 0; i < NUM_ITERATIONS; ++i) {
        kaapic_begin_parallel (KAAPIC_FLAG_DEFAULT);
        test(n);
        kaapic_end_parallel (KAAPIC_FLAG_DEFAULT);
        TIMING_add(LOG_start, LOG_mid, LOG_stop);
        LOG_optional_break
    }

    kaapic_finalize();

    TIMING_end(n);
    LOG_dump("kaapic.log");
    return 0;
}
