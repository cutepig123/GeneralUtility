#include <iostream>
#include <algorithm>
// ��ת�ӱ�
// todo: ��δ������շ���

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
		// ��[0, i)�����ķŵ�������
		int *pMax = std::max_element(a, a + i);
		flip(a, pMax + 1);
		print(a, a + N);
		//���巭ת
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