
#include <catch/catch.h>

#include "cloudsim/virtual_resource.h"

TEST_CASE("VirtualResource::VirtualResource(double, group)", "[VirtualResource]")
{
	cloudsim::VirtualResource r(4, 0);
	REQUIRE(r.demand() == Approx(4));
	REQUIRE(r.group() == 0);
}

TEST_CASE("VirtualResource::demand()", "[VirtualResource]")
{
	cloudsim::VirtualResource r(4, 0);
	REQUIRE(r.demand() == Approx(4));
}

TEST_CASE("VirtualResource::group()", "[VirtualResource]")
{
	cloudsim::VirtualResource r(4, 0);
	REQUIRE(r.group() == 0);
}
