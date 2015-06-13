// test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "stdio.h"
#include "string.h"

// ������ȫ����Ԥ���������һ������P[j]����ʾ��ƥ�䵽B����ĵ�j����ĸ����j+1����ĸ����ƥ����ʱ���µ�j����Ƕ��١�P[j]Ӧ������������B[1..P[j]]=B[j-P[j]+1..j]�����ֵ��
void makeNext(const char *s, int n, int next[])
{
	next[0]=0;
	next[1]=0;

	for(int j=2;j<n;j++)
	{
		//B[0..k]==B[j-k..j]
		int k=j-1;
		for(;k>0;k--)
		{
			if(0==strncmp(s, s+j-k,k))
			{
				printf("j %d k %d\n",j,k);
				printf("%s\n",s);
				for(int m=0;m<j-k;m++)
					printf(" ");
				for(int m=0;m<k;m++)
					printf("%c",(s+j-k)[m]);
				printf("\n");
				break;
			}
		}
		next[j]=k;
	}
}

void get_next(const char* T, int n,int next[])  
{  
    //��ģʽ��T��next����ֵ����������next  
   int  i = 0; next[1] = 0; int j = 0;  
    while (i < n)  
    {  
        if(j ==0 || T[i] == T[j])  
        {  
            ++i; ++j;  
            if (T[i] != T[j])  
                next[i] = j;  
            else  
                next[i] = next[j];  
        }   
        else   
        {  
            j = next[j];  
        }     
    }  
}  

void match2(const char *A, int nA, const char *B, int nB, const int next[])
{
	int i=0,j=0;
	while(i<nA && j<nB)
	{
		//A[i-j..i]==B[0..j]
		printf("%s\n",A);
		for(int k=0;k<i-j;k++) 
			printf(" ");
		for(int k=0;k<=j;k++) 
			printf("%c",B[k]);
		printf("\n");

		if(j==0|| A[i]==B[j]) 
		{i++;	j++;}
		else
			j=next[j];

		if(j>=nB)
		{
			printf("matched at pos %d\n",i-nB);
			j=0;
		}


	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	char *B="ababacb";
	int next[100];
	char *A="abababaababacbabababaababacb";
	int nA=strlen(A);
	int nB=strlen(B);
	makeNext( B, nB, next);
	printf("Next:\n");for(int i=0;i<nB;i++) printf("%d ", next[i]); printf("\n");
	
	get_next( B, nB, next);
	printf("Next:\n");for(int i=0;i<nB;i++) printf("%d ", next[i]); printf("\n");

	match2(A,nA,B,nB,next);

	return 0;
}

