#include <stdio.h>
#include <algorithm>
#include <map>
// DFS ref: CLRS $22.3 & 22.4

#define	MAX_VERTEX	10

int G[MAX_VERTEX][MAX_VERTEX] = {0};
int nVertex = 0;

#define	WHITE	1
#define	GRAY	2
#define	BLACK	3
int color[MAX_VERTEX] = {0};
int parent[MAX_VERTEX] = {0};
int time = 0;
int d[MAX_VERTEX] = {0};	//start visit time for each vertex
int f[MAX_VERTEX] = {0};	//end visit time for each vertex
int sequence[MAX_VERTEX*2] = {0};

#define	ADD_EDGE(x,y)	G[x-1][y-1] = 1;

void DFS_VISIT(int u)
{
	color[u] = GRAY;

	sequence[time]=u;
	d[u] = ++time;

	for(int v=0; v<nVertex; v++)
	{
		if( G[u][v] && color[v] == WHITE )
		{
			parent[v] = u;
			DFS_VISIT(v);
		}
	}

	color[u]=BLACK;
	sequence[time]=u;
	f[u] = ++time;
}

void DFS()
{
	for(int i=0; i<nVertex; i++)
	{
		color[i] = WHITE;
		parent[i] = -1;		//NIL
	}

	time = 0;

	for(int u=0; u<nVertex; u++)
	{
		if( color[u]==WHITE )
			DFS_VISIT(u);
	}
}

void testDFS()
{
	ADD_EDGE(1,2);
	ADD_EDGE(1,4);

	ADD_EDGE(2,5);

	ADD_EDGE(3,5);
	ADD_EDGE(3,6);

	ADD_EDGE(4,2);

	ADD_EDGE(5,4);

	ADD_EDGE(6,6);

	nVertex = 6;

	DFS();

	printf("visiting sequence (image22-4 in CLRS:)\n");

	for(int i=0; i<time; i++)
	{
		printf("%c ", sequence[i]+'u');
	}
}

void testTopologicalSort()
{
	ADD_EDGE(1,2);
	ADD_EDGE(1,8);

	ADD_EDGE(2,3);
	ADD_EDGE(2,8);

	ADD_EDGE(3,4);
	ADD_EDGE(3,6);

	ADD_EDGE(4,5);

	ADD_EDGE(5,6);

	ADD_EDGE(7,8);

	nVertex = 6;

	DFS();

	char *text[] = {"undershot", "pant", "belt", "shirt", "tie", "jacket", "socks", "shoes", "watch"};

	printf("visiting sequence (image22-7 in CLRS:)\n");

	for(int i=0; i<time; i++)
	{
		printf("%s ", text[ sequence[i]%nVertex ]);
	}

	printf("\nTopological sort result:\n");
	std::map<int,int> m;	//reverse finish time -> vertex id
	for(int i=0; i<nVertex; i++)
		m[ -f[i] ] = i;

	for(std::map<int,int>::iterator it=m.begin(); it!=m.end(); ++it)
	{
		printf("%s ", text[ it->second%nVertex ]);
	}
}

int main()
{
	testTopologicalSort();
}