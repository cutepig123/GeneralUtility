//task:
//Implement f(n)
// f(n) =	2*n									if n<=3
//			f(n - 1) + f(n - 2)*f(n - 3)		otherwise
//
//requirements:
// use recursive thinking
// but without real - recursive, i.e. need manual construct a stack
//

#include "stdafx.h"
#include "assert.h"
#include "memory.h"


struct Placeholder
{
	bool isSolved;		// whether result is solved or not
	union
	{
		int res;		// to store the result, i.e. f(n)
		int holder_n;	// to store the place holder, i.e. n
	}u;
};

struct Elem
{
	Placeholder *p_res;				//	to store the result pointer
	Placeholder placeholder_1;		//	to store result of f(n-1)
	Placeholder placeholder_2;		//	to store result of f(n-2)
	Placeholder placeholder_3;		//	to store result of f(n-3)
};

struct Stack
{
	static const int maxN = 1000;
	Elem stk[maxN];
	int n;

	Stack()
	{
		n = 0;
		memset(stk, 0, sizeof(stk));
	}
	void push(Elem e)
	{
		assert(n < maxN);
		stk[n++] = e;
	}

	Elem& top(){
		assert(n >= 1);
		return stk[n - 1];
	}
	void pop(){
		assert(n >= 1);
		n--;
	}
	bool isEmpty()
	{
		return n == 0;
	}
};


void initHolder(Placeholder &h, int n)
{
	if (n <= 3)
	{
		h.isSolved = true;
		h.u.res = n * 2;
	}
	else
	{
		h.isSolved = false;
		h.u.holder_n = n;
	}
}

// split a placeholder
void split(Stack &stk, Placeholder &h)
{
	assert(!h.isSolved);
	int n = h.u.holder_n;
	if (n <= 3)
	{
		h.isSolved = true;
		h.u.res = n * 2;
	}
	else
	{
		Elem e1;
		e1.p_res = &h;
		initHolder(e1.placeholder_1, n - 1);
		initHolder(e1.placeholder_2, n - 2);
		initHolder(e1.placeholder_3, n - 3);
		stk.push(e1);
	}
}

void solve(Stack &stk)
{
	while (!stk.isEmpty())
	{
		Elem &e = stk.top();

		if (!e.placeholder_1.isSolved)
		{
			split(stk, e.placeholder_1);
		}
		else if (!e.placeholder_2.isSolved)
		{
			split(stk, e.placeholder_2);
		}
		else if (!e.placeholder_3.isSolved)
		{
			split(stk, e.placeholder_3);
		}

		if (e.placeholder_1.isSolved && e.placeholder_2.isSolved && e.placeholder_3.isSolved)
		{
			e.p_res->u.res = e.placeholder_1.u.res + e.placeholder_2.u.res*e.placeholder_3.u.res;
			e.p_res->isSolved = true;
			stk.pop();
		}

	}
}


int f(int n)
{
	if (n <= 3)
		return n * 2;

	Stack stk;

	Placeholder h;
	h.isSolved = false;
	h.u.holder_n = n;

	split(stk, h);

	solve(stk);
	assert(stk.isEmpty());
	assert(h.isSolved);
	return h.u.res;
}

int f2(int n)
{
	if (n <= 3)
		return n * 2;
	else
		return f2(n - 1) + f2(n - 2)*f2(n - 3);
}

int _tmain(int argc, _TCHAR* argv[])
{
	for (int i = 1; i < 20; i++)
	{
		int n = f(i);
		int n2 = f2(i);
		assert(n == n2);
	}
	return 0;
}

