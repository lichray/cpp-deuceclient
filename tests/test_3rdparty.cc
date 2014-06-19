#define CATCH_CONFIG_MAIN
#include <catch/catch.hpp>

#include <rapidjson/document.h>
#include <restclient/restclient.h>
#include <stdex/string_view.h>

TEST_CASE("restclient and libcurl")
{
	RestClient::response r = RestClient::get(
	  "http://http-test-server.heroku.com");

	CHECK(r.body == "GET");
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
