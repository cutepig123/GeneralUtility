#include <iostream>
#include <tbb/flow_graph.h>
using namespace std;
using namespace tbb::flow;
void main1() {
	graph g;
	continue_node<continue_msg >h(g, [](const continue_msg &)
	{std::cout << "Hello "; });
	continue_node<continue_msg >w(g, [](const continue_msg &)
	{std::cout << "Flow Graph World\n"; });
	make_edge(h, w);
	h.try_put(continue_msg());
	g.wait_for_all();
	system("pause");
}