/* Test Result
After Define G 0.00206538
After Setup Graph 0.332915
After trigger 0.0166173
After Run 0.0512333
After Sleep 1s 1.00724

Conclusion:
Setup time is too long, Need consider how to remove this time
*/

#include "tbb/flow_graph.h"
#include "tbb/tick_count.h"
#include <iostream>
#include <vector>

using namespace std;
using namespace tbb;
using namespace tbb::flow;

// Solar test

void main()
{
	struct TimeInfo
	{
		tick_count t;
		char *info;
	}astTi[100];
	int nTime = 0;

	int nTrig = 10000;
	const int nCam = 6;

#define	ADD_TIME(s)	astTi[nTime].info = s; astTi[nTime].t = tick_count::now(); nTime++;

	typedef continue_node<continue_msg>		node_t;
	typedef const continue_msg&		msg_t;

	ADD_TIME("Init");
	graph g;
	ADD_TIME("After Define G");

	vector<node_t>	vGrab[nCam], vWarp[nCam], vRecon[nCam], vMerge[nCam];
	for (int i = 0; i < nCam; i++)
	{
		vGrab[i].reserve(nTrig);
		vWarp[i].reserve(nTrig);
		vRecon[i].reserve(nTrig);
		vMerge[i].reserve(nTrig);

		for (int j = 0; j < nTrig; j++)
		{
			vGrab[i].push_back(node_t(g, [=](msg_t) {}));
			vWarp[i].push_back(node_t(g, [=](msg_t) {}));
			vRecon[i].push_back(node_t(g, [=](msg_t) {}));
			vMerge[i].push_back(node_t(g, [=](msg_t) {}));
		}
	}
	
	for (int i = 0; i < nCam; i++)
	{
		for (int j = 0; j < nTrig; j++)
		{
			make_edge(vGrab[i][j], vWarp[i][j]);
		}

		for (int j = 0; j+2 < nTrig; j++)
		{
			make_edge(vWarp[i][j], vRecon[i][j]);
			make_edge(vWarp[i][j+1], vRecon[i][j]);
			make_edge(vWarp[i][j+2], vRecon[i][j]);
			make_edge(vRecon[i][j], vMerge[i][j]);

			if (j>0)
			{
				make_edge(vMerge[i][j-1], vMerge[i][j]);
			}
		}
	}
	ADD_TIME("After Setup Graph");

	for (int i = 0; i < nCam; i++)
	{
		for (int j = 0; j < nTrig; j++)
			vGrab[i][j].try_put(continue_msg());
	}
	ADD_TIME("After trigger");

	g.wait_for_all();
	ADD_TIME("After Run");

	Sleep(1000);
	ADD_TIME("After Sleep 1s");

	for (int i = 1; i < nTime; i++)
	{
		printf("%s %g\n", astTi[i].info, (astTi[i].t - astTi[i-1].t).seconds());
	}
	system("pause");
}
