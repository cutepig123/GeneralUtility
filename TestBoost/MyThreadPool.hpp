#pragma once
#include <MyBlockingQueue.hpp>
#include <MyEvent.hpp>
#include <thread>
#include <memory>
#include <functional>
#include <assert.h>
#include <algorithm>

class MyThreadPool{
	
public:
	typedef std::function<void()>	FunType;
	
	struct QueueItem{
		FunType f;
		MyEvent	evt;

		DWORD Wait(DWORD t){
			return evt.Wait(t);
		}
	};
	
	typedef	std::shared_ptr<QueueItem>	HandleType;
	
	explicit MyThreadPool(int nThread){
		bExit =false;
		for(int i=0; i<nThread; i++)
			t.push_back(new std::thread(&MyThreadPool::thread_fun, this));
	}
	
	~MyThreadPool(){
		bExit =true;
		join();
		std::for_each(t.begin(), t.end(), [](std::thread* i){delete i; });
	}
	
	void join(){
		std::for_each(t.begin(), t.end(), [](std::thread* i){i->join(); });
	}

	HandleType Enqueue(FunType f){
		HandleType i(new QueueItem);
		i->f =f;
		q.Enqueue(i);
		return i;
	}
private:
	
	MyBlockingQueue< HandleType > q;
	std::list<std::thread*>	t;
	bool	bExit;
	
	void thread_fun(){
		while (!bExit)
		{
			HandleType h = q.Dequeue(1000);
			if (h)
			{
				h->f();
				h->evt.Set();
			}
		}
	}
};
