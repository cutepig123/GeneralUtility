//Difficulties 
//1: difficult to debug program error, e.g. If I forgot to connect input_port<1>(join), there is no any error, but result is wrong

#include <cstdio>
#include "tbb/flow_graph.h"
using namespace tbb::flow;
struct square {
	int operator()(int v) { return v*v; }
};
struct cube {
	int operator()(int v) { return v*v*v; }
};
class sum {
	int &my_sum;
public:
	sum(int &s) : my_sum(s) {}
	int operator()(std::tuple< int, int > v) {
		my_sum += std::get<0>(v) + std::get<1>(v);
		return my_sum;
	}
};
int main3() {
	int result = 0;
	graph g;
	broadcast_node<int> input(g);
	function_node<int, int> squarer(g, unlimited, square());
	function_node<int, int> cuber(g, unlimited, cube());
	join_node< std::tuple<int, int>, queueing > join(g);
	function_node<std::tuple<int, int>, int>
		summer(g, serial, sum(result));
	make_edge(input, squarer);
	make_edge(input, cuber);
	make_edge(squarer, input_port<0>(join));
	make_edge(cuber, input_port<1>(join));
	make_edge(join, summer);
	for (int i = 1; i <= 10; ++i)
		input.try_put(i);
	g.wait_for_all();
	printf("Final result is %d\n", result);
	system("pause");
	return 0;
}