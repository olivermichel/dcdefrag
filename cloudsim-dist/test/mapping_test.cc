
#include <catch/catch.h>

#include "cloudsim/mapping.h"

TEST_CASE("Mapping::Mapping()", "[Mapping]")
{
	SECTION("is empty upon construction")
	{
		cloudsim::Mapping m;

		REQUIRE(m.servers.size() == 0);
		REQUIRE(m.switches.size() == 0);
		REQUIRE(m.links.size() == 0);

		REQUIRE(m.servers.empty());
		REQUIRE(m.switches.empty());
		REQUIRE(m.links.empty());
	}
}

TEST_CASE("Mapping::_map_proxy::add()", "[Mapping]")
{
	SECTION("adds servers and rejects duplicate keys")
	{
		cloudsim::Mapping m;
		REQUIRE_NOTHROW(m.servers.add(1, 2));
		REQUIRE_NOTHROW(m.servers.add(2, 3));
		REQUIRE_THROWS(m.servers.add(1, 5));
		REQUIRE(m.servers.size() == 2);
	}

	SECTION("adds switches and rejects duplicate keys")
	{
		cloudsim::Mapping m;
		REQUIRE_NOTHROW(m.switches.add(1, 2));
		REQUIRE_NOTHROW(m.switches.add(2, 3));
		REQUIRE_THROWS(m.switches.add(1, 5));
		REQUIRE(m.switches.size() == 2);
	}

	SECTION("adds links and rejects duplicate keys")
	{
		cloudsim::Mapping m;
		REQUIRE_NOTHROW(m.links.add(1, {2}));
		REQUIRE_NOTHROW(m.links.add(2, {2, 3}));
		REQUIRE_THROWS(m.links.add(1, {4,5}));
		REQUIRE(m.links.size() == 2);
	}
}

TEST_CASE("Mapping::_map_proxy::has()", "[Mapping]")
{
	cloudsim::Mapping m;
	REQUIRE_NOTHROW(m.servers.add(1, 2));
	REQUIRE_NOTHROW(m.switches.add(1, 2));
	REQUIRE_NOTHROW(m.links.add(1, {2}));
	REQUIRE_NOTHROW(m.links.add(2, {2}));

	REQUIRE(m.servers.has(1));
	REQUIRE(!m.servers.has(2));
	REQUIRE(m.switches.has(1));
	REQUIRE(m.links.has(1));
	REQUIRE(!m.links.has(3));
}

TEST_CASE("Mapping::_map_proxy::operator[]()", "[Mapping]")
{
	cloudsim::Mapping m;

	REQUIRE_NOTHROW(m.servers.add(1, 2));
	REQUIRE_NOTHROW(m.switches.add(1, 2));
	REQUIRE_NOTHROW(m.links.add(1, {2}));
	REQUIRE_NOTHROW(m.links.add(2, {2}));

	REQUIRE(m.servers[1] == 2);
	REQUIRE(m.switches[1] == 2);
	REQUIRE(m.links[1] == std::vector<unsigned long>{2});
	REQUIRE(m.links[2] == std::vector<unsigned long>{2});

	REQUIRE_THROWS(m.servers[0]);
	REQUIRE_THROWS(m.switches[5]);
	REQUIRE_THROWS(m.links[4]);
}

TEST_CASE("Mapping::avg_path_length()", "[Mapping]")
{
	cloudsim::Mapping m;
	m.links.add(1, {2, 3, 4});
	m.links.add(2, {2, 3});
	m.links.add(3, {2});

	REQUIRE(m.avg_path_length() == Approx(2.0));
}

TEST_CASE("Mapping::path_length_distribution()", "[Mapping]")
{
	cloudsim::Mapping m;
	m.links.add(1, {2, 3, 4});
	m.links.add(2, {2, 3});
	m.links.add(3, {2});

	std::vector<double> dist = m.path_length_distribution();

	REQUIRE(dist.size() == 3);
	REQUIRE(dist[0] == 3);
	REQUIRE(dist[1] == 2);
	REQUIRE(dist[2] == 1);
}