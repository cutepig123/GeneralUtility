/*4.在二元树中找出和为某一值的所有路径（树）

题目：输入一个整数和一棵二元树。
从树的根结点开始往下访问一直到叶结点所经过的所有结点形成一条路径。
打印出和与输入整数相等的所有路径。
例如 输入整数22和如下二元树
    10 
  /   /  
 5    12  
/ \     
4  7
则打印出两条路径：10, 12和10, 5, 7。*/


#include <stdio.h>
#include <assert.h>

struct TreeNode
{
	int val;
	TreeNode *lc, *rc;
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
		newNode( 5,
			newNode(4, NULL, NULL),
			newNode(7, NULL, NULL)
				),
		newNode( 12,0,0
				)
		);
}

int 	g_npath[100];
int		g_ndepth=0;

void	solve(TreeNode const *p, int object)
{
	assert(object>0);
	TreeNode const *ap[]={p->lc, p->rc};
	
	for(int i=0; i<2; i++)
	{
		if(ap[i])
		{
			g_npath[g_ndepth++] =ap[i]->val;
			
			if(!ap[i]->lc && !ap[i]->rc && object-ap[i]->val==0)
			{
				for(int i=0; i<g_ndepth; i++)
					printf("%d ", g_npath[i]);
				printf("\n");
			}
			else
				solve(ap[i], object-ap[i]->val);
			g_ndepth--;
		}
	}
}


void main()
{
	TreeNode *p=0;
	initTree(&p);
	solve(p, 22-p->val);
}
