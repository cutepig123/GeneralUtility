// TestVSBug.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


int _tmain(int argc, _TCHAR* argv[])
{
	int a=1;

	{
		a=2;
		// check value of a in quick watch, you will find it display the value of a in "T1"
		printf("a %p %d\n", &a, a);

		int a=3;			// T1
		printf("a %p %d\n", &a, a);
	}

	printf("a %p %d\n", &a, a);

	return 0;
}

