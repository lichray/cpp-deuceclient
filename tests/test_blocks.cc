#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "test_data.h"

#include <deuceclient/deuceclient.h>

using namespace rax;

TEST_CASE("bundle upload", "[deuce]")
{
	auto client = deuceclient::client("http://localhost:8080",
	    "sample_project_id");
	auto vault = client.create_vault("log-horizon");

	deuceclient::unmanaged_bundle bs;
	deuceclient::block_arrangement ba;

	// empty bundle
	REQUIRE_NOTHROW(vault.upload_bundle(bs));

	// random blocks
	int64_t offset = 0;

	for (int n = 0; n < 10; ++n)
	{
		auto blk = get_random_block();

		bs.add_block(blk);

		size_t end_of_block;
		deuceclient::sha1_digest blockid;
		std::tie(end_of_block, blockid) = bs.blocks().back();

		ba.add(blockid, offset);
		offset = end_of_block;
	}

	auto sz = bs.size();

	vault.upload_bundle(bs);

	// cleared after succeed
	REQUIRE(bs.empty());

	auto f = vault.make_file();
	f.assign_blocks(ba);

	REQUIRE_NOTHROW(f.finalize_file(sz));
}
