#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "test_data.h"

#include <deuceclient/deuceclient.h>

using namespace rax;

inline
std::string get_file_content(deuceclient::vault& vault, deuceclient::file& f)
{
	std::string s;
	vault.download_file(f.id(), [&](char* p, size_t n) -> size_t
	    {
		s.append(p, n);
		return n;
	    });

	return s;
}

TEST_CASE("fill file content", "[deuce]")
{
	auto client = deuceclient::client("http://localhost:8080",
	    "sample_project_id");
	auto vault = client.create_vault("zone-alone");

	// empty file
	{
		auto f = vault.make_file();

		f.finalize_file(0);

		REQUIRE(get_file_content(vault, f).empty());
		REQUIRE_NOTHROW(vault.delete_file(f.id()));
	}

	// text file
	{
		auto b1 = get_random_text(500);
		auto b2 = get_random_text(1000);

		auto h1 = hashlib::sha1(b1).digest();
		auto h2 = hashlib::sha1(b2).digest();

		auto f = vault.make_file();

		// upload 1st block before assigning
		vault.upload_block(h1, b1);

		deuceclient::block_arrangement ba;
		ba.add(h1, 0);
		ba.add(h2, 500);

		// get 1 missing block
		auto v = f.assign_blocks(ba);

		REQUIRE(v.size() == 1);
		REQUIRE(v[0] == h2);

		// upload the 2nd block
		vault.upload_block(h2, b2);

		// cleared after succeed
		REQUIRE(ba.text() == "[]");

		REQUIRE_NOTHROW(f.finalize_file(1500));

		// download and compare the files
		auto s = get_file_content(vault, f);

		REQUIRE(s == b1 + b2);

		REQUIRE_NOTHROW(vault.delete_file(f.id()));
	}
}
