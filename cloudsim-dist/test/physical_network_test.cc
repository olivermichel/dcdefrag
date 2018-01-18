
#include <catch/catch.h>
#include <vector>
#include "cloudsim/physical_network.h"

void block_resources(cloudsim::PhysicalNetwork& p)
{
	for (auto node : p.nodes)
		dynamic_cast<cloudsim::PhysicalNode*>(node)->block(50.0, 0); // avg residual 50

	for (auto node : p.nodes)
		dynamic_cast<cloudsim::PhysicalNode*>(node)->block(50.0, 1); // avg residual ~ 178

	for (auto edge : p.edges)
		dynamic_cast<cloudsim::PhysicalEdge*>(edge)->block(50.0, 0); // avg residual ~ 100
}

TEST_CASE("PhysicalNetwork::PhysicalNetwork()", "[PhysicalNetwork]")
{
	SECTION("the network is empty upon construction")
	{
		cloudsim::PhysicalNetwork p;
		REQUIRE(p.nodes.count() == 0);
		REQUIRE(p.edges.count() == 0);
	}
}

TEST_CASE("PhysicalNetwork::PhysicalNetwork(std::string file_name)", "[PhysicalNetwork]")
{
	SECTION("the network can be constructed from a json file")
	{
		cloudsim::PhysicalNetwork p("test/test_physical.json");
		REQUIRE(p.nodes.count() == 7);
		REQUIRE(p.edges.count() == 6);
		REQUIRE(dynamic_cast<cloudsim::PhysicalNode*>(*(p.nodes[0]))->capacity(0) == Approx(100));
		REQUIRE(dynamic_cast<cloudsim::PhysicalNode*>(*(p.nodes[0]))->capacity(1) == Approx(600));
		REQUIRE(dynamic_cast<cloudsim::PhysicalNode*>(*(p.nodes[3]))->capacity(0) == Approx(100));
		REQUIRE(dynamic_cast<cloudsim::PhysicalNode*>(*(p.nodes[3]))->capacity(1) == Approx(100));
		REQUIRE(dynamic_cast<cloudsim::PhysicalNode*>(*(p.nodes[0]))->residual(0) == Approx(100));
		REQUIRE(dynamic_cast<cloudsim::PhysicalNode*>(*(p.nodes[0]))->residual(1) == Approx(600));
		REQUIRE(dynamic_cast<cloudsim::PhysicalNode*>(*(p.nodes[3]))->residual(0) == Approx(100));
		REQUIRE(dynamic_cast<cloudsim::PhysicalNode*>(*(p.nodes[3]))->residual(1) == Approx(100));
		REQUIRE(dynamic_cast<cloudsim::PhysicalNode*>(*(p.nodes[0]))->utilization(0) == Approx(0));
		REQUIRE(dynamic_cast<cloudsim::PhysicalNode*>(*(p.nodes[0]))->utilization(1) == Approx(0));
		REQUIRE(dynamic_cast<cloudsim::PhysicalNode*>(*(p.nodes[3]))->utilization(0) == Approx(0));
		REQUIRE(dynamic_cast<cloudsim::PhysicalNode*>(*(p.nodes[3]))->utilization(1) == Approx(0));

		REQUIRE(dynamic_cast<cloudsim::PhysicalEdge*>(*(p.edges[0]))->capacity(0) == Approx(200.0));
		REQUIRE(dynamic_cast<cloudsim::PhysicalEdge*>(*(p.edges[0]))->utilization(0) == Approx(0));
		REQUIRE(dynamic_cast<cloudsim::PhysicalEdge*>(*(p.edges[1]))->capacity(0) == Approx(200.0));
		REQUIRE(dynamic_cast<cloudsim::PhysicalEdge*>(*(p.edges[1]))->utilization(0) == Approx(0));
		REQUIRE(dynamic_cast<cloudsim::PhysicalEdge*>(*(p.edges[1]))->residual(0) == Approx(200.0));

		REQUIRE(dynamic_cast<cloudsim::PhysicalNode*>(*(p.nodes[0]))->group() == 0);
		REQUIRE(dynamic_cast<cloudsim::PhysicalEdge*>(*(p.edges[0]))->group() == 0);
		REQUIRE(dynamic_cast<cloudsim::PhysicalNode*>(*(p.nodes[3]))->group() == 1);
		REQUIRE(dynamic_cast<cloudsim::PhysicalNode*>(*(p.nodes[5]))->group() == 2);
	}
}
/*
TEST_CASE("PhysicalNetwork::node_distribution(std::function f)", "[PhysicalNetwork]")
{
	cloudsim::PhysicalNetwork p("test/test_physical.json");
	block_resources(p);

	std::vector<double> d = p.node_distribution<double>([](cloudsim::PhysicalNode* n) -> double {
		return n->capacity(0);
	});

	REQUIRE(d[0] == Approx(600));
}

TEST_CASE("PhysicalNetwork::node_distribution(f, type = 0)", "[PhysicalNetwork]")
{
	cloudsim::PhysicalNetwork p("test/test_physical.json");
	block_resources(p);

	std::vector<double> d = p.node_distribution<double>([](cloudsim::PhysicalNode* n) -> double {
		return n->capacity(0);
	}, 0);

	REQUIRE(d[0] == Approx(600));
}

TEST_CASE("PhysicalNetwork::edge_distribution(std::function f)", "[PhysicalNetwork]")
{
	cloudsim::PhysicalNetwork p("test/test_physical.json");
	block_resources(p);
}
*/

