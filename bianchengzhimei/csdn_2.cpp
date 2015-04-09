
//F(X)
//
//发布公司：
//有 效 期：
//
//CSDN
//2014 - 04 - 16至2015 - 04 - 16
//
//难 度 等 级：
//答 题 时 长：
//编程语言要求：
//
//120分钟
//C C++ Java C#
//
//题目详情
//
//我们定义 F（x）是满足 x  mod（a*b） == 0这样的a，b的组数。现在给你一个n，你需要求出 F（n）
//
//输入格式：
//
//多组数据，每组第一行有一个整数n, 0 < n <= 10 ^ 11。
//
//输出格式：
//
//每组输出一行，满足条件的(a, b)对数
//
//
//
//答题说明
//
//输入样例
//
//1
//
//2
//
//3
//
//4
//
//输出样例：
//
//1
//
//3
//
//3
//
//6
//
//解释
//
//第一组： （1，1）
//
//第二组：(1，1） （1，2） （2， 1）
//
//第三组：  （1，1） （1，3） （3，1）
//
//第四组：   （1，1） （1，2） （1， 4） （2， 1） （2，2） （4，1）


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

