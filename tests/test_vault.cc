#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <deuceclient/client.h>

using namespace rax;

TEST_CASE("vault CRUD", "[deuce]")
{
	auto client = deuceclient::client("http://localhost:8080");

	SECTION("create and read")
	{
		auto v1 = client.create_vault("area11");
		auto v2 = client.get_vault("area11");

		REQUIRE(v1 == v2);
	}

	SECTION("delete")
	{
		REQUIRE_NOTHROW(client.delete_vault("area11"));
	}
}
