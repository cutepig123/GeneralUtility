//����һ����ͨ����ʺŵ���W���ʺſ��Դ�������һ��һλ���֡�
//
//�ٸ���һ������X����W����ͬ���ĳ��ȡ�
//
//���ж��ٸ���������W����ʽ���ұ�X��
//
//
//�����ʽ
//
//�������ݣ�ÿ���������У���һ����W���ڶ�����X�����ǳ�����ͬ����[1..10]֮��.
//
//�����ʽ
//
//ÿ��һ��������ʾ�����
//����˵��
//
//��������
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
//�������
//
//100
//
//0
//
//4
//
//
//�������ѣ�
//
//1��������OJģʽ���⣬����ʱ�ɰ������������Ԥ���ĺ���ȫ��ֱ��ȥ�����Լ���������ʵ�ֱ��⹦�ܡ�
//
//2����Ϊ��ϵͳ��OJģʽ����Java �� C# ��֧�������⣬���Ա����ݲ�֧��������������ս��


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

