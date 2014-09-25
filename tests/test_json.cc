#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <deuceclient/block_arrangement.h>
#include "../src/json_parsers.h"

using namespace rax;

deuceclient::sha1_digest h1 = { "sakurasou no pet na" };
deuceclient::sha1_digest h2 = { "kanojo             " };

TEST_CASE("json to assign blocks")
{
	auto ba = deuceclient::block_arrangement();

	SECTION("no block")
	{
		REQUIRE(ba.text() == "[]");
	}

	SECTION("stable after clear")
	{
		ba.add(h1, 0);
		ba.clear();

		REQUIRE(ba.text() == "[]");
	}

	SECTION("with blocks")
	{
		char sample[] = "["
			"[\"73616b757261736f75206e6f20706574206e6100\",1],"
			"[\"6b616e6f6a6f2020202020202020202020202000\","
			"4294967297]]";

		ba.add(h1, 1);
		ba.add(h2, 4294967297LL);

		REQUIRE(ba.text() == sample);
		REQUIRE(ba.text() == sample);
	}
}

TEST_CASE("sha1 list parser")
{
	using namespace deuceclient;

	SECTION("empty list")
	{
		REQUIRE(parse_list_of_sha1("[]").empty());
	}

	SECTION("real list")
	{
		char s[] = "[ \"73616b757261736f75206e6f20706574206e6100\","
		    "\"6b616e6f6a6f2020202020202020202020202000\"]";
		auto v = parse_list_of_sha1(s);

		REQUIRE(v[0] == h1);
		REQUIRE(v[1] == h2);
	}

	SECTION("unaccepted")
	{
		REQUIRE_THROWS_AS(parse_list_of_sha1("{}"),
		    std::invalid_argument&);
		REQUIRE_THROWS_AS(parse_list_of_sha1("[[]]"),
		    std::invalid_argument&);
		REQUIRE_THROWS_AS(parse_list_of_sha1(
		    "\"73616b757261736f75206e6f20706574206e6100\""),
		    std::invalid_argument&);
		REQUIRE_THROWS_AS(parse_list_of_sha1("[1024]"),
		    std::invalid_argument&);
	}
}
