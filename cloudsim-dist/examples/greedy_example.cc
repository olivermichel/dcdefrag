
#include <ostream>

#include <cloudsim/physical_network.h>
#include <cloudsim/virtual_network.h>
#include <cloudsim/physical_node.h>
#include <cloudsim/physical_edge.h>
#include <cloudsim/virtual_node.h>
#include <cloudsim/virtual_edge.h>
#include <cloudsim/greedy.h>
#include <cloudsim/mapping.h>

int main(int argc, char** argv)
{
	const unsigned long TYPE_SWITCH = 1;
	const unsigned long TYPE_SERVER = 2;

	cloudsim::PhysicalNetwork p("greedy_example_pnet.json");

	cloudsim::VirtualNetwork v0("greedy_example_vnet0.json");
	cloudsim::VirtualNetwork v1("greedy_example_vnet1.json");
	cloudsim::VirtualNetwork v2("greedy_example_vnet2.json");

	try {
		cloudsim::Greedy g(p, v0);
		cloudsim::Mapping m = g();
//		m.operator<<(std::cout);

		p.embed(v0, m);

	} catch (cloudsim::runtime_error& e) {
		std::cout << "embedding error: " << e.what() << std::endl;
	}

	try {
		cloudsim::Greedy g(p, v1);
		cloudsim::Mapping m = g();
//		m.operator<<(std::cout);

		p.embed(v1, m);

	} catch (cloudsim::runtime_error& e) {
		std::cout << "embedding error: " << e.what() << std::endl;
	}


	try {
		cloudsim::Greedy g(p, v2);
		cloudsim::Mapping m = g();
//		m.operator<<(std::cout);

		p.embed(v2, m);

	} catch (cloudsim::runtime_error& e) {
		std::cout << "embedding error: " << e.what() << std::endl;
	}

	std::cout << "embdeded networks: " << std::endl;
	for (auto sub : p.subgraphs())
		std::cout << static_cast<cloudsim::VirtualNetwork*>(sub)->id() << std::endl;

	return 0;
}
