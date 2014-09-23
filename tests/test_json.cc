#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <deuceclient/block_arrangement.h>

using namespace rax;

TEST_CASE("json to assign blocks")
{
	auto ba = deuceclient::block_arrangement();

	SECTION("no block")
	{
		REQUIRE(ba.text() == "[]");
	}

	SECTION("stable after clear")
	{
		ba.add("eaf", 0);
		ba.clear();

		REQUIRE(ba.text() == "[]");
	}

	SECTION("with blocks")
	{
		char sample[] = "[[\"blk1\",1],[\"blk2\",4294967297]]";

		ba.add("blk1", 1);
		ba.add("blk2", 4294967297LL);

		REQUIRE(ba.text() == sample);
		REQUIRE(ba.text() == sample);
	}
}
