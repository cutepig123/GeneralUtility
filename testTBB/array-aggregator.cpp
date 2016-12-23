#include "tbb/flow_graph.h"
#include <iostream>
#include <vector>

using namespace tbb::flow;
const int threshold = 5;

typedef std::vector<int> input_array_type;
typedef multifunction_node<input_array_type *, tuple<input_array_type*> > aggregator_node_type;

struct multi_functor {
	input_array_type* msa;

	// allocate a vector to store the intermediate results
	multi_functor() { msa = new input_array_type(); msa->reserve(2*threshold); msa->resize(0);}
	// have to create an explicit copy_constructor because the functors _will_ be copied at least once
	multi_functor(const multi_functor & other) { msa = new input_array_type(); msa->reserve(2*threshold); msa->resize(0);}
	// on destruction check the array and print a note of how many values are thrown away.
	~multi_functor() { if(msa->size() > 0) std::cout << "Throwing away " << msa->size() << " elements\n"; delete msa;}
	// receive a vector.  Push-back the elements in that vector to our result.  If we have passed a threshold,
	// create a new empty vector and send the full one to the next node, otherwise just return.
	// we should probably delete the passed-in vectors; the problem is we expect the input values to be
	// const-qualified.  In a real program one would do better buffer space management.
    void operator()(const input_array_type *in, aggregator_node_type::output_ports_type &outports) {
		for(int i=0; i < in->size(); ++i) {
		    msa->push_back((*in)[i]);
		}
		if(msa->size() >= threshold) {
            input_array_type *old_saved_array = msa;
			msa = new input_array_type(2*threshold);msa->reserve(2*threshold); msa->resize(0);
			get<0>(outports).try_put(old_saved_array);
		}
    }	
};

// function_node body that just prints the value of the vector it receives.
struct uni_functor {
    continue_msg operator()(const input_array_type *in) {
	    std::cout << "Received array with size " << in->size() << " containing [";
		for( int i = 0; i < in->size(); ++i) {
		    std::cout << " " << (*in)[i];
		}
		std::cout << " ]\n";
	}
};

int main() {
    graph g;
	multifunction_node<input_array_type *, tuple<input_array_type*> > my_mfnode(g,serial,multi_functor() );
	function_node<input_array_type *> my_out_node(g, unlimited, uni_functor());
	make_edge(output_port<0>(my_mfnode), my_out_node);
	for( int i = 0; i < 2 * threshold; i+=2) {
	// send an array of length 2 to the multifunction_node.
	    input_array_type *ia = new std::vector<int>(2);
		ia->clear();
		ia->push_back(i); ia->push_back(i+1);
		my_mfnode.try_put(ia);
	}
	// absolutely do not forget a wait_for_all(), unless you are certain you don't need it.
	g.wait_for_all();
}
