//#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include "sg/superglue.hpp"
#include "common.h"
#include "sg/option/instr_trace.hpp"

#define	N_TRIG		200
#define	FACTOR		10*1000
#define	GRAB_TIME	3.5*FACTOR
#define	SHEAR_TIME	1*FACTOR
#define	RECON_TIME	4.5*FACTOR
#define	MERGE_TIME	3*FACTOR
#define	RECON_POOL_SZ	5
#define	N_CAM			6

void MySleep(DWORD n)
{
	for(DWORD a =SYS_GetTimeInUS()+n; SYS_GetTimeInUS()<a;);
}

//struct MyOptions : public DefaultOptions<MyOptions> {};
struct MyOptions : public DefaultOptions<MyOptions> {
	typedef Enable TaskName;
	typedef Trace<MyOptions> Instrumentation;
	typedef Disable Stealing;
};

#define	MY_DEFINE_TASK(x)	struct x : public Task<MyOptions> { \
	std::string get_name() { return #x; }

MY_DEFINE_TASK( grab )
	int i_;
    grab(int i, Handle<MyOptions> *hr, Handle<MyOptions> &h) {
		if(hr)
			register_access(ReadWriteAdd::read, *hr);
        register_access(ReadWriteAdd::write, h);
		i_ =i;
    }
	void run() {  
		char s[100];
		sprintf(s, "grab %d", i_);
		
		LOG_TIMER_BEGIN(s);

		MySleep(GRAB_TIME);

		LOG_TIMER_END(s);
	}
};

MY_DEFINE_TASK(shear)
	int i_;
    shear(int i, Handle<MyOptions> &hr, Handle<MyOptions> &h) {
		register_access(ReadWriteAdd::read, hr);
        register_access(ReadWriteAdd::write, h);
		i_ =i;
    }
	void run() {  
		char s[100];
		sprintf(s, "shear %d", i_);
		
		LOG_TIMER_BEGIN(s);

		MySleep(SHEAR_TIME);

		LOG_TIMER_END(s);
	}
};

MY_DEFINE_TASK(grab2)
	int i_;
    grab2(int i, Handle<MyOptions> *hr, Handle<MyOptions> &h) {
		if(hr)
			register_access(ReadWriteAdd::read, *hr);
        register_access(ReadWriteAdd::write, h);
		i_ =i;
    }
	void run() {  
		char s[100];
		sprintf(s, "grabx %d", i_);
		
		LOG_TIMER_BEGIN(s);

		MySleep(GRAB_TIME);

		LOG_TIMER_END(s);
	}
};

MY_DEFINE_TASK(recon)
	int i_, j_;
    recon(int i, int j, Handle<MyOptions> ahr[3], Handle<MyOptions> *hr_pre, Handle<MyOptions> &h) {
		for(int j=0; j<3; j++)
			register_access(ReadWriteAdd::read, ahr[j]);
		if(hr_pre)
			register_access(ReadWriteAdd::read, *hr_pre);
        register_access(ReadWriteAdd::write, h);
		
		i_ =i; j_=j;
    }
	void run() {  
		char s[100];
		sprintf(s, "recon %d_%d", i_, j_);
		
		LOG_TIMER_BEGIN(s);

		MySleep(RECON_TIME);

		LOG_TIMER_END(s);
	}
};

MY_DEFINE_TASK(merge)
	int i_, j_;
    merge(int i, int j, Handle<MyOptions> &hr1,Handle<MyOptions> *hr2, Handle<MyOptions> &h) {
		register_access(ReadWriteAdd::read, hr1);
		if(hr2)
			register_access(ReadWriteAdd::read, *hr2);
        register_access(ReadWriteAdd::write, h);
		
		i_ =i; j_ =j;
    }
	void run() {  
		char s[100];
		sprintf(s, "merge %d_%d", i_,j_);
		
		LOG_TIMER_BEGIN(s);

		MySleep(MERGE_TIME);

		LOG_TIMER_END(s);
	}
};

static SuperGlue<MyOptions> tm;
static Handle<MyOptions> *hGrab;
static Handle<MyOptions> *hShear;
static Handle<MyOptions> *hRecon[N_CAM];
static Handle<MyOptions> *hMerge[N_CAM];

//#define	Get_Idx(p, cam, trig)	p[cma*N_TRIG + trig]

static
void compute(LOG_TimeUnit *LOG_start,
             LOG_TimeUnit *LOG_mid,
             LOG_TimeUnit *LOG_stop, int DIM) {

    LOG_TimeUnit curr = *LOG_start = LOG_getTimeStart();

    for (int k = 0; k < N_TRIG; ++k) {
        tm.submit(new grab( k, k>0?&hGrab[k-1]:0, hGrab[k] ));
    }
	
	for (int k = 0; k < N_TRIG; ++k) {
        tm.submit(new shear( k, hGrab[k], hShear[k] ));
    }
	
	// In fact we have max 20 reconstruct at same time because we uses a pool...
	for( int i=0; i<N_CAM; i++ )
	{
		for (int k = 0; k < N_TRIG-2; ++k) {

			if( k>RECON_POOL_SZ )
				tm.submit(new recon( i, k, &hShear[k], &hRecon[i][k-RECON_POOL_SZ], hRecon[i][k] ));
			else
				tm.submit(new recon( i, k, &hShear[k], NULL, hRecon[i][k] ));
		}
	
		for (int k = 0; k < N_TRIG-2; ++k) {
			tm.submit(new merge( i, k, hRecon[i][k], k>0?&hMerge[i][k-1]:0, hMerge[i][k] ));
		}
	}

	//for (int k = 0; k < N_TRIG; ++k) {
 //       tm.submit(new grab2(k, 0, hGrab[k] ));
 //   }

    *LOG_mid = LOG_getTimeStop();

    tm.barrier();

    *LOG_stop = LOG_getTimeStop();
}

int main_solar(int argc, char *argv[]) {

    int n = 5;

    if (argc > 1)
        n = atoi(argv[1]);
    if (argc > 2)
        delay = atoi(argv[2]);

    hGrab = new Handle<MyOptions>[N_TRIG];
	hShear = new Handle<MyOptions>[N_TRIG];

	for(int i=0; i<N_CAM; i++)
	{
		hRecon[i] = new Handle<MyOptions>[N_TRIG-2];
		hMerge[i] = new Handle<MyOptions>[N_TRIG-2];
	}

    TIMING_init();

    for (int i = 0; i < NUM_ITERATIONS; ++i) {
        LOG_TimeUnit LOG_start, LOG_mid, LOG_stop;
        compute(&LOG_start, &LOG_mid, &LOG_stop, n);
        TIMING_add(LOG_start, LOG_mid, LOG_stop);
        LOG_optional_break
    }

    TIMING_end(n);

    LOG_dump("superglue.log");
	Trace<MyOptions>::dump("solar.log");
   
    return 0;
}
