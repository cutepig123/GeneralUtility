/*
2.设计包含min函数的栈（栈）
定义栈的数据结构，要求添加一个min函数，能够得到栈的最小元素。
要求函数min、push以及pop的时间复杂度都是O(1)。
*/

#include <stdio.h>
#include <assert.h>

#define N 100
int stk[N];
int stkmin[N];
int n=0;

int min(int a, int b)
{
	return a<b?a:b;
}

void push(int i)
{
	assert(n<N);
	stk[n]=i;
	
	if(n>0)
		stkmin[n]=min(stkmin[n-1], i);
	else
		stkmin[n]=i;
	n++;
}

int mymin()
{
	assert(n>0);
	return stkmin[n-1];
}

int pop()
{
	assert(n>0);
	int v= stk[n-1];
	n--;
	return v;
}

void main()
{
	//int an[]={4,3,2,1};
	int an[]={4,3,2,1,2,3,4,5};
	for(int i=0; i<sizeof(an)/sizeof(an[0]); i++)
		push(an[i]);
		
	for(int i=0; i<sizeof(an)/sizeof(an[0]); i++)
	{
		int minv=mymin();
		int t=pop();
		printf("%d %d %d\n", i, minv, t);
	}
}
