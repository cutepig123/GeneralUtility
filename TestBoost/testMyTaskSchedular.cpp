#include "MyTaskSchedular.h"
#include "assert.h"
#include "windows.h"

void MyDouble(int const *a, int *b){
	Sleep(1000);
	*b = *a * 2;
}

void MyTriple(int const *a, int *b){
	*b = *a * 3;
}

void main(){

	// handle
	Handle h;
	
	assert(!h.isFinished());
	//assert(h.getReply<int>() == NULL);

	// task
	MyTaskScheduler scheduler(5);

	Handle h1;

	h1 = scheduler.RunAsynTask(MyDouble, 10, NULL, "MyDouble");

	Handle h2 = scheduler.RunAsynTask(MyTriple, 10, &h1,	"MyTriple");

	Handle h4 = scheduler.RunAsynTask(MyTriple, 20, NULL, "MyTriple2");

	assert(60==*h4.getReply<int>());

	int *pRpy2 = h2.getReply<int>();
	
	assert(*pRpy2 == 30);
	
	assert(h1.isFinished());
	int *pRpy1 = h1.getReply<int>();
	assert(*pRpy1 == 20);

	Handle h3(h2);
	assert(*h3.getReply<int>() == 30);


	//int a;
	//assert(typeid(int*) == typeid((void*)&a));
}