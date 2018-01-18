
#include <catch/catch.h>

#include "cloudsim/physical_resource.h"

TEST_CASE("PhysicalResource::PhysicalResource(capacity, group)", "[PhysicalResource]")
{
	cloudsim::PhysicalResource r(4, 0);
	REQUIRE(r.capacity() == 4);
	REQUIRE(r.group() == 0);
}

TEST_CASE("PhysicalResource::capacity()", "[PhysicalResource]")
{
	cloudsim::PhysicalResource r(4, 0);
	REQUIRE(r.capacity() == 4);
}

TEST_CASE("PhysicalResource::group()", "[PhysicalResource]")
{
	cloudsim::PhysicalResource r(4, 0);
	REQUIRE(r.group() == 0);
}
