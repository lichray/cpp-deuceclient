#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "test_data.h"

#include <deuceclient/hashlib.h>

TEST_CASE("SHA1")
{
#define SRC_LITERAL "natsu no ame"
	SECTION("zero-length string")
	{
		REQUIRE(hashlib::sha1("") == hashlib::sha1());
		REQUIRE(hashlib::sha1("") != hashlib::sha1("", 1));
	}

	SECTION("null-terminated")
	{
		auto h = sha1(SRC_LITERAL).hexdigest();
		auto h1 = hashlib::sha1(SRC_LITERAL).hexdigest();
		auto h2 = hashlib::sha1(std::string(SRC_LITERAL)).hexdigest();

		REQUIRE(h1 == h);
		REQUIRE(h2 == h);
	}

	SECTION("embedded null")
	{
		std::array
		<
		    char const, sizeof(SRC_LITERAL)
		> arr = { SRC_LITERAL };

		auto h = sha1(arr).hexdigest();
		auto h1 = hashlib::sha1(arr).hexdigest();
		auto h2 = hashlib::sha1(SRC_LITERAL,
		    sizeof(SRC_LITERAL)).hexdigest();

		REQUIRE(h1 == h);
		REQUIRE(h2 == h);
	}

	SECTION("updating")
	{
		std::string part1 = SRC_LITERAL;
		std::string part2 = "\nyour diary";

		sha1 hobj;
		hashlib::sha1 hobj1;

		hobj.update(part1);
		hobj1.update(part1);

		REQUIRE(hobj1.hexdigest() == sha1(part1).hexdigest());

		hobj.update(part2);
		hobj1.update(part2);

		REQUIRE(hobj1.hexdigest() == hobj.hexdigest());
	}
}
