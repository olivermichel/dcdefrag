
#include <catch/catch.h>

#include <vector>
#include <initializer_list>

#include "cloudsim/reservable.h"
#include "cloudsim/logic_error.h"

using namespace cloudsim::interfaces;

class test_reservable : public Reservable {
public:
	test_reservable(unsigned w) : Reservable(w) { }
	test_reservable(std::initializer_list<unsigned> w) : Reservable(w) { }
	test_reservable(std::vector<unsigned> w) : Reservable(w) { }
};

TEST_CASE("Reservable::Reservable(double w)", "[Reservable]")
{
	test_reservable t(12);

	SECTION("properties are initialized correctly") {
		REQUIRE(t.residual() == Approx(12));
		REQUIRE(t.utilization() == Approx(0));
		REQUIRE(t.reservation() == Approx(0));
	}
}

TEST_CASE("Reservable::reserve(double w)", "[Reservable]")
{
	SECTION("reserving more than available throws an exception")
	{
		test_reservable t(24);
		REQUIRE_THROWS(t.reserve(26));
		REQUIRE(t.reservation() == Approx(0.0));
	}

	SECTION("resources can be reserved")
	{
		test_reservable t(24);
		REQUIRE_NOTHROW(t.reserve(12));
		REQUIRE(t.reservation() == Approx(12));
	}

	SECTION("resources can be repeatedly reserved")
	{
		test_reservable t(24);
		REQUIRE_NOTHROW(t.reserve(12));
		REQUIRE(t.reservation() == Approx(12));
		REQUIRE_NOTHROW(t.reserve(8));
		REQUIRE(t.reservation() == Approx(20));
	}
}

TEST_CASE("Reservable::release(double w)", "[Reservable]")
{
	SECTION("throws an exception when trying to release more than is blocked") {
		test_reservable t(5);
		REQUIRE_THROWS(t.release(3));
	}

	SECTION("releases utilization") {
		test_reservable t(2);
		t.block(2);
		t.release(1);
		REQUIRE(t.utilization() == Approx(1));
	}
}
/*
TEST_CASE("block()", "[Reservable]")
{
	GIVEN("no resources are reserved")
	{
		test_reservable t(10.0);

		SECTION("block full capacity")
		{
			REQUIRE_NOTHROW(t.block());
			REQUIRE(t.utilization() == Approx(10.0));
			REQUIRE(t.residual() == Approx(0.0));
		}
	}

	GIVEN("resources are reserved")
	{
		test_reservable t(10.0);
		t.reserve(5.0);

		SECTION("block current reservation")
		{
			REQUIRE_NOTHROW(t.block());
			REQUIRE(t.utilization() == Approx(5.0));
			REQUIRE(t.residual() == Approx(5.0));
		}
	}
}
*/
	TEST_CASE("Reservable::block(double w)", "[Reservable]")
	{
		SECTION("w must be >= 0") {
			test_reservable t(10);
			REQUIRE_THROWS(t.block(0));
		}

		GIVEN("no resources are reserved") {
			SECTION("throws an exception when trying to block more than capacity") {
				test_reservable t(10);
				REQUIRE_THROWS(t.block(15));
				REQUIRE(t.utilization() == Approx(0));
				REQUIRE(t.residual() == Approx(10));
			}

			SECTION("blocks a fraction of the capacity") {
				test_reservable t(10);
//
//			REQUIRE(t.reservation() == 0);
				REQUIRE_NOTHROW(t.block(4));
//			REQUIRE(t.utilization() == Approx(8.0));
//			REQUIRE(t.residual() == Approx(2.0));
			}
		}

		GIVEN("resources are reserved") {
			test_reservable t(10);
			t.reserve(5);

			SECTION("throws an exception when trying to block more than is currently reserved") {
				REQUIRE_THROWS(t.block(8));
				REQUIRE(t.utilization() == Approx(0));
				REQUIRE(t.residual() == Approx(5));
			}

			SECTION("blocks a fraction of the reservation") {
				REQUIRE_NOTHROW(t.block(4.0));
				REQUIRE(t.utilization() == Approx(4.0));
				REQUIRE(t.reservation() == Approx(1.0));
				REQUIRE(t.residual() == Approx(5.0));
			}
		}
	}

	TEST_CASE("Reservable::block_all()", "[Reservable]")
	{
		GIVEN("there is a single resource type") {
			test_reservable t(10);

			GIVEN("no resources are reserved") {
				SECTION("no exception is thrown") {
					REQUIRE_NOTHROW(t.block_all());
				}
			}

			GIVEN("resources are reserved") {
				SECTION("blocks all reservations") {
					REQUIRE_NOTHROW(t.reserve(5));
					REQUIRE_NOTHROW(t.block_all());
					REQUIRE(t.utilization() == Approx(5));
				}
			}
		}

		GIVEN("there are multiple resource types") {
			test_reservable t{10, 5};

			GIVEN("no resources are reserved") {
				SECTION("no exception is thrown") {
					REQUIRE_NOTHROW(t.block_all());
				}
			}

			GIVEN("resources are reserved") {
				SECTION("blocks all reservations") {
					REQUIRE_NOTHROW(t.reserve(5, 0));
					REQUIRE_NOTHROW(t.reserve(2, 1));
					REQUIRE_NOTHROW(t.block_all());
					REQUIRE(t.utilization(0) == Approx(5));
					REQUIRE(t.utilization(1) == Approx(2));
				}
			}
		}
	}

	TEST_CASE("Reservable::reset_all()", "[Reservable]")
	{
		test_reservable t(10);

		GIVEN("no resources are reserved") {
			SECTION("does not throw an exception") {
				REQUIRE_NOTHROW(t.reset_all());
			}
		}

		GIVEN("resources are reserved") {
			t.reserve(5);

			SECTION("resets the reservation") {
				REQUIRE_NOTHROW(t.reset_all());
				REQUIRE(t.reservation() == Approx(0));
			}
		}
	}

