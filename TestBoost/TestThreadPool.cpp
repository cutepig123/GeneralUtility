#include <MyThreadPool.hpp>

int main()
{
	MyThreadPool pool(4);
	
	MyThreadPool::HandleType h[5];

	for (int i = 0; i < 5; i++)
	{
		h[i] = pool.Enqueue([i](){Sleep(1000); printf("%d\n", i); });
		assert(!h[i]->Wait(0));
	}

	for (int i = 0; i < 5; i++){
		assert(h[0]->Wait(2000));
	}
}
