#pragma once
#include <noncopyable.hpp>
#include <assert.h>
#include <Windows.h>
#include <stdio.h>

class MySemaphore :noncopyable{
	HANDLE h;
public:
	MySemaphore(){
		h =CreateSemaphore(0, 0, 10000, NULL);
		assert(h && h != INVALID_HANDLE_VALUE);
	}

	DWORD WaitAndDec(DWORD t){
		DWORD ret =WaitForSingleObject(h, t);
		printf("Semaphore Waited return %d\n", ret);
		return ret;
	}
	
	void IncAndSignal(){
		LONG lPreviousCount = 0;
		BOOL ret = ReleaseSemaphore(h, 1, &lPreviousCount);
		assert(ret);
		printf("Semaphore Inc lPreviousCount %d\n", lPreviousCount);
	}

	~MySemaphore(){
		BOOL ret = CloseHandle(h);
		assert(ret);
	}
};