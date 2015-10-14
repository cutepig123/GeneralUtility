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
};

class MyLock :noncopyable{
public:
	explicit MyLock(MyMutex &){}
};

class MyTaskData:noncopyable
{
private:
	std::unique_ptr<MyTaskBase>	pTask;
	MyEvent evt;
	Handle prevReply;
	const std::type_info *ptaskInfo;
public:

	MyTaskData() :ptaskInfo(0){}
	MyTaskData(std::auto_ptr<MyTaskBase>	&task, const std::type_info &taskInfo, Handle *pprevReply)
		:pTask(task.release()), ptaskInfo(&taskInfo) {
		if (pprevReply) prevReply = *pprevReply;
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
		if (!prevReply.isEmpty())
			prevReply.getReply<int>();
		pTask->operator()();
		evt.Set();
	}
};

class Handle::Impl{
public:
	std::shared_ptr<MyTaskData>	p; 
	
};

Handle::Handle():impl(0){

}

Handle::~Handle(){
	if (impl)
	{
		delete impl; impl = 0;
	}
}

Handle::Handle(const Handle& rhs):impl(0) {
	if (rhs.impl)
	{
		impl = new Impl(*rhs.impl);
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
		return true;
	return impl->p->isFinished();
}

class MyTaskScheduler::Impl{
public:
	std::vector<HANDLE>		m_vThread;
	BOOL					m_exit;
	
	MyAutoResetEvent		m_queueEvent;
	MyMutex					m_queueMutex;
	std::list< std::shared_ptr<MyTaskData> >	m_taskQueue;

	static DWORD WINAPI MyThread(
		LPVOID lpThreadParameter
		){
		Impl *This = (Impl*)lpThreadParameter;
		while (!This->m_exit){

			This->m_queueEvent.Wait();
			
			while (1)
			{
				std::shared_ptr<MyTaskData> task;

				// dequeue
				{
					MyLock lock(This->m_queueMutex);
					if (This->m_taskQueue.empty())
						break;

					task = This->m_taskQueue.front();
					This->m_taskQueue.pop_front();
				}
				
				// run
				task->Run();
			}
		}
		return 0;
	}

	explicit Impl(int nThread) :m_exit(FALSE){
		for (int i = 0; i < nThread; i++){
			HANDLE h = CreateThread(NULL, 0, &MyThread, this, 0, NULL);
			assert(h != INVALID_HANDLE_VALUE);
			m_vThread.push_back(h);
		}
	}

	~Impl()
	{
		m_exit = TRUE;
		for (int i = 0; i < (int)m_vThread.size(); i++)
		{
			WaitForSingleObject(m_vThread[i], INFINITE);
		}
	}
};

MyTaskScheduler::MyTaskScheduler(int nThread)
	:impl(new Impl(nThread)){
}

MyTaskScheduler::~MyTaskScheduler(){
	delete impl;
}

Handle MyTaskScheduler::RunAsynTaskImpl(std::auto_ptr<MyTaskBase> p, const std::type_info &taskinfo, Handle *pstPrevReply){
	Handle h;

	h.impl = new Handle::Impl;
	h.impl->p.reset(new MyTaskData(p, taskinfo, pstPrevReply));
	{
		MyLock lock(impl->m_queueMutex);
		
		impl->m_taskQueue.push_back(h.impl->p);
	}
	
	impl->m_queueEvent.Set();

	return h;
}
