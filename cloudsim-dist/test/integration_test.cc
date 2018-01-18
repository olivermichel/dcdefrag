
#include <catch/catch.h>
#include <cloudsim/physical_network.h>
/*
SCENARIO("network configurations can be saved and restored")
{
	cloudsim::PhysicalNetwork p1("test/test_physical.json");
	cloudsim::PhysicalNetwork p2("test/test_physical.json");
	cloudsim::VirtualNetwork v1("test/test_virtual1.json");
	cloudsim::VirtualNetwork v2("test/test_virtual2.json");

	SECTION("the networks are correctly initialzed")
	{
		REQUIRE(p1.nodes.count() == 7);
		REQUIRE(p1.edges.count() == 6);
		REQUIRE(p2.nodes.count() == 7);
		REQUIRE(p2.edges.count() == 6);
		REQUIRE(v1.nodes.count() == 4);
		REQUIRE(v1.edges.count() == 3);
		REQUIRE(v2.nodes.count() == 4);
		REQUIRE(v2.edges.count() == 3);

		REQUIRE(p1.total_node_utilization(0, 0) == 0);
		REQUIRE(p1.total_node_utilization(0, 1) == 0);
		REQUIRE(p1.total_node_utilization(1, 0) == 0);
		REQUIRE(p1.total_node_utilization(1, 1) == 0);
		REQUIRE(p1.total_edge_utilization(0) == 0);

		REQUIRE(p2.total_node_utilization(0, 0) == 0);
		REQUIRE(p2.total_node_utilization(0, 1) == 0);
		REQUIRE(p2.total_node_utilization(1, 0) == 0);
		REQUIRE(p2.total_node_utilization(1, 1) == 0);
		REQUIRE(p2.total_edge_utilization(0) == 0);
	}

	SECTION("the networks can sequentially be mapped")
	{
		cloudsim::Mapping m1;
		m1.switches.add(0, 1);
		m1.servers.add(1, 3);
		m1.servers.add(2, 3);
		m1.servers.add(3, 4);
		m1.links.add(0, {2});
		m1.links.add(1, {2});
		m1.links.add(2, {3});

		cloudsim::Mapping m2;
		m2.switches.add(0, 2);
		m2.servers.add(1, 5);
		m2.servers.add(2, 5);
		m2.servers.add(3, 6);
		m2.links.add(0, {4});
		m2.links.add(1, {4});
		m2.links.add(2, {5});

		REQUIRE_NOTHROW(p1.add_virtual_network(&v1, m1));
		REQUIRE_NOTHROW(p1.add_virtual_network(&v2, m2));

		REQUIRE(p1.subgraphs().size() == 2);
		REQUIRE(p1.subgraphs()[0] == &v1);
		REQUIRE(p1.subgraphs()[1] == &v2);

		REQUIRE(p1.nodes[0].sub_vertices().size() == 0);
		REQUIRE(p1.nodes[1].sub_vertices().size() == 1);
		REQUIRE(p1.nodes[2].sub_vertices().size() == 1);

		REQUIRE(p1.nodes[3].sub_vertices().size() == 2);
		REQUIRE(p1.nodes[4].sub_vertices().size() == 1);
		REQUIRE(p1.nodes[5].sub_vertices().size() == 2);
		REQUIRE(p1.nodes[6].sub_vertices().size() == 1);

		REQUIRE(p1.edges[0].sub_edges().size() == 0);
		REQUIRE(p1.edges[1].sub_edges().size() == 0);

		REQUIRE(p1.edges[2].sub_edges().size() == 2);
		REQUIRE(p1.edges[3].sub_edges().size() == 1);
		REQUIRE(p1.edges[4].sub_edges().size() == 2);
		REQUIRE(p1.edges[5].sub_edges().size() == 1);

		REQUIRE(dynamic_cast<cloudsim::PhysicalNode*>(*(p1.nodes[0]))->residual(0) == 100);
		REQUIRE(dynamic_cast<cloudsim::PhysicalNode*>(*(p1.nodes[0]))->residual(1) == 600);
		REQUIRE(dynamic_cast<cloudsim::PhysicalNode*>(*(p1.nodes[1]))->residual(0) == 50);
		REQUIRE(dynamic_cast<cloudsim::PhysicalNode*>(*(p1.nodes[1]))->residual(1) == 250);
		REQUIRE(dynamic_cast<cloudsim::PhysicalNode*>(*(p1.nodes[2]))->residual(0) == 50);
		REQUIRE(dynamic_cast<cloudsim::PhysicalNode*>(*(p1.nodes[2]))->residual(1) == 250);
		REQUIRE(dynamic_cast<cloudsim::PhysicalNode*>(*(p1.nodes[3]))->residual(0) == 20);
		REQUIRE(dynamic_cast<cloudsim::PhysicalNode*>(*(p1.nodes[3]))->residual(1) == 20);
		REQUIRE(dynamic_cast<cloudsim::PhysicalNode*>(*(p1.nodes[4]))->residual(0) == 60);
		REQUIRE(dynamic_cast<cloudsim::PhysicalNode*>(*(p1.nodes[4]))->residual(1) == 60);
		REQUIRE(dynamic_cast<cloudsim::PhysicalNode*>(*(p1.nodes[5]))->residual(0) == 20);
		REQUIRE(dynamic_cast<cloudsim::PhysicalNode*>(*(p1.nodes[5]))->residual(1) == 20);
		REQUIRE(dynamic_cast<cloudsim::PhysicalNode*>(*(p1.nodes[6]))->residual(0) == 60);
		REQUIRE(dynamic_cast<cloudsim::PhysicalNode*>(*(p1.nodes[6]))->residual(1) == 60);

		REQUIRE(dynamic_cast<cloudsim::PhysicalEdge*>(*(p1.edges[0]))->residual(0) == 200);
		REQUIRE(dynamic_cast<cloudsim::PhysicalEdge*>(*(p1.edges[1]))->residual(0) == 200);
		REQUIRE(dynamic_cast<cloudsim::PhysicalEdge*>(*(p1.edges[2]))->residual(0) == 30);
		REQUIRE(dynamic_cast<cloudsim::PhysicalEdge*>(*(p1.edges[3]))->residual(0) == 65);
		REQUIRE(dynamic_cast<cloudsim::PhysicalEdge*>(*(p1.edges[4]))->residual(0) == 30);
		REQUIRE(dynamic_cast<cloudsim::PhysicalEdge*>(*(p1.edges[5]))->residual(0) == 65);

		REQUIRE_NOTHROW(p1.save_configuration("test/test_configuration.json"));

		std::list<cloudsim::VirtualNetwork*> networks;

		REQUIRE_NOTHROW(networks = p2.load_configuration("test/test_configuration.json"));

		REQUIRE(networks.size() == 2);

		REQUIRE(p1.total_node_utilization(0, 0) == p2.total_node_utilization(0, 0));
		REQUIRE(p1.total_node_utilization(0, 1) == p2.total_node_utilization(0, 1));
		REQUIRE(p1.total_node_utilization(1, 0) == p2.total_node_utilization(1, 0));
		REQUIRE(p1.total_node_utilization(1, 1) == p2.total_node_utilization(1, 1));
		REQUIRE(p1.total_edge_utilization(0)    == p2.total_edge_utilization(0));
	}
}
*/