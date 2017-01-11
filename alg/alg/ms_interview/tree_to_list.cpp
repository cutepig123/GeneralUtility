/*
 1.把二元查找树转变成排序的双向链表（树）
 题目：
输入一棵二元查找树，将该二元查找树转换成一个排序的双向链表。
要求不能创建任何新的结点，只调整指针的指向。
   10
  / /
  6  14
 / / / /
4  8 12 16
 转换成双向链表
4=6=8=10=12=14=16。
*/

#include <stdio.h>
#include <assert.h>

struct TreeNode
{
	int val;
	TreeNode *lc, *rc;
};

struct ListNode
{
	int val;
	ListNode *prev, *next;
};

TreeNode	g_pool[100];
int			g_nUsed =0;

TreeNode	*newNode(int v, TreeNode *lc, TreeNode *rc)
{
	if(g_nUsed>=100)
		return NULL;
		
	TreeNode *p= &g_pool[g_nUsed++];
	p->val =v;
	p->lc =lc;
	p->rc =rc;
	return p;
}

void initTree(TreeNode **p)
{
	*p =newNode(10,
		newNode( 6,
			newNode(4, NULL, NULL),
			newNode(8, NULL, NULL)
				),
		newNode( 14,
			newNode(12, NULL, NULL),
			newNode(16, NULL, NULL)
				)
		);
};

void connect(ListNode *a, ListNode *b, ListNode *c)
{
	a->next =b;
	b->prev=a;
	b->next=c;
	c->prev=b;
}

void toList(TreeNode *p, ListNode **ppHead,ListNode **ppTail)
{
	ListNode *p1H =NULL, *p1T =NULL;
	ListNode *p2H =NULL, *p2T =NULL;
	if(p->lc)
		toList(p->lc, &p1H, &p1T);
		
	if(p->rc)
		toList(p->rc, &p2H, &p2T);
		
	ListNode *pp[]={p1H, p1T, (ListNode*)p, p2H, p2T};
	*ppHead =*ppTail=NULL;
	
	for(int i=0; i<5; i++)
	{	
		if(!*ppHead && pp[i])
			*ppHead =pp[i];
			
		if(!*ppTail && pp[4-i])
			*ppTail =pp[4-i];
	}
	
	assert(*ppHead && *ppTail);
	(*ppHead)->prev=*ppTail;
	(*ppTail)->next=*ppHead;
	
	assert( (p1T?1:0) ==(p1H?1:0));
	assert( (p2T?1:0) ==(p2H?1:0));
		
	if(p1T)	
	{
		if(p2H) 
		{
			connect( p1T, ((ListNode*)p), p2H);
		}
		else
		{
			connect( p1T, ((ListNode*)p), p1H);
		}
	}
	else
	{
		if(p2H) 
		{
			connect( p2T, ((ListNode*)p), p1H);
		}
		else
		{
			p->lc =p->rc =p;
		}
	}
}

void printList(ListNode const *l)
{
	for(ListNode const *p=l; ; p=p->next)
	{
		printf("%d ",p->val);
		if(p->next==l)break;
	}
	
}

void main()
{
	TreeNode *p=0;
	initTree(&p);
	
	ListNode *h=0, *t=0;
	toList(p, &h, &t);
	
	printList(h);
}
