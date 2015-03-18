#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <cassert>
#include "kaapi++"
#include "common.h"

struct MyTask : public ka::Task<1>::Signature<int> {};

template<>
struct TaskBodyCPU<MyTask> {
    void operator()(int) { LOG_TIMER("task"); }
};

LOG_TimeUnit LOG_start, LOG_mid, LOG_stop;

void test(int n) {

    LOG_start = LOG_getTimeStart();

    for (int i = 0; i < n; ++i)
        ka::Spawn<MyTask>()(i);

    LOG_mid = LOG_getTimeStop();

    ka::Sync();

    LOG_stop = LOG_getTimeStop();
}

struct main_task {
    void operator()(int argc, char** argv ) {
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
    }
};

int main(int argc, char** argv) {

    try {
        ka::Community com = ka::System::join_community( argc, argv );
        ka::SpawnMain<main_task>()(argc, argv); 
        com.leave();
        ka::System::terminate();
    }
    catch (const std::exception& E) {
        ka::logfile() << "Catch : " << E.what() << std::endl;
    }
    catch (...) {
        ka::logfile() << "Catch unknown exception: " << std::endl;
    }

    LOG_dump("xkaapi.log");
    return 0;
}
