#include <stdio.h>
#include <assert.h>
#include <algorithm>
//
//匈牙利算法是求解最大匹配的有效算法，该算法用到了增广路的定义(也称增广轨或交错轨)：若边集合P是图G中一条连通两个未匹配顶点的路径，并且属M的边和不属M的边(即已匹配和待匹配的边)在P上交替出现，则称P为相对于M的一条增广路径。
//由增广路径的定义可以推出下述三个结论：
//    1.  P的路径长度必定为奇数，第一条边和最后一条边都不属于M。
//    2.  P经过“取反操作”（即非M中的边变为M中的边，原来M中的边去掉）可以得到一个更大的匹配M’。
//3.  M为G的最大匹配当且仅当不存在相对于M的增广路径。
//从而可以得到求解最大匹配的匈牙利算法：
//(1)置M为空
//(2)找出一条增广路径P，通过“取反操作”获得更大的匹配M’代替M
//(3)重复(2)操作直到找不出增广路径为止
//根据该算法，可以选择深搜或者广搜实现，下面给出易于实现的深度优先搜索（DFS）实现。
//prototype

int    n, m, match[100];                        //二分图的两个集合分别含有n和m个元素,match[i]存储集合m中的节点i在集合n中的匹配节点,初值为-1。
bool    visited[100], map[100][100];               //map存储邻接矩阵。

bool DFS(const int &k)
{
	for(int i = 0; i < m; i++)
	{
		if( map[k][i] && !visited[i]    )	//if the edge node not visited
		{
			visited[i] = true;	//为什么不放在下面那个if里面??
			if( match[i] == -1 || DFS(match[i]) )   //寻找是否为增广路径
			{ 
				match[i] = k;            //路径取反操作。
				return true;
			}
		}
	}
	return false;
}

int main(void)
{
	//...........
	int     count = 0;
	memset(match, -1, sizeof(match));
	for(i = 0; i < n; i++)
	{    //以二分集中的较小集为n进行匹配较优
		memset(visited, 0,sizeof(visited));
		if( DFS(i) )     ++count;    //count为匹配数
	}
	//............
	return 0;
}
