// csdn_1.cpp : �������̨Ӧ�ó������ڵ㡣
//
//����༭����
//��Ŀ���� :
//
//��ͳ�ı༭�������������ֲ�����������ɾ���ģ���������Ҫ���۵ı༭����ֻ�������ֲ�����������һ���ַ���ɾ��һ���ַ��������������ַ��������ֱ༭���룬����һ���ַ����������һ���ַ��������ٲ���������
//
//�����ʽ��
//
//�������ݣ�ÿ���������У�ÿ��һ���ַ�����ÿ���ַ������Ȳ�����1000��ֻ�д�дӢ����ĸ��ɡ�
//
//�����ʽ��
//
//ÿ���������һ�а���һ����������ʾ��Ҫ���ٲ����Ĵ�����

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
//˼·��s1����l1,s2����l2,��󹫹����ַ�������m,�򷵻�l1+l2-m*2

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

