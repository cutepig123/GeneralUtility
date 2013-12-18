//union_find_sets
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

//两个优化：
//1：rank数组的引入，启发式函数
//2：路径压缩；


//Abstract: UFSet                  
//Author:Lifeng Wang （Fandywang）
// Model One 与Model 2 路径压缩方式不同,合并标准不同
const int MAXSIZE = 500010;
int rank[MAXSIZE];    // 节点高度的上界
int parent[MAXSIZE]; // 根节点

int FindSet(int x){// 查找+递归的路径压缩
     if( x != parent[x] ) parent[x] = FindSet(parent[x]);
     return parent[x];
}
void Union(int root1, int root2){
     int x = FindSet(root1), y = FindSet(root2);
     if( x == y ) return ;
     if( rank[x] > rank[y] ) parent[y] = x;
     else{
         parent[x] = y;
         if( rank[x] == rank[y] ) ++rank[y];
     }
}
void Initi(void){
     memset(rank, 0, sizeof(rank));
     for( int i=0; i < MAXSIZE; ++i ) parent[i] = i;
}


int main()
{
	Initi();

	Union(0,1);
	Union(0,2);
	Union(2,1);
	Union(3,1);
	Union(4,5);
	Union(4,6);
	Union(7,8);

	int nSet=0;
	for( int i=0; i<10; i++)	
		if(parent[i]==i)
			nSet++;
	assert(nSet==4);
}	
