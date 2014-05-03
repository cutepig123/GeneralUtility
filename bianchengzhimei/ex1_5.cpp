#include "inc.h"
#include "assert.h"
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

#define	N 100
int a[N*2];
int nID[2];
int nMissID = 2;

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

int findAnyBit1(int x)
{
	for (int i = 0; i < sizeof(int)* 8; i++)
	{
		if (x & (1 << i))
			return (1 << i);
	}
	assert(0);
	return -1;
}
void solve()
{
	int x = 0;
	for (int i = 0; i < N * 2; i++)
		x = x^a[i];

	if (nMissID==1)
		std::cout << "Find missing char " << x << std::endl;
	else
	{
		// Find bits with 1
		int y = findAnyBit1(x);
		// split data to 2 grps, and find missing char in each grp
		int z[2] = { 0 };
		for (int i = 0; i < N * 2; i++)
		{
			int id = (a[i] & y) ? 0:1;
			z[id] = z[id] ^ a[i];
		}

		std::cout << "Find missing char " ;
		print(z, z + nMissID);
	}
}
void ex()
{
	init();
	solve();
}