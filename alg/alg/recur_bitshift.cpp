/*百度之星大赛：一道简单而不简约的算法题
09/24? 王先生 ?8 评论 ? 百度 , 笔试 , 算法

设计一个算法，把一个含有 N 个元素的数组循环右移 K 位，要求时间复杂度为 O (N)，且只允许使用两个附加变量。*/

/*
source data: abcd efgh ijkl mnop qrst uvwx yz00
after shift 4bits: efgh ijkl mnop qrst uvwx yz00 abcd
*/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

//assumes little endian
void printBits(int size, void const * ptr)
{
    unsigned char *b = (unsigned char*) ptr;
    unsigned char byte;
    int i, j;

    for (i=size-1;i>=0;i--)
    {
        for (j=7;j>=0;j--)
        {
            byte = b[i] & (1<<j);
            byte >>= j;
            printf("%u", byte);
			if (j == 4)
				printf(" ");
        }
		printf(" ");
    }
	
}

void shift(int 	n, 	// array size
			char *s,	// IO, data 
			int ns,	//num shift
			char *d
			)
{
	assert(ns>=0 && ns<8);
	
	char t =s[0];
	int nBitShiftout = ns;
	for(int i=0; i<n; i++)
	{
		if(i+1<n)
			d[i] = (s[i]>>nBitShiftout) | (s[i+1]<<(8-nBitShiftout));
		else
			d[i] = (s[i]>>nBitShiftout) | (t<<(8-nBitShiftout));
		nBitShiftout =8-nBitShiftout;
	}
}

int main()
{
#define N 4
	//int s[N];
	int d[N];
	//for(int i=0; i<N; i++)
	//	s[i] =rand();
	
	int start = 5;
	char s[N];
	int n = sizeof(s);

	for (int i = 0; i < N; i++)
		s[i] = start + i;

	printf("\nS: "); printBits(n, (char*)s);
	shift(n, (char*)s, 5, (char*)d);
	printf("\nD: "); printBits(n, (char*)d);
}
