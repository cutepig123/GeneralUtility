#include "MyTaskSchedular.h"
#include <Windows.h>
#include <vector>
#include <list>
#include <assert.h>


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

class MySemaphore : noncopyable{
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
class MyTaskData:noncopyable
{
private:
	std::unique_ptr<MyTaskBase>	pTask;
	MyEvent evt;
	std::unique_ptr<Handle>  prevReply;
	const std::type_info *ptaskInfo;
	std::string		m_desc;
	DWORD m_threadid;	// which thread does it run at
public:

	//MyTaskData() :ptaskInfo(0), m_threadid(-1){}
	MyTaskData(std::auto_ptr<MyTaskBase>	&task, const std::type_info &taskInfo, Handle *pprevReply, const char *desc)
		:pTask(task.release()), ptaskInfo(&taskInfo), m_threadid(-1) {
		if (pprevReply) prevReply.reset(new Handle(*pprevReply));
		if (desc) m_desc = desc;
	}
	bool isFinished(){
		return evt.Wait(1);
	}
	
	void* getReply(const std::type_info &taskInfo){
		evt.Wait();
		if (taskInfo == *ptaskInfo)
			return pTask->getReply();
		return NULL;
	}

	void Run(){
		m_threadid = GetCurrentThreadId(); 
		if (prevReply)
			prevReply->getReply<int>();
		pTask->operator()();
		evt.Set();

		printf("Task %s run at thread %Iu\n", m_desc.c_str(), m_threadid);
	}
	
	bool isParentDone() const{
		if (!prevReply)
			return true;
		return prevReply->isFinished();
	}

	//bool match(DWORD threadid) const;
};

class Handle::Impl{
public:
	std::shared_ptr<MyTaskData>	p;
};

struct Handle::Access{
	static std::shared_ptr<MyTaskData> getTaskData(const Handle &h){
		return h.impl->p;
	}

	static void reset(Handle &h, std::shared_ptr<MyTaskData>	p2)
	{
		h.impl.reset(new Impl);
		h.impl->p = p2;
	}
};



//bool MyTaskData::match(DWORD threadid) const{
//	assert(threadid != 0);
//
//	if (!prevReply)
//		return true;
//
//	if (Handle::Access::getTaskData(*prevReply)->m_threadid == threadid)
//		return true;
//
//	return false;
//}

Handle::Handle(){
}

Handle::~Handle(){
}

Handle::Handle(const Handle& rhs) {
	if (rhs.impl)
	{
		impl.reset( new Impl(*rhs.impl));
	}
}

Handle& Handle::operator=(const Handle& rhs){
	this->swap(Handle(rhs));
	return *this;
}

void Handle::swap(Handle &rhs){
	std::swap(impl, rhs.impl);
}

void *Handle::getReply(const std::type_info &t){
	if (!impl || !impl->p)
		return NULL;

	return impl->p->getReply(t);
}

bool Handle::isFinished() const{
	if (!impl || !impl->p)
		return false;
	return impl->p->isFinished();
}

class MyTaskScheduler::Impl{
public:

	struct MyThreadPara
	{
		HANDLE m_handle;
		DWORD m_threadidx;
	};

	std::vector<MyThreadPara>	m_vThread;
	BOOL					m_exit;
	
	//MyAutoResetEvent		m_queueEvent;
	MySemaphore				m_queueSemaphore;
	MyMutex					m_queueMutex;
	std::list< std::shared_ptr<MyTaskData> >	m_taskQueue;

	std::shared_ptr<MyTaskData> getTaskAndPop(DWORD threadidx){
		MyLock lock(m_queueMutex);
		if(m_taskQueue.empty())
			return std::shared_ptr<MyTaskData>();

		auto it = m_taskQueue.front();
		m_taskQueue.pop_front();
		return it;
	}
	static DWORD WINAPI MyThread(
		LPVOID lpThreadParameter
		){
		Impl *This = (Impl*)lpThreadParameter;

		DWORD threadidx = GetCurrentThreadId();
		while (!This->m_exit){

			if (WAIT_OBJECT_0 == This->m_queueSemaphore.WaitAndDec(1000))
			{
				std::shared_ptr<MyTaskData> task;

				task = This->getTaskAndPop(threadidx);
				assert(task);	

				// run
				task->Run();
			}
		}
		return 0;
	}

	explicit Impl(int nThread/*, int const aSid[]*/) :m_exit(FALSE){
		for (int i = 0; i < nThread; i++){
			MyThreadPara t;
			t.m_handle = CreateThread(NULL, 0, &MyThread, this, 0, &t.m_threadidx);
			assert(t.m_handle != INVALID_HANDLE_VALUE);
			m_vThread.push_back(t);
		}
	}

	~Impl()
	{
		m_exit = TRUE;
		for (int i = 0; i < (int)m_vThread.size(); i++)
		{
			WaitForSingleObject(m_vThread[i].m_handle, INFINITE);
		}
	}
};

MyTaskScheduler::MyTaskScheduler(int nThread/*, int const aSid[]*/)
	:impl(new Impl(nThread/*, aSid*/)){
}

MyTaskScheduler::~MyTaskScheduler(){
	delete impl;
}

Handle MyTaskScheduler::RunAsynTaskImpl(std::auto_ptr<MyTaskBase> p, const std::type_info &taskinfo, Handle *pstPrevReply, const char *desc){
	Handle h;

	std::shared_ptr<MyTaskData> ptask(new MyTaskData(p, taskinfo, pstPrevReply, desc));
	{
		MyLock lock(impl->m_queueMutex);
		
		impl->m_taskQueue.push_back(ptask);
	}
	Handle::Access::reset(h, ptask);

	impl->m_queueSemaphore.IncAndSignal();

	return h;
}
