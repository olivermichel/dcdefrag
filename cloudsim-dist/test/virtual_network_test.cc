#include <catch/catch.h>

#include "cloudsim/virtual_network.h"

TEST_CASE("VirtualNetwork::VirtualNetwork(id, group)", "[VirtualNetwork]")
{
	cloudsim::VirtualNetwork v(0, 0);

	SECTION("sets the virtual network id")
	{
		REQUIRE(v.id() == 0);
	}

	SECTION("sets the virtual network type")
	{
		REQUIRE(v.type() == 0);
	}

	SECTION("the network is empty upon construction")
	{
		REQUIRE(v.nodes.count() == 0);
		REQUIRE(v.edges.count() == 0);
	}
}

TEST_CASE("VirtualNetwork::VirtualNetwork(file_name)", "[VirtualNetwork]")
{
	SECTION("the network can be constructed from a json file") {

		cloudsim::VirtualNetwork v("test/test_virtual1.json");
		REQUIRE(v.id() == 1);
		REQUIRE(v.type() == 0);
		REQUIRE(v.nodes.count() == 4);
		REQUIRE(v.edges.count() == 3);
		REQUIRE(dynamic_cast<cloudsim::VirtualNode*>(*(v.nodes[0]))->demand() == Approx(50.0));
		REQUIRE(dynamic_cast<cloudsim::VirtualNode*>(*(v.nodes[1]))->demand() == Approx(40.0));
		REQUIRE(dynamic_cast<cloudsim::VirtualEdge*>(*(v.edges[0]))->demand() == Approx(35.0));

		REQUIRE(dynamic_cast<cloudsim::VirtualNode*>(*(v.nodes[0]))->group() == 0);
		REQUIRE(dynamic_cast<cloudsim::VirtualNode*>(*(v.nodes[1]))->group() == 1);
	}
}

TEST_CASE("VirtualNetwork::total_node_demand(type, node_type)", "[VirtualNetwork]")
{
	cloudsim::VirtualNetwork v("test/test_virtual1.json");

	REQUIRE(v.total_node_demand(0, 0) == Approx(50.0));
	REQUIRE(v.total_node_demand(1, 0) == Approx(120.0));
	REQUIRE(v.total_node_demand(0, 1) == Approx(50.0));
	REQUIRE(v.total_node_demand(1, 1) == Approx(120.0));
}

TEST_CASE("VirtualNetwork::total_edge_demand(type)", "[VirtualNetwork]")
{
	cloudsim::VirtualNetwork v("test/test_virtual1.json");

	REQUIRE(v.total_edge_demand(0) == Approx(105.0));
}

TEST_CASE("VirtualNetwork::add_mapping(mapping)", "[VirtualNetwork]")
{
	cloudsim::VirtualNetwork v("test/test_virtual1.json");

	cloudsim::Mapping m1;

	m1.switches.add(0, 1);
	m1.servers.add(1, 3);
	m1.links.add(0, {2});

	cloudsim::Mapping m2;

	m2.switches.add(0, 1);
	m2.servers.add(1, 3);
	m2.links.add(0, {2});

	REQUIRE_NOTHROW(v.add_mapping(m1));
	REQUIRE_NOTHROW(v.add_mapping(m2));
}

TEST_CASE("VirtualNetwork::mappings()", "[VirtualNetwork]")
{
	cloudsim::VirtualNetwork v("test/test_virtual1.json");

	cloudsim::Mapping m1;

	m1.switches.add(0, 1);
	m1.servers.add(1, 3);
	m1.links.add(0, {2});

	cloudsim::Mapping m2;

	m2.switches.add(0, 1);
	m2.servers.add(1, 3);
	m2.links.add(0, {2});

	v.add_mapping(m1);
	v.add_mapping(m2);

	REQUIRE(v.mappings().size() == 2);
}
