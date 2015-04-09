// TestVirtualInheritance.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

namespace NonVirtualInheritant
{
	class Top
	{
	public:
		int a;
	};

	class Left : public Top
	{
	public:
		int b;
	};

	class Right : public Top
	{
	public:
		int c;
	};

	class Bottom : public Left, public Right
	{
	public:
		int d;
	};

	void test()
	{
		Bottom* bottom = new Bottom();
		Left* left = bottom;
		Top* top = left;
		Right* right = bottom;
		Top* top2 = right;
		printf("bottom %p\n", bottom);
		printf("bottom->Left:: %p\n", &bottom->Left::a);
		printf("left %p\n", left);
		printf("top %p\n", top);
		printf("right %p\n", right);
		printf("top2 %p\n", top2);
	}
}

namespace VirtualInheritant
{
	class Top
	{
	public:
		int a;
		virtual ~Top()
		{}
	};

	class Left : public virtual Top
	{
	public:
		int b;
	};

	class Right : public virtual Top
	{
	public:
		int c;
	};

	class Bottom : public Left, public Right
	{
	public:
		int d;
	};

	void test()
	{
		Bottom* bottom = new Bottom();
		Left* left = bottom;
		Top* top = left;
		Right* right = bottom;
		Top* top2 = right;
		printf("bottom %p\n", bottom);
		printf("bottom->Left::a %p\n", &bottom->Left::a);
		printf("left %p\n", left);
		printf("top %p\n", top);
		printf("right %p\n", right);
		printf("top2 %p\n", top2);
		printf("bottom->a %p\n", &bottom->a);
		printf("bottom->b %p\n", &bottom->b);
		printf("bottom->c %p\n", &bottom->c);
		printf("bottom->d %p\n", &bottom->d);

		Left *left2 = dynamic_cast<Left*>(top);
		printf("left2 %p\n", left2);

		/*bottom left left2 002AE478
bottom->b 002AE47C
right 002AE480
bottom->c 002AE484
bottom->d 002AE488
top top2 002AE48C
bottom->a bottom->Left::a 002AE490

bottom地址分布：	
left | right | top
0		8		20	   

left.vtpr	002AE478
b			002AE47C
right.vptr	002AE480
c			002AE484
d			002AE488
top.vptr	002AE48C
a			002AE490

虚函数表里面有啥呢？
？？*/
	}
}


int _tmain(int argc, _TCHAR* argv[])
{
	//NonVirtualInheritant::test();
	VirtualInheritant::test();
	return 0;
}

