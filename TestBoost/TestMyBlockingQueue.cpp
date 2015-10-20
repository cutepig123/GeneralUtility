#include <MyBlockingQueue.hpp>
#include <memory>
#include <assert.h>

int main(){
	MyBlockingQueue< std::shared_ptr<int> > q;
	assert(!q.Dequeue(1000));
	
	for(int i=0; i<3;i++){
		std::shared_ptr<int> p(new int(i));
		q.Enqueue(p);
	}
	
	for(int i=0; i<3;i++){
		assert(*q.Dequeue(INFINITE)==i);
	}
	assert(!q.Dequeue(1000));
}
