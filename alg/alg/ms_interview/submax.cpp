/*
3.求子数组的最大和（数组）
题目：
输入一个整形数组，数组里有正数也有负数。
数组中连续的一个或多个整数组成一个子数组，每个子数组都有一个和。
求所有子数组的和的最大值。要求时间复杂度为O(n)。

例如输入的数组为1, -2, 3, 10, -4, 7, 2, -5，和最大的子数组为3, 10, -4, 7, 2，
因此输出为该子数组的和18。
*/
#include <stdio.h>
#include <assert.h>

int max(int a, int b)
{
	return a>b?a:b;
}

void main()
{
	int an[]={1, -2, 3, 10, -4, 7, 2, -5};
	int n=sizeof(an)/sizeof(an[0]);
	

	
	
	int sum=0;
	int sumidx[2]={0,0};
	int res=0;
	int residx[2]={0,0};
	
	for(int i=0; i<n;i++)
	{
		if(sum>0)
		{
			sumidx[1]=i;
			sum =an[i]+sum;
		}
		else
		{
			sumidx[0]=sumidx[1]=i;
			sum =an[i];
		}
		
		if(sum>res)
		{
			res=sum;
			residx[0]=sumidx[0];
			residx[1]=sumidx[1];
		}
	}
	
	printf("%d %d->%d", res,residx[0],residx[1]);
}


/*DP解法的具体方程设sum[i]为前i个元素中
包含第i个元素且和最大的连续子数组
result为已找到的子数组中和最大的
对第i+1个元素有两种选择
做为新子数组的第一个元素、放入前面找到的子数组
*/
