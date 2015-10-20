#pragma once
#include <noncopyable.hpp>
#include <Windows.h>

class MyMutex :noncopyable{
	CRITICAL_SECTION cs;
public:
	MyMutex(){
		InitializeCriticalSection(&cs);
	}
	~MyMutex(){
		DeleteCriticalSection(&cs);
	}
	void lock(){
		EnterCriticalSection(&cs);
	}
	void unlock(){
		LeaveCriticalSection(&cs);
	}
};

class MyLock :noncopyable{
	MyMutex *p;
public:
	explicit MyLock(MyMutex &m):p(&m){
		m.lock();
	}

	~MyLock(){
		p->unlock();
	}
};