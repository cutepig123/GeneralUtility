//给定一个带通配符问号的数W，问号可以代表任意一个一位数字。
//
//再给定一个整数X，和W具有同样的长度。
//
//问有多少个整数符合W的形式并且比X大？
//
//
//输入格式
//
//多组数据，每组数据两行，第一行是W，第二行是X，它们长度相同。在[1..10]之间.
//
//输出格式
//
//每行一个整数表示结果。
//答题说明
//
//输入样例
//
//36 ? 1 ? 8
//
//236428
//
//8 ? 3
//
//910
//
//?
//
//5
//
//输出样例
//
//100
//
//0
//
//4
//
//
//友情提醒：
//
//1、本题用OJ模式答题，答题时可把下面输入框内预留的函数全部直接去掉，自己另起函数名实现本题功能。
//
//2、因为本系统的OJ模式对于Java 和 C# 的支持有问题，所以本题暂不支持这两种语言挑战。


#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

using namespace std;

#define	N	11

char s1[N];
char s2[N];

long long res[N];
int CmpRes[N];

long long getN(int n)
{
	if (!s1[n])return getN(n - 1);

	long long r = 0;
	if (n == 0)
	{
		if (s1[n] != '?')
		{
			if (s1[n] > s2[n])
				r = 1;
			else
			{
				CmpRes
				r = 0;
			}
				
		}
		else
			r = '9' - s2[n];
	}
	else
	{
		if (s1[n] != '?')
		{
			r = getN(n-1);
		}
		else
		{
			long long r_1 = getN(n - 1);
			if (r_1==1)
				r = ('9' - s2[n])*r_1;
			else
				r = 10*r_1;
		}
	}

	res[n] = r;
	cout << "n=" << n << " r= " << r << endl;
	return r;
}
int main(int argc, char* argv[])
{
	while (cin>>s1>>s2)
	{
		for (int i = 0; i < N; i++)
		{
			res[i] = -1;
			CmpRes[i] = -2;	// Not init
		}

		cout << getN(strlen(s1)) << endl;
	}
    return 0;
}

