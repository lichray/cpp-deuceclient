#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "test_data.h"

#include <deuceclient/deuceclient.h>
#include <httpverbs/stream.h>

#include <boost/foreach.hpp>

using namespace rax;
using namespace httpverbs::keywords;

TEST_CASE("split random data with rabin boundary", "[deuce]")
{
	auto client = deuceclient::client("http://localhost:8080",
	    "sample_project_id");
	auto vault = client.create_vault("GGO");

	deuceclient::managed_bundle<rabin_boundary> bs(1024 * 1024);
	deuceclient::block_arrangement ba;

	int target_file_size = (2 * 1024 - 100) * 1024;
	randomstream src(target_file_size);

	bs.boundary().set_limits(14843, 17432, 90406);

	int64_t file_size = 0;
	bool bundle_is_full;

	do
	{
		bundle_is_full = bs.consume(from_stream(src));

		if (bs.size() == 0)
			break;

		int64_t offset = file_size;

		BOOST_FOREACH(auto&& t, bs.blocks())
		{
			size_t end_of_block;
			deuceclient::sha1_digest blockid;
			std::tie(end_of_block, blockid) = t;

			ba.add(blockid, offset);
			offset = file_size + end_of_block;
		}

		file_size += bs.size();
		vault.upload_bundle(bs);

	} while (bundle_is_full);

	REQUIRE(file_size == target_file_size);

	auto f = vault.make_file();
	auto v = f.assign_blocks(ba);

	REQUIRE(v.empty());
	REQUIRE_NOTHROW(f.finalize_file(file_size));
}
