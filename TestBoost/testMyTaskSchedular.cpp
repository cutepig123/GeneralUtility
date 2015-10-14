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
	MyTaskScheduler scheduler(1);

	Handle h1 = scheduler.RunAsynTask(MyDouble, 10);
	
	Handle h2 = scheduler.RunAsynTask(MyTriple, 10, &h1);
	int *pRpy2 = h2.getReply<int>();
	
	assert(*pRpy2 == 30);
	assert(!h1.isEmpty());
	assert(h1.isFinished());
	int *pRpy1 = h1.getReply<int>();
	assert(*pRpy1 == 20);
	//int a;
	//assert(typeid(int*) == typeid((void*)&a));
}