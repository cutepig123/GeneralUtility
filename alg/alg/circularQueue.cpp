#include <stdio.h>
#include <assert.h>
#include <vector>

template <class T, size_t N=10>
class CircularQueue
{
public:
	CircularQueue()
		:begin_(0), end_(0),data_(N+1)
	{
	}

	void Enqueue(const T& t)
	{
		data_[end_] = t;
		end_ = (end_ + 1) % data_.size();
		if (end_ == begin_)
			begin_ = (begin_ + 1) % data_.size(); /* full, overwrite */

	}

	const T* Dequeue() 
	{
		if( IsEmpty() )
			return NULL;

		const T* pT = &data_[begin_];
		begin_ = (begin_ + 1) % data_.size();
		return pT;
	}

private:
	size_t begin_,end_;
	std::vector<T> data_;

	int IsFull() {
		return (end_ + 1) % data_.size() == begin_; 
	}
 
	int IsEmpty() {
		return begin_==end_; 
	}

};


int main()
{
	CircularQueue<int> Q;
	for(int i=0; i<2; i++ )
	{
		printf("Enqueue %d\n", i);
		Q.Enqueue(i);
	}

	printf("Dequeue\n");
		
	while( const int* p=Q.Dequeue() )
	{
		printf("%d ", *p);
	}
}
