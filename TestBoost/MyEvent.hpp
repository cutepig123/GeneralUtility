#pragma once
#include <noncopyable.hpp>
#include <Windows.h>

class MyEvent:noncopyable{
	HANDLE h;
public:
	explicit MyEvent(BOOL bManualReset =TRUE)
	{
		h = CreateEvent(NULL, bManualReset, FALSE, NULL);
	}

	~MyEvent(){
		CloseHandle(h);
	}

	bool Wait(DWORD tm=INFINITE){
		return WaitForSingleObject(h, tm) == WAIT_OBJECT_0;
	}

	void Set(){
		SetEvent(h);
	}

	void Reset(){
		ResetEvent(h);
	}
};

class MyAutoResetEvent :public MyEvent{
public:
	MyAutoResetEvent() :MyEvent(FALSE){
	}
};
