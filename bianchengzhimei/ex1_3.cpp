#include <iostream>
#include <algorithm>
// 翻转烙饼
// todo: 尚未完成最终方案

template <class T>
void print(T begin, T end)
{
	for (; begin != end; ++begin)
	{
		std::cout << *begin;
	}
	std::cout << std::endl;
}

template <class T>
void flip(T begin, T end)
{
	std::cout << "Flip - "; print(begin, end);
	end--;
	for (; begin < end; begin++, end--)
	{
		std::swap(*begin, *end);
	}
}

void method1(int a[], int N)
{
	for (int i = N; i >0; i--)
	{
		// 把[0, i)中最大的放到最上面
		int *pMax = std::max_element(a, a + i);
		flip(a, pMax + 1);
		print(a, a + N);
		//整体翻转
		flip(a, a + i);
		print(a, a + N);

	}
}
void ex()
{
	int a[] = { 3, 2, 1, 5, 4 };
	int N = sizeof(a) / sizeof(a[0]);

	method1(a,N);
}