#include <catch/catch.h>

#include "cloudsim/request.h"

TEST_CASE("Request::Request(unsigned long)", "[Request]")
{
	cloudsim::Request r(2323245);
	REQUIRE(r.id() == 2323245);
}

TEST_CASE("Request::id()", "[Request]")
{
	cloudsim::Request r(2323245);
	REQUIRE(r.id() == 2323245);
}
