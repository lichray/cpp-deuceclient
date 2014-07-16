#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <rapidjson/document.h>
#include <httpverbs/httpverbs.h>
#include <stdex/string_view.h>

TEST_CASE("httpverbs and libcurl")
{
	auto r = httpverbs::get("http://deuce.apiary-mock.com/v1.0/1");

	// XXX must be 200
	CHECK(r.status_code == 204);
}

TEST_CASE("rapidjson")
{
	using namespace rapidjson;

	Document doc;
	doc.Parse<0>("[42, true]");

	REQUIRE(doc.IsArray());
	REQUIRE(doc.Size() == 2);
}

TEST_CASE("string_view")
{
	stdex::string_view s("ni\0ce", 5);
	auto s2 = s;

	REQUIRE(s == s2);
	REQUIRE(s == s.to_string());
}
