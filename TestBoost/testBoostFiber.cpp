// ConsoleApplication3.cpp : Defines the entry point for the console application.
//

//#include "stdafx.h"
#include <boost/fiber/all.hpp>
#include <functional>
#include <boost/assert.hpp>

using namespace boost::fibers;
using namespace boost::this_fiber;

boost::fibers::fiber f1; // not-a-fiber

void some_fn() {
	printf("AA");
}

void f() {
	boost::fibers::fiber f2(some_fn);

	f1 = std::move(f2); // f2 moved to f1
}

struct callable {
	void operator()() {

	}
};

boost::fibers::fiber copies_are_safe() {
	callable x;
	return boost::fibers::fiber(x);
} // x is destroyed, but the newly-created fiber has a copy, so this is OK

boost::fibers::fiber oops() {
	callable x;
	return boost::fibers::fiber(std::ref(x));
} // x is destroyed, but the newly-created fiber still has a reference
  // this leads to undefined behaviour

void t1() {
	f();
	f1.join();
}

void t2() {
	{
		boost::fibers::fiber f(some_fn);
	} // std::terminate() will be called

}

void t3() {
	{
		boost::fibers::fiber f(some_fn);
		f.detach();
	} // okay, program continues
}

typedef boost::fibers::buffered_channel< int > channel_t;

void sendmy(channel_t & chan) {
	for (int i = 0; i < 5; ++i) {
		chan.push(i);
	}
	
}

void recvmy(channel_t & chan) {
	//int i;
	//while (boost::fibers::channel_op_status::success == chan.pop(i)) {
	//	std::cout << "received " << i << std::endl;
	//}

	for (unsigned int value : chan) {
		std::cout << value << " ";
	}

}

void fmain() {
	channel_t chan{ 2 };

	boost::fibers::fiber f1(std::bind(sendmy, std::ref(chan)));
	boost::fibers::fiber f2(std::bind(recvmy, std::ref(chan)));

	f1.join();
	f2.join();

	chan.close();
}

void t4() {

	boost::fibers::fiber ff(fmain);
	ff.join();
}


int main()
{


	t4();

    return 0;
}

