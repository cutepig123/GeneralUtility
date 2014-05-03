#include "inc.h"
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

#define	N 100
int a[N*2];
int nID[2];
int nMissID = 1;

void init()
{
	srand(time(NULL));

	for (int i = 0; i < nMissID; i++)
	{
		nID[i] = rand() % (N)+1;
	}
	for (int i = 0; i < N; i++)
	{
		a[i] = a[i + N] = i + 1;
	}
	for (int i = 0; i < nMissID; i++)
	{
		a[nID[i] - 1] = 0;
	}

	std::cout << "Init missing char ";
	print(nID, nID + nMissID);
}

void Find()
{
	int x = 0;
	for (int i = 0; i < N * 2; i++)
		x = x^a[i];
	std::cout << "Find missing char " << x << std::endl;
}
void ex()
{
	init();
	Find();
}