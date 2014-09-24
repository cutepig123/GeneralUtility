// test1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

class X{
public: int i;
};


class A: public virtual X{
public: int j1;
};


class B: public virtual X{
public: int j2;
};

class C: public A, public B{
public: int k;
};

void foo(X *pa) {pa->i=1;}

int _tmain(int argc, _TCHAR* argv[])
{
	//foo(new A);
	//foo(new C);
	C *pc=new C;
	printf("i %x\n", &pc->i);
	printf("j1 %x\n", &pc->j1);
	printf("j2 %x\n", &pc->j2);
	printf("k %x\n", &pc->k);
	return 0;
}

