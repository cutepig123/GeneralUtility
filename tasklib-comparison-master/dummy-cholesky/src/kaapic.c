#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <cassert>
#include "kaapi++"
#include "common.h"

struct TaskDPOTRF: public ka::Task<1>::Signature<
    ka::RW<ka::range2d<double> > /*A*/
>{};
template<>
struct TaskBodyCPU<TaskDPOTRF> {
    void operator()( ka::range2d_rw<double> A ) { LOG_TIMER("potrf"); }
};

struct TaskDTRSM: public ka::Task<2>::Signature< 
    ka::R<ka::range2d<double> >, /* Akk */
    ka::RW<ka::range2d<double> > /* Akj */
>{};
template<>
struct TaskBodyCPU<TaskDTRSM> {
    void operator()( ka::range2d_r<double> Akk, ka::range2d_rw<double> Akj ) { LOG_TIMER("trsm"); }
};

struct TaskDSYRK: public ka::Task<2>::Signature<
    ka::R<ka::range2d<double> >, /* A */
    ka::RW<ka::range2d<double> > /* C */
>{};
template<>
struct TaskBodyCPU<TaskDSYRK> {
    void operator()( ka::range2d_r <double> A, ka::range2d_rw<double> C ) { LOG_TIMER("syrk"); }
};

struct TaskDGEMM: public ka::Task<3>::Signature<
    ka::R<ka::range2d<double> >, /* Aik   */
    ka::R<ka::range2d<double> >, /* Akj   */
    ka::RW<ka::range2d<double> > /* Aij   */
>{};
template<>
struct TaskBodyCPU<TaskDGEMM> {
    void operator()(ka::range2d_r<double> Aik,
                    ka::range2d_r<double> Akj,
                    ka::range2d_rw<double> Aij) { LOG_TIMER("gemm"); }
};

double *dA;

struct TaskCholesky
: public ka::Task<1>::Signature< 
    ka::RPWP<ka::range2d<double> > /*A*/
>{};
template<>
struct TaskBodyCPU<TaskCholesky> {
    void operator()( const ka::StaticSchedInfo* info, 
                     ka::range2d_rpwp<double> A )
    {
        const int N = A->dim(0);

        for (int k=0; k < N; ++k) {
            ka::rangeindex rk(k, k+1);
            ka::Spawn<TaskDPOTRF>()( A(rk,rk) );

            for (int m=k+1; m < N; ++m) {
                ka::rangeindex rm(m, m+1);
                ka::Spawn<TaskDTRSM>()( A(rk,rk), A(rm,rk) );
            }

            for (int m=k+1; m < N; ++m) {
                ka::rangeindex rm(m, m+1);
                ka::Spawn<TaskDSYRK>()( A(rm,rk), A(rm,rm));
                for (int n=k+1; n < m; ++n) {
                    ka::rangeindex rn(n, n+1);
                    ka::Spawn<TaskDGEMM>()( A(rm,rk), A(rn,rk), A(rm,rn));
                }
            }
        }
    }
};

LOG_TimeUnit LOG_start, LOG_mid, LOG_stop;

void compute(LOG_TimeUnit *LOG_start,
             LOG_TimeUnit *LOG_mid,
             LOG_TimeUnit *LOG_stop, int DIM) {

    ka::array<2,double> A(dA, DIM, DIM, DIM);

    *LOG_start = LOG_getTimeStart();

    ka::Spawn<TaskCholesky>()(A);

    *LOG_mid = LOG_getTimeStop();

    ka::Sync();

    *LOG_stop = LOG_getTimeStop();
}

struct main_task {
    void operator()(int argc, char** argv ) {
        int n = 5;
        if (argc > 1)
            n = atoi(argv[1]);

        if (argc > 2)
            delay = atoi(argv[2]);

        assert( posix_memalign((void**)&dA, 4096, n*n*sizeof(double)) == 0);

        TIMING_init();

        for (int i = 0; i < NUM_ITERATIONS; ++i) {
            LOG_TimeUnit LOG_start, LOG_mid, LOG_stop;
            compute(&LOG_start, &LOG_mid, &LOG_stop, n);
            TIMING_add(LOG_start, LOG_mid, LOG_stop);
            LOG_optional_break
        }

        TIMING_end(n);

        free(dA);
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
