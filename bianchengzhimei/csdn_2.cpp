
//F(X)
//
//������˾��
//�� Ч �ڣ�
//
//CSDN
//2014 - 04 - 16��2015 - 04 - 16
//
//�� �� �� ����
//�� �� ʱ ����
//�������Ҫ��
//
//120����
//C C++ Java C#
//
//��Ŀ����
//
//���Ƕ��� F��x�������� x  mod��a*b�� == 0������a��b�����������ڸ���һ��n������Ҫ��� F��n��
//
//�����ʽ��
//
//�������ݣ�ÿ���һ����һ������n, 0 < n <= 10 ^ 11��
//
//�����ʽ��
//
//ÿ�����һ�У�����������(a, b)����
//
//
//
//����˵��
//
//��������
//
//1
//
//2
//
//3
//
//4
//
//���������
//
//1
//
//3
//
//3
//
//6
//
//����
//
//��һ�飺 ��1��1��
//
//�ڶ��飺(1��1�� ��1��2�� ��2�� 1��
//
//�����飺  ��1��1�� ��1��3�� ��3��1��
//
//�����飺   ��1��1�� ��1��2�� ��1�� 4�� ��2�� 1�� ��2��2�� ��4��1��


#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

using namespace std;

#define	N	1000
unsigned long long yueshu[N];
size_t nyueshu = 0;
unsigned long long nTotalYueshu = 0;

void calcNumYueshu(unsigned long long x)
{
	memset(yueshu, 0, sizeof(yueshu));
	nyueshu = 0;
	for (unsigned long long i = 2; i <= x; i++)
	{
		yueshu[nyueshu] = 0;
		while (i!=x)
		{
			if (x%i == 0)
			{
				yueshu[nyueshu]++;
				x = x / i;
			}
			else
				break;
		}

		if (yueshu[nyueshu] > 0)
			nyueshu++;
	}

	printf("nyueshu %d\n", nyueshu);
	nTotalYueshu = 1;
	for (size_t i = 0; i < nyueshu; i++)
	{

	}
}
int main(int argc, char* argv[])
{
	unsigned long long x;
	
	while (cin>>x)
	{

	}
    return 0;
}

