#ifndef _GNU_SOURCE
#define _GNU_SOURCE         /* See feature_test_macros(7) */
#endif
#include <unistd.h>
#include <sys/syscall.h>   /* For SYS_xxx definitions */
#include <sys/types.h>

//int syscall(int number, ...);

#include <pthread.h>
#include <string.h>
#include <sys/ioctl.h>
//extern "C" {
#include <sys/syscall.h>
//}

#include <stdio.h>

//////////// logging

typedef unsigned long long LOG_TimeUnit;
static inline LOG_TimeUnit LOG_getTimeStart() {
  unsigned int a, d;

  __asm__ __volatile__ ("cpuid\n"
                        "rdtsc\n"
         : "=a" (a), "=d" (d)
         :: "%rbx", "%rcx");
  return ( (unsigned long long) a) | ( ((unsigned long long) d) << 32 );
}

static inline LOG_TimeUnit LOG_rdtsc() {
  unsigned int a, d;
  __asm__ __volatile__ ("rdtsc" : "=a" (a), "=d" (d));
  return ( (unsigned long long) a) | ( ((unsigned long long) d) << 32 );
}

static inline LOG_TimeUnit LOG_getTimeStop() {
  unsigned d, a;

  __asm__ __volatile__ ("rdtscp\n"
                        "mov %%edx, %0\n\t"
                        "mov %%eax, %1\n\t"
                        "cpuid\n"
          : "=r" (d), "=r" (a)
          :: "%rax", "%rbx", "%rcx", "%rdx");
  return ( (unsigned long long) a) | ( ((unsigned long long) d) << 32 );
}

#ifdef LOGGING

#define LOG_MAX_ENTRIES 2000000

static inline unsigned long LOG_getThread() {
  return pthread_self();
}

typedef struct {
  LOG_TimeUnit start, length;
  unsigned long thread;
  char text[64 - 2*sizeof(LOG_TimeUnit)- sizeof(unsigned long)];
} LOG_entry;

LOG_entry LOG_data[LOG_MAX_ENTRIES];
size_t LOG_ptr;

static inline void LOG_add1(const char *text) {
  LOG_TimeUnit start = LOG_getTimeStart();
  LOG_TimeUnit stop = start;
  size_t i = __sync_fetch_and_add(&LOG_ptr, 1);
  strcpy(LOG_data[i].text, text);
  LOG_data[i].start = start;
  LOG_data[i].length = stop-start;
  LOG_data[i].thread = LOG_getThread();
}

static inline void LOG_add(const char *text, LOG_TimeUnit start, LOG_TimeUnit stop) {
  size_t i = __sync_fetch_and_add(&LOG_ptr, 1);
  strcpy(LOG_data[i].text, text);
  LOG_data[i].start = start;
  LOG_data[i].length = stop-start;
  LOG_data[i].thread = LOG_getThread();
}

void LOG_dump(const char *filename) {
  size_t i;
  FILE *out = fopen(filename, "w");
  fprintf(out, "LOG 2\n");
  LOG_TimeUnit minimum = LOG_data[0].start;
  for (i = 0; i < LOG_ptr; ++i) {
      if (LOG_data[i].start < minimum)
          minimum = LOG_data[i].start;
  }
  for (i = 0; i < LOG_ptr; ++i) {
      fprintf(out, "%lu: %llu %llu %s\n",
              LOG_data[i].thread,
              LOG_data[i].start-minimum,
              LOG_data[i].length,
              LOG_data[i].text);
  }
  fclose(out);
}

#define LOG_optional_break break;

#else
#define LOG_add1(a)
#define LOG_add(a,b,c)
#define LOG_dump(a)
#define LOG_optional_break
#endif

//////////// logging

char *TIMING_submit;
char *TIMING_execute;
char *TIMING_total;
int TIMING_submit_i;
int TIMING_execute_i;
int TIMING_total_i;
LOG_TimeUnit delay = 1000;

#define LOG_TIMER(LOG_NAME) \
    LOG_TimeUnit start = LOG_getTimeStart(); \
    LOG_TimeUnit stop = start + delay; \
    LOG_TimeUnit curr; \
    while ( (curr = LOG_rdtsc()) < stop); \
    LOG_add(LOG_NAME, start, curr)

void TIMING_init() {
    TIMING_submit = (char *) malloc(20*100);
    TIMING_execute = (char *) malloc(20*100);
    TIMING_total = (char *) malloc(20*100);
}

void TIMING_add(unsigned long long LOG_start, unsigned long long LOG_mid, unsigned long long LOG_stop) {
    TIMING_submit_i += sprintf(&TIMING_submit[TIMING_submit_i], "%llu ", LOG_mid-LOG_start);
    TIMING_execute_i += sprintf(&TIMING_execute[TIMING_execute_i], "%llu ", LOG_stop-LOG_mid);
    TIMING_total_i += sprintf(&TIMING_total[TIMING_total_i], "%llu ", LOG_stop-LOG_start);
}

void TIMING_end(int n) {
    printf("n= %d delay= %llu submit=[ %s] execute=[ %s] total=[ %s]\n",
        n, delay, TIMING_submit, TIMING_execute, TIMING_total);
    free(TIMING_submit);
    free(TIMING_execute);
    free(TIMING_total);
}

#define NUM_ITERATIONS 20