TEST_CASE("PhysicalNetwork::count_nodes(node_type)", "[PhysicalNetwork]")
{
	cloudsim::PhysicalNetwork p("test/test_physical.json");
	REQUIRE(p.count_nodes(0) == 3);
	REQUIRE(p.count_nodes(1) == 4);
}

TEST_CASE("PhysicalNetwork::total_node_capacity(resource_type)", "[PhysicalNetwork]")
{
	cloudsim::PhysicalNetwork p("test/test_physical.json");
	REQUIRE(p.total_node_capacity(0) == Approx(700.0));
	REQUIRE(p.total_node_capacity(1) == Approx(1600.0));
}

TEST_CASE("PhysicalNetwork::total_node_capacity(resource_type, node_type)", "[PhysicalNetwork]")
{
	cloudsim::PhysicalNetwork p("test/test_physical.json");
	REQUIRE(p.total_node_capacity(0, 0) == Approx(300.0));
	REQUIRE(p.total_node_capacity(0, 1) == Approx(400.0));
	REQUIRE(p.total_node_capacity(1, 0) == Approx(1200.0));
	REQUIRE(p.total_node_capacity(1, 1) == Approx(400.0));
}

TEST_CASE("PhysicalNetwork::total_edge_capacity(resource_type)", "[PhysicalNetwork]")
{
	cloudsim::PhysicalNetwork p("test/test_physical.json");
	REQUIRE(p.total_edge_capacity(0) == Approx(800.0));
}

TEST_CASE("PhysicalNetwork::total_node_utilization(resource_type)", "[PhysicalNetwork]")
{
	cloudsim::PhysicalNetwork p("test/test_physical.json");
	block_resources(p);

	REQUIRE(p.total_node_utilization(0) == Approx(350));
	REQUIRE(p.total_node_utilization(1) == Approx(350));
}

TEST_CASE("PhysicalNetwork::total_node_utilization(resource_type, node_type)", "[PhysicalNetwork]")
{
	cloudsim::PhysicalNetwork p("test/test_physical.json");
	block_resources(p);

	REQUIRE(p.total_node_utilization(0, 0) == Approx(150));
	REQUIRE(p.total_node_utilization(1, 0) == Approx(150));
	REQUIRE(p.total_node_utilization(0, 1) == Approx(200));
	REQUIRE(p.total_node_utilization(1, 1) == Approx(200));
}

TEST_CASE("PhysicalNetwork::total_edge_utilization(resource_type)", "[PhysicalNetwork]")
{
	cloudsim::PhysicalNetwork p("test/test_physical.json");
	block_resources(p);

	REQUIRE(p.total_edge_utilization(0) == Approx(300));
}

TEST_CASE("PhysicalNetwork::rel_node_utilization(resource_type)", "[PhysicalNetwork]")
{
	cloudsim::PhysicalNetwork p("test/test_physical.json");
	block_resources(p);

	REQUIRE(p.rel_node_utilization(0) == Approx(0.5)); // 350 / 700
	REQUIRE(p.rel_node_utilization(1) == Approx(0.218).epsilon(0.01)); // 350 / 1600
}

