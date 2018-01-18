
#include <cloudsim/virtual_network.h>
#include <cloudsim/physical_node.h>

#include <cloudsim/reservable.h>
#include <cloudsim/physical_network.h>
#include <cloudsim/util.h>
#include <cloudsim/greedy.h>

int main(int argc, char** argv)
{
/*
	cloudsim::PhysicalNetwork p("test/test_physical.json");
	cloudsim::VirtualNetwork v("test/test_virtual1.json");

	auto pn1 = dynamic_cast<cloudsim::PhysicalNode*>(*p.nodes[1]);
	auto pn3 = dynamic_cast<cloudsim::PhysicalNode*>(*p.nodes[3]);
	auto pn4 = dynamic_cast<cloudsim::PhysicalNode*>(*p.nodes[4]);
	auto pe2 = dynamic_cast<cloudsim::PhysicalEdge*>(*p.edges[2]);
	auto pe3 = dynamic_cast<cloudsim::PhysicalEdge*>(*p.edges[3]);




	pn1->reserve(50, 0);
	pn3->reserve(80, 0);
	pn4->reserve(40, 0);
	pn1->reserve(50, 1);
	pn3->reserve(80, 1);
	pn4->reserve(40, 1);
	pe2->reserve(70, 0);
	pe3->reserve(35, 0);

	cloudsim::Greedy g(p, v, true, std::cout);
	cloudsim::Mapping m = g();

	m.operator<<(std::cout);


	cloudsim::Mapping m;

	m.switches.add({0, 1});
	m.servers.add({1, 3});
	m.servers.add({2, 3});
	m.servers.add({3, 4});
	m.links.add({0, {2}});
	m.links.add({1, {2}});
	m.links.add({2, {3}});


	p.embed(v, m);

	p.remove_network(*(p.virtual_network(0)));

*/
	return 0;
}
