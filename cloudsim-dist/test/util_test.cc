
#include <catch/catch.h>
#include <vector>

#include "cloudsim/util.h"

TEST_CASE("util::mean(data)", "[util]")
{
	std::vector<double> data_double = { 0.4 , 1 , 2.0, 5.8, 8.2, 8, 9.8, 10.1, 3.2, 5};
	std::vector<int> data_int = { 3, 2, 5, 9, 7};

	REQUIRE(cloudsim::util::mean<double>(data_double) == Approx(5.35));
	REQUIRE(cloudsim::util::mean<int>(data_int) == Approx(5.2));
}

TEST_CASE("util::variance(data)", "[util]")
{
	std::vector<double> data_double = { 0.4 , 1 , 2.0, 5.8, 8.2, 8, 9.8, 10.1, 3.2, 5};
	std::vector<int> data_int = { 3, 2, 5, 9, 7};

	REQUIRE(cloudsim::util::variance<double>(data_double) == Approx(11.7105));
	REQUIRE(cloudsim::util::variance<int>(data_int) == Approx(6.0));
}