TEST_CASE("PhysicalNetwork::rel_node_utilization(resource_type, node_type)", "[PhysicalNetwork]")
{
	cloudsim::PhysicalNetwork p("test/test_physical.json");
	block_resources(p);

	REQUIRE(p.rel_node_utilization(0, 0) == Approx(0.5)); // 150 / 300
	REQUIRE(p.rel_node_utilization(0, 1) == Approx(0.5)); // 200 / 400
	REQUIRE(p.rel_node_utilization(1, 0) == Approx(0.125)); // 150 / 1200
	REQUIRE(p.rel_node_utilization(1, 1) == Approx(0.5)); // 200 / 400
}

TEST_CASE("PhysicalNetwork::rel_edge_utilization(resource_type)", "[PhysicalNetwork]")
{
	cloudsim::PhysicalNetwork p("test/test_physical.json");
	block_resources(p);

	REQUIRE(p.rel_edge_utilization(0) == Approx(0.375).epsilon(0.01)); // 300 / 900
}

TEST_CASE("PhysicalNetwork::avg_node_utilization(unsigned long resource_type)", "[PhysicalNetwork]")
{
	cloudsim::PhysicalNetwork p("test/test_physical.json");
	block_resources(p);

	REQUIRE(p.avg_node_utilization(0) == Approx(50));
	REQUIRE(p.avg_node_utilization(1) == Approx(50));
}

TEST_CASE("PhysicalNetwork::avg_node_utilization(resource_type, node_type)", "[PhysicalNetwork]")
{
	cloudsim::PhysicalNetwork p("test/test_physical.json");
	block_resources(p);

	REQUIRE(p.avg_node_utilization(0, 0) == Approx(50));
	REQUIRE(p.avg_node_utilization(1, 1) == Approx(50));
}

TEST_CASE("PhysicalNetwork::avg_edge_utilization(resource_type)", "[PhysicalNetwork]")
{
	cloudsim::PhysicalNetwork p("test/test_physical.json");
	block_resources(p);

	REQUIRE(p.avg_edge_utilization(0) == Approx(50));
}

TEST_CASE("PhysicalNetwork::total_node_residual(resource_type)", "[PhysicalNetwork]")
{
	cloudsim::PhysicalNetwork p("test/test_physical.json");
	block_resources(p);

	REQUIRE(p.total_node_residual(0) == Approx(350));
	REQUIRE(p.total_node_residual(1) == Approx(1250));
}

TEST_CASE("PhysicalNetwork::total_node_residual(resource_type, node_type)", "[PhysicalNetwork]")
{
	cloudsim::PhysicalNetwork p("test/test_physical.json");
	block_resources(p);

	REQUIRE(p.total_node_residual(0, 0) == Approx(150));
	REQUIRE(p.total_node_residual(0, 1) == Approx(200));
	REQUIRE(p.total_node_residual(1, 0) == Approx(1050));
	REQUIRE(p.total_node_residual(1, 1) == Approx(200));
}

TEST_CASE("PhysicalNetwork::total_edge_residual(resource_type)", "[PhysicalNetwork]")
{
	cloudsim::PhysicalNetwork p("test/test_physical.json");
	block_resources(p);

	REQUIRE(p.total_edge_residual(0) == Approx(500));
}

TEST_CASE("PhysicalNetwork::avg_node_residual(resource_type)", "[PhysicalNetwork]")
{
	cloudsim::PhysicalNetwork p("test/test_physical.json");
	block_resources(p);

	REQUIRE(p.avg_node_residual(0) == Approx(50));
	REQUIRE(p.avg_node_residual(1) == Approx(178.5).epsilon(0.5));
}

TEST_CASE("PhysicalNetwork::avg_node_residual(resource_type, node_type)", "[PhysicalNetwork]")
{
	cloudsim::PhysicalNetwork p("test/test_physical.json");
	block_resources(p);

	REQUIRE(p.avg_node_residual(0, 0) == Approx(50));
	REQUIRE(p.avg_node_residual(0, 1) == Approx(50));
	REQUIRE(p.avg_node_residual(1, 0) == Approx(350));
	REQUIRE(p.avg_node_residual(1, 1) == Approx(50));
}

