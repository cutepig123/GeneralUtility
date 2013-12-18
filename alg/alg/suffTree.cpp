#include <stdio.h>
#include <assert.h>
#include <algorithm>

#define	MAX_NUM_CHILD	(1<<8)

struct NODE
{
	NODE* child[MAX_NUM_CHILD];

	NODE()
	{
		std::fill( child, child+MAX_NUM_CHILD, (NODE*)0 );
	}
};

// insert a string to suff tree
void insert(const char*s, NODE *node)
{
	NODE *p = node;
	for(;*s;++s)
	{
		NODE *&p2 = p->child[*s/*-'a'*/];
		if(!p2)
			p2 = new NODE;

		p = p2;
	}
}

//create a suff tree of a string
void createSuffTree(const char*s, NODE *node)
{
	for(;*s;++s)
	{
		insert(s, node);
	}
}

//
char stack[200]={0};
int nstack=0;

bool isEnd(const NODE*p)
{
	for(int i=0; i<MAX_NUM_CHILD; i++)
		if(p->child[i])
			return false;
	return true;
}

void print(const NODE*p)
{
	if(!p)
		return;

	for(int i=0; i<MAX_NUM_CHILD; i++)
	{
		if(p->child[i])	//if can go
		{
			stack[nstack++]=i/*+'a'*/;	//go
			
			if( isEnd(p->child[i]) )
			{
				stack[nstack++]=0;
				printf("%s\n", stack);
				nstack--;
			}
			else
				print(p->child[i]);

			nstack--;
		}
	}
}

bool isMatch(const char*p, const NODE*node)
{
	for(; *p; ++p)
	{
		if(!node->child[*p])
			return false;
	}

	return true;
}

int main()
{
	const char*s="abcde";

	NODE *node = new NODE;
	createSuffTree(s,node);
	print(node);

	{
		const char*p = "bcd";
		assert(isMatch(p, node));
	}

	{
		const char*p = "bcdf";
		assert(!isMatch(p, node));
	}
}
