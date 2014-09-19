#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "test_data.h"

#include <deuceclient/deuceclient.h>

using namespace rax;

TEST_CASE("vault CRUD", "[deuce]")
{
	auto client = deuceclient::client("http://localhost:8080",
	    "sample_project_id");

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

TEST_CASE("block CRUD", "[deuce]")
{
	auto client = deuceclient::client("http://localhost:8080",
	    "sample_project_id");
	auto vault = client.create_vault("tatsuta-gawa");

	{
		auto txt = get_random_text(2000);
		auto h = hashlib::sha1(txt).hexdigest();

		REQUIRE_NOTHROW(vault.upload_block(h, txt));

		std::string s;
		vault.download_block(h, [&](char* p, size_t n) -> size_t
		    {
			s.append(p, n);
			return n;
		    });

		REQUIRE(s == txt);
		REQUIRE_NOTHROW(vault.delete_block(h));
	}
}

TEST_CASE("file CRUD", "[deuce]")
{
	auto client = deuceclient::client("http://localhost:8080",
	    "sample_project_id");
	auto vault = client.create_vault("tsukubane");

	{
		auto f = vault.make_file();
		auto f1 = vault.get_file(f.id());

		REQUIRE(f1 == f);
		REQUIRE_NOTHROW(vault.delete_file(f.id()));
	}
}