TEST_CASE("PhysicalNetwork::avg_edge_residual(resource_type)", "[PhysicalNetwork]")
{
	cloudsim::PhysicalNetwork p("test/test_physical.json");
	block_resources(p);

	REQUIRE(p.avg_edge_residual(0) == Approx(83));
}

TEST_CASE("PhysicalNetwork::node_utilization_variance(resource_type)", "[PhysicalNetwork]")
{
	cloudsim::PhysicalNetwork p("test/test_physical.json");
	block_resources(p);

	//TODO: TEST_CASE("PhysicalNetwork::node_utilization_variance(resource_type)
}

TEST_CASE("PhysicalNetwork::node_utilization_variance(resource_type, node_type)", "[PhysicalNetwork]")
{
	cloudsim::PhysicalNetwork p("test/test_physical.json");
	block_resources(p);

	//TODO: TEST_CASE("PhysicalNetwork::node_utilization_variance(resource_type, node_type)
}

TEST_CASE("PhysicalNetwork::edge_utilization_variance(resource_type)", "[PhysicalNetwork]")
{
	cloudsim::PhysicalNetwork p("test/test_physical.json");
	block_resources(p);

	//TODO: TEST_CASE("PhysicalNetwork::edge_utilization_variance(resource_type)
}

TEST_CASE("PhysicalNetwork::rel_node_utilization_variance(resource_type)", "[PhysicalNetwork]")
{
	cloudsim::PhysicalNetwork p("test/test_physical.json");
	block_resources(p);

	//TODO: TEST_CASE("PhysicalNetwork::rel_node_utilization_variance(resource_type)
}

TEST_CASE("PhysicalNetwork::rel_node_utilization_variance(resource_type, node_type)", "[PhysicalNetwork]")
{
	cloudsim::PhysicalNetwork p("test/test_physical.json");
	block_resources(p);

	//TODO: TEST_CASE("PhysicalNetwork::rel_node_utilization_variance(resource_type, node_type)
}

TEST_CASE("PhysicalNetwork::rel_edge_utilization_variance(resource_type)", "[PhysicalNetwork]")
{
	cloudsim::PhysicalNetwork p("test/test_physical.json");
	block_resources(p);

	//TODO: TEST_CASE("PhysicalNetwork::rel_edge_utilization_variance(resource_type)
}

TEST_CASE("PhysicalNetwork::node_utilization_distribution(resource_type)", "[PhysicalNetwork]")
{
	cloudsim::PhysicalNetwork p("test/test_physical.json");
	block_resources(p);

	std::vector<unsigned> d = p.node_utilization_distribution(0);

	REQUIRE(d.size() == 7);
	REQUIRE(d[0] == Approx(50.0));
	REQUIRE(d[6] == Approx(50.0));
}

TEST_CASE("PhysicalNetwork::node_utilization_distribution(resource_type, node_type)",
		  "[PhysicalNetwork]")
{
	cloudsim::PhysicalNetwork p("test/test_physical.json");
	block_resources(p);

	std::vector<unsigned> d = p.node_utilization_distribution(0, 1);

	REQUIRE(d.size() == 4);
	REQUIRE(d[0] == Approx(50.0));
	REQUIRE(d[3] == Approx(50.0));
}

TEST_CASE("PhysicalNetwork::edge_utilization_distribution(resource_type)", "[PhysicalNetwork]")
{
	cloudsim::PhysicalNetwork p("test/test_physical.json");
	block_resources(p);

	std::vector<unsigned> d = p.edge_utilization_distribution(0);

	REQUIRE(d.size() == 6);
	REQUIRE(d[0] == Approx(50.0));
	REQUIRE(d[5] == Approx(50.0));
}
/*
TEST_CASE("PhysicalNetwork::node_residual_distribution(resource_type)", "[PhysicalNetwork]")
{
	cloudsim::PhysicalNetwork p("test/test_physical.json");
	block_resources(p);
}

TEST_CASE("PhysicalNetwork::node_residual_distribution(resource_type, node_type)", "[PhysicalNetwork]")
{
	cloudsim::PhysicalNetwork p("test/test_physical.json");
	block_resources(p);
}

TEST_CASE("PhysicalNetwork::edge_residual_distribution(resource_type)", "[PhysicalNetwork]")
{
	cloudsim::PhysicalNetwork p("test/test_physical.json");
	block_resources(p);
}
*/
TEST_CASE("PhysicalNetwork::rel_node_utilization_distribution(resource_type)", "[PhysicalNetwork]")
{
	cloudsim::PhysicalNetwork p("test/test_physical.json");
	block_resources(p);

	std::vector<double> d = p.rel_node_utilization_distribution(0);

	REQUIRE(d.size() == 7);
	REQUIRE(d[0] == Approx(0.5));
	REQUIRE(d[6] == Approx(0.5));
}

