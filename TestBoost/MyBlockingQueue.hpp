#pragma once
#include <MySemaphore.hpp>
#include <MyMutex.hpp>
#include <list>
#include <Windows.h>

template <class T>
class MyBlockingQueue :noncopyable{
	std::list<T>	q;
	MySemaphore		sem;
	MyMutex		mtx;
public:
	void Enqueue(T const &t){
		MyLock l(mtx);
		q.push_back(t);
		sem.IncAndSignal();
	}
	
	T Dequeue(DWORD timeout){
		
		if(WAIT_OBJECT_0==sem.WaitAndDec(timeout)){
			MyLock l(mtx);
			T t =q.front();
			q.pop_front();
			return t;
		}
		return T();
	}
};