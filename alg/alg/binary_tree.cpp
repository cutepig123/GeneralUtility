#include "binary_tree.h"
#include <stdlib.h>
#include <stdio.h>

struct BSTreeNode
{
    int v; // value of node
    BSTreeNode *l; // left child of node
    BSTreeNode *r; // right child of node
};


void Init(BSTreeNode *&pT)
{
	pT = 0;
}

void Insert(BSTreeNode *&pT, int v)
{
	// srch for insert position
	BSTreeNode *p(pT),	//the insert posiiton
		*pp(0);	//the parent of the insert posiiton
	bool isLc = true;

	while(p)
	{
		pp = p;

		if(p->v==v)
			break;
		else if(p->v > v)
		{
			p=p->l;
			isLc = true;
		}
		else if(p->v < v)
		{
			p=p->r;
			isLc  = false;
		}
		
	}

	if(p)	
	{
		//the node already exists
	}
	else
	{
		p = new BSTreeNode;
		p->v = v;
		p->l = p->r = 0;

		if(pp)	//parent is not root
		{
			if(isLc)
				pp->l = p;
			else
				pp->r = p;
		}
		else	//parent is root
		{
			pT = p;
		}
	}
}

//zhong xu visit
void Print_ZX(BSTreeNode const *pT)
{
	if(!pT)
		return;
	if(pT->l)
		Print_ZX(pT->l);

	printf("%d ", pT->v);

	if(pT->r)
		Print_ZX(pT->r);
	
}

//把二元查找树转变成排序的双向链表
BSTreeNode* ConvertToList(BSTreeNode *pT)
{
	if(!pT)
		return pT;
	if(pT->v
}

Print_List(BSTreeNode const*pHead)
{
	for(; pHead; pHead = pHead->r )
	{
		printf("%d ", pHead->v);
	}
	printf("\n");
}

void TestTree()
{
	BSTreeNode *pT;
	Init(pT);
	srand(12345);

	printf("Insert..\n");

	for(int i=0; i<10; i++)
	{
		int v = rand()%20;
		Insert(pT, v );
	}

	printf("Print_ZX..\n");
	Print_ZX(pT);

	printf("ConvertToList..\n");
}
