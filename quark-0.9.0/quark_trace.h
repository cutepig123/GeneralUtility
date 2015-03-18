/**
 * @file quark_trace.h
 *
 * QUARK (QUeuing And Runtime for Kernels) provides a runtime
 * enviornment for the dynamic execution of precedence-constrained
 * tasks.
 *
 * QUARK is a software package provided by Univ. of Tennessee,
 * Univ. of California Berkeley and Univ. of Colorado Denver.
 *
 * @version 2.4.5
 * @author Mathieu Faverge
 * @date 2010-11-15
 *
 */
#ifndef _QUARK_TRACE_H_
#define _QUARK_TRACE_H_

#ifdef TRACE_QUARK

#include <eztrace.h>
#include <ev_codes.h>
#include "../core_blas-eztrace/coreblas_ev_codes.h"

#define quark_trace_addtask() EZTRACE_EVENT1(FUT_COREBLAS_TASK,  1)
#define quark_trace_deltask() EZTRACE_EVENT1(FUT_COREBLAS_TASK, -1)
#define quark_trace_addtask2worker(__tid) EZTRACE_EVENT2(FUT_COREBLAS_TASKW, __tid,  1)
#define quark_trace_deltask2worker(__tid) EZTRACE_EVENT2(FUT_COREBLAS_TASKW, __tid, -1)

#else

#define quark_trace_addtask()
#define quark_trace_deltask()
#define quark_trace_addtask2worker(__tid)
#define quark_trace_deltask2worker(__tid)

#endif

#endif /* _QUARK_TRACE_H_ */
