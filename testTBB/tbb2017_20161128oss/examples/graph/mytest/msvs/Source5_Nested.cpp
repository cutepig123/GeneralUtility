#include "tbb/flow_graph.h"
#include "tbb/tick_count.h"
#include <iostream>

using namespace std;
using namespace tbb;
using namespace tbb::flow;

void main5()
{
	typedef continue_node<continue_msg>		node_t;
	typedef const continue_msg&		msg_t;

	graph g;
	function_node< int, int > a(g, unlimited, [](int i) -> int {
		graph h;
		node_t n1(h, [=](msg_t) { cout << "n1: " << i << "\n"; });
		node_t n2(h, [=](msg_t) { cout << "n2: " << i << "\n"; });
		node_t n3(h, [=](msg_t) { cout << "n3: " << i << "\n"; });
		node_t n4(h, [=](msg_t) { cout << "n4: " << i << "\n"; });
		make_edge(n1, n2);
		make_edge(n1, n3);
		make_edge(n2, n4);
		make_edge(n3, n4);
		n1.try_put(continue_msg());
		h.wait_for_all();
		return i;
	});
	function_node< int, int > b(g, unlimited, [](int i) -> int {
		graph h;
		function_node< int, int > m1(h, unlimited, [](int j) -> int {
			cout << "m1: " << j << "\n";
			return j;
		});
		function_node< int, int > m2(h, unlimited, [](int j) -> int {
			cout << "m2: " << j << "\n";
			return j;
		});
		function_node< int, int > m3(h, unlimited, [](int j) -> int {
			cout << "m3: " << j << "\n";
			return j;
		});
		function_node< int, int > m4(h, unlimited, [](int j) -> int {
			cout << "m4: " << j << "\n";
			return j;
		});
		make_edge(m1, m2);
		make_edge(m1, m3);
		make_edge(m2, m4);
		make_edge(m3, m4);
		m1.try_put(i);
		h.wait_for_all();
		return i;
	});
	make_edge(a, b);

	tick_count t0 = tick_count::now();
	for (int i = 0; i < 3; ++i) {
		a.try_put(i);
	}

	g.wait_for_all();
	tick_count t1 = tick_count::now();

	system("pause");
}