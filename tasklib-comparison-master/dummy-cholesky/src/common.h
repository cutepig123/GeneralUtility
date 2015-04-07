#ifndef _GNU_SOURCE
#define _GNU_SOURCE         /* See feature_test_macros(7) */
#endif
//#include <unistd.h>
//#include <sys/syscall.h>   /* For SYS_xxx definitions */
#include <sys/types.h>

//int syscall(int number, ...);

//#include <pthread.h>
#include <string.h>
//#include <sys/ioctl.h>
//extern "C" {
//#include <sys/syscall.h>
//}

#include <stdio.h>

//////////// logging

inline
void SYS_GetTime_int(double *plrClick, double rFactor)
{
	LARGE_INTEGER	liTime, liFreq;
		
	QueryPerformanceCounter(&liTime);
	QueryPerformanceFrequency(&liFreq);

	*plrClick = (double)((double)liTime.QuadPart / liFreq.QuadPart * rFactor );
}

inline
DWORD SYS_GetTimeInUS()
{
	double		lrTemp = 0;
	SYS_GetTime_int(&lrTemp, 1000000.0);
	return (DWORD)floor(lrTemp + 0.5);
}


typedef unsigned long long LOG_TimeUnit;
static inline LOG_TimeUnit LOG_getTimeStart() {
	return SYS_GetTimeInUS();
}

static inline LOG_TimeUnit LOG_rdtsc() {
	return SYS_GetTimeInUS();
}

static inline LOG_TimeUnit LOG_getTimeStop() {
	return SYS_GetTimeInUS();
}

//#define LOGGING
//#ifdef LOGGING

#define LOG_MAX_ENTRIES 2000000

static inline unsigned long LOG_getThread() {
  return GetCurrentThreadId();
}

typedef struct {
  LOG_TimeUnit start, length;
  unsigned long thread;
  char text[64 - 2*sizeof(LOG_TimeUnit)- sizeof(unsigned long)];
} LOG_entry;

static LOG_entry LOG_data[LOG_MAX_ENTRIES];
static LONG LOG_ptr =0;

//static inline void LOG_add1(const char *text) {
//  LOG_TimeUnit start = LOG_getTimeStart();
//  LOG_TimeUnit stop = start;
//  size_t i = InterlockedAdd(&LOG_ptr, 1);
//  strcpy(LOG_data[i].text, text);
//  LOG_data[i].start = start;
//  LOG_data[i].length = stop-start;
//  LOG_data[i].thread = LOG_getThread();
//}

static inline void LOG_add(const char *text, LOG_TimeUnit start, LOG_TimeUnit stop) {
	size_t i = InterlockedAdd(&LOG_ptr, 1)-1;
	if( i>= LOG_MAX_ENTRIES )
		return;

  strcpy(LOG_data[i].text, text);
  LOG_data[i].start = start;
  LOG_data[i].length = stop-start;
  LOG_data[i].thread = LOG_getThread();
}

#define LOG_optional_break break;

//#else
//#define LOG_add1(a)
//#define LOG_add(a,b,c)
//#define LOG_dump(a)
//#define LOG_optional_break
//#endif

//////////// logging

static char *TIMING_submit;
static char *TIMING_execute;
static LOG_TimeUnit TIMING_start;
static char *TIMING_total;
static int TIMING_submit_i;
static int TIMING_execute_i;
static int TIMING_total_i;
static LOG_TimeUnit delay = 1000;

#define LOG_TIMER_BEGIN(LOG_NAME) \
	LOG_TimeUnit start = LOG_getTimeStart(); 
	
#define LOG_TIMER_END(LOG_NAME) \
	LOG_TimeUnit curr =LOG_rdtsc();		\
	LOG_add(LOG_NAME, start, curr)

inline void LOG_TIMER(const char *LOG_NAME)
{
	LOG_TimeUnit start = LOG_getTimeStart(); 
		LOG_TimeUnit stop = start + delay; 
		LOG_TimeUnit curr; 
	while ((curr = LOG_rdtsc()) < stop); 
		LOG_add(LOG_NAME, start, curr);
}

inline
void TIMING_init() {
    TIMING_submit = (char *) malloc(20*100);
    TIMING_execute = (char *) malloc(20*100);
    TIMING_total = (char *) malloc(20*100);
	TIMING_start = LOG_rdtsc();
}

inline
void TIMING_add(unsigned long long LOG_start, unsigned long long LOG_mid, unsigned long long LOG_stop) {
    TIMING_submit_i += sprintf(&TIMING_submit[TIMING_submit_i], "%llu ", LOG_mid-LOG_start);
    TIMING_execute_i += sprintf(&TIMING_execute[TIMING_execute_i], "%llu ", LOG_stop-LOG_mid);
    TIMING_total_i += sprintf(&TIMING_total[TIMING_total_i], "%llu ", LOG_stop-LOG_start);
}

inline
void TIMING_end(int n) {
    printf("n= %d delay= %llu submit=[ %s] execute=[ %s] total=[ %s]\n",
        n, delay, TIMING_submit, TIMING_execute, TIMING_total);
    free(TIMING_submit);
    free(TIMING_execute);
    free(TIMING_total);
}

#define NUM_ITERATIONS 20


static
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
      fprintf(out, "%lu %llu %llu %llu %s\n",
              LOG_data[i].thread,
              LOG_data[i].start-minimum,
			  LOG_data[i].start - minimum +LOG_data[i].length,
			  LOG_data[i].length,
              LOG_data[i].text);
  }
  fclose(out);
}