TEST_CASE("PhysicalNetwork::rel_node_utilization_distribution(resource_type, node_type)",
		  "[PhysicalNetwork]")
{
	cloudsim::PhysicalNetwork p("test/test_physical.json");
	block_resources(p);

	std::vector<double> d = p.rel_node_utilization_distribution(0, 1);

	REQUIRE(d.size() == 4);
	REQUIRE(d[0] == Approx(0.5));
	REQUIRE(d[3] == Approx(0.5));
}

TEST_CASE("PhysicalNetwork::rel_edge_utilization_distribution(resource_type)", "[PhysicalNetwork]")
{
	cloudsim::PhysicalNetwork p("test/test_physical.json");
	block_resources(p);

	std::vector<double> d = p.rel_edge_utilization_distribution(0);

	REQUIRE(d.size() == 6);
	REQUIRE(d[0] == Approx(0.25));
	REQUIRE(d[5] == Approx(0.5));
}

TEST_CASE("PhysicalNetwork::distance_matrix()", "[PhysicalNetwork]")
{
	cloudsim::PhysicalNetwork p("test/test_physical.json");

	auto d = p.distance_matrix();

	SECTION("returns a NxN matrix")
	{
		REQUIRE(d.size() == p.nodes.count());
		REQUIRE(d[0].size() == p.nodes.count());
	}

	SECTION("returns a matrix with correct hop counts")
	{
		REQUIRE(d[0][1] == 1);
		REQUIRE(d[0][3] == 2);
		REQUIRE(d[1][2] == 2);
		REQUIRE(d[3][6] == 4);
		REQUIRE(d[1][0] == 1);
		REQUIRE(d[3][0] == 2);
		REQUIRE(d[5][6] == 2);
	}
}

TEST_CASE("PhysicalNetwork::diameter()", "[PhysicalNetwork]")
{
	cloudsim::PhysicalNetwork p("test/test_physical.json");

	REQUIRE(p.diameter() == 4);
}

TEST_CASE("PhysicalNetwork::add_virtual_network()", "[PhysicalNetwork]")
{
	cloudsim::PhysicalNetwork p("test/test_physical.json");
	cloudsim::VirtualNetwork v("test/test_virtual1.json");

	cloudsim::Mapping m;

	m.switches.add(0, 1);
	m.servers.add(1, 3);
	m.servers.add(2, 3);
	m.servers.add(3, 4);
	m.links.add(0, {2});
	m.links.add(1, {2});
	m.links.add(2, {3});

	SECTION("should not allow to map the same network twice")
	{
		REQUIRE_NOTHROW(p.add_virtual_network(&v, m));
		REQUIRE_THROWS(p.add_virtual_network(&v, m));
	}

	SECTION("should map the nodes and edges")
	{
		p.add_virtual_network(&v, m);

		REQUIRE(p.has_subgraphs());
		REQUIRE(p.has_subgraph(&v));

		REQUIRE(p.nodes[1].has_subvertices());
		REQUIRE(p.nodes[3].has_subvertices());
		REQUIRE(p.nodes[4].has_subvertices());
		REQUIRE(p.edges[2].has_subedges());
		REQUIRE(p.edges[3].has_subedges());

		REQUIRE(v.nodes[0].has_supervertex());
		REQUIRE(v.nodes[1].has_supervertex());
		REQUIRE(v.nodes[2].has_supervertex());
		REQUIRE(v.nodes[3].has_supervertex());
		REQUIRE(v.edges[0].has_superedge());
		REQUIRE(v.edges[1].has_superedge());
		REQUIRE(v.edges[2].has_superedge());
	}

	SECTION("should reserve resources on nodes and edges")
	{
		auto pn1 = dynamic_cast<cloudsim::PhysicalNode*>(*p.nodes[1]);
		auto pn3 = dynamic_cast<cloudsim::PhysicalNode*>(*p.nodes[3]);
		auto pn4 = dynamic_cast<cloudsim::PhysicalNode*>(*p.nodes[4]);
		auto pe2 = dynamic_cast<cloudsim::PhysicalEdge*>(*p.edges[2]);
		auto pe3 = dynamic_cast<cloudsim::PhysicalEdge*>(*p.edges[3]);

		pn1->reserve(50, 0);
		pn3->reserve(80, 0);
		pn4->reserve(40, 0);
		pe2->reserve(70, 0);
		pe3->reserve(35, 0);

		p.add_virtual_network(&v, m);

		REQUIRE(pn1->utilization(0) == Approx(50.0));
		REQUIRE(pn3->utilization(0) == Approx(80.0));
		REQUIRE(pn4->utilization(0) == Approx(40.0));
		REQUIRE(pe2->utilization(0) == Approx(70.0));
		REQUIRE(pe3->utilization(0) == Approx(35.0));
	}
}

