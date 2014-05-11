// csdn_1.cpp : 定义控制台应用程序的入口点。
//
//另类编辑距离
//题目详情 :
//
//传统的编辑距离里面有三种操作，即增、删、改，我们现在要讨论的编辑距离只允许两种操作，即增加一个字符、删除一个字符。我们求两个字符串的这种编辑距离，即把一个字符串变成另外一个字符串的最少操作次数。
//
//输入格式：
//
//多组数据，每组数据两行，每行一个字符串。每个字符串长度不超过1000，只有大写英文字母组成。
//
//输出格式：
//
//每组数据输出一行包含一个整数，表示需要最少操作的次数。

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
//思路：s1长度l1,s2长度l2,最大公共子字符串长度m,则返回l1+l2-m*2

using namespace std;

#define N 1001
int dp[N + 1][N + 1];
char str1[N], str2[N];
int maxx(int a, int b)
{
	if (a > b)
		return a;
	return b;
}
int LCSL(int len1, int len2)
{
	int i, j;
	int len = maxx(len1, len2);
	for (i = 0; i <= len; i++)
	{
		dp[i][0] = 0; dp[0][i] = 0;
	}
	for (i = 1; i <= len1; i++)
	for (j = 1; j <= len2; j++)
	{
		if (str1[i - 1] == str2[j - 1])
		{
			dp[i][j] = dp[i - 1][j - 1] + 1;
		}
		else
		{
			dp[i][j] = maxx(dp[i - 1][j], dp[i][j - 1]);
		}
	}
	return dp[len1][len2];
}

int main(int argc, char* argv[])
{
	while (cin >> str1 >> str2)
    {
		int len1 = strlen(str1);
		int len2 = strlen(str2);
            
		int n = LCSL(len1, len2);
		//printf("%d %d %d\n", len1, len2, len1 + len2 - 2 * n);
		printf("%d\n", len1 + len2 - 2 * n);
    }
    return 0;
}

