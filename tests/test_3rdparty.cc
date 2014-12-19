#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <rapidjson/document.h>
#include <httpverbs/httpverbs.h>

TEST_CASE("httpverbs and libcurl")
{
	auto r = httpverbs::get("http://private-anon-e9be20b4e-"
	    "deuce.apiary-mock.com/v1.0/vaults/1");

	CHECK(r.status_code == 200);
}

TEST_CASE("rapidjson")
{
	using namespace rapidjson;

	Document doc;
	doc.Parse<0>("[42, true]");

	REQUIRE(doc.IsArray());
	REQUIRE(doc.Size() == 2);
}