TEST_CASE("PhysicalNetwork::remove_network()", "[PhysicalNetwork]")
{
	cloudsim::PhysicalNetwork p("test/test_physical.json");
	cloudsim::VirtualNetwork v("test/test_virtual1.json");

	cloudsim::Mapping m;

	m.switches.add(0, 1);
	m.servers.add(1, 3);
	m.servers.add(2, 3);
	m.servers.add(3, 4);
	m.links.add(0, {2});
	m.links.add(1, {2});
	m.links.add(2, {3});

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

	SECTION("should throw an exception when the network is not embedded")
	{
		REQUIRE_THROWS(p.remove_virtual_network(&v));
		REQUIRE_NOTHROW(p.add_virtual_network(&v, m));
		REQUIRE(p.has_subgraph(&v));
		REQUIRE_NOTHROW(p.remove_virtual_network(&v));
	}

	SECTION("should remove the network mapping")
	{
		p.add_virtual_network(&v, m);
		p.remove_virtual_network(&v);

		REQUIRE_FALSE(p.has_subgraphs());
		REQUIRE_FALSE(p.has_subgraph(&p));

		REQUIRE_FALSE(p.nodes[1].has_subvertices());
		REQUIRE_FALSE(p.nodes[3].has_subvertices());
		REQUIRE_FALSE(p.nodes[4].has_subvertices());
		REQUIRE_FALSE(p.edges[2].has_subedges());
		REQUIRE_FALSE(p.edges[3].has_subedges());

		REQUIRE_FALSE(v.nodes[0].has_supervertex());
		REQUIRE_FALSE(v.nodes[1].has_supervertex());
		REQUIRE_FALSE(v.nodes[2].has_supervertex());
		REQUIRE_FALSE(v.nodes[3].has_supervertex());
		REQUIRE_FALSE(v.edges[0].has_superedge());
		REQUIRE_FALSE(v.edges[1].has_superedge());
		REQUIRE_FALSE(v.edges[2].has_superedge());

	}

	SECTION("should free all previously allocated resources")
	{
		p.add_virtual_network(&v, m);
		p.remove_virtual_network(&v);
		REQUIRE(pn1->utilization(0) == Approx(0.0));
		REQUIRE(pn3->utilization(0) == Approx(0.0));
		REQUIRE(pn4->utilization(0) == Approx(0.0));
		REQUIRE(pe2->utilization(0) == Approx(0.0));
		REQUIRE(pe3->utilization(0) == Approx(0.0));

	}
}

TEST_CASE("PhysicalNetwork::virtual_network()", "[PhysicalNetwork]")
{
	//TODO: TEST_CASE("PhysicalNetwork::virtual_network()")
}

TEST_CASE("PhysicalNetwork::virtual_networks()", "[PhysicalNetwork]")
{
	//TODO: TEST_CASE("PhysicalNetwork::virtual_networks()")
}

TEST_CASE("PhysicalNetwork::avg_path_length_distribution()", "[PhysicalNetwork]")
{
	//TODO: TEST_CASE("PhysicalNetwork::avg_path_length_distribution()")
}
