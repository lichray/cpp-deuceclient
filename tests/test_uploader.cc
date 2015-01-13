#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "test_data.h"

#include <deuceclient/deuceclient.h>
#include <httpverbs/stream.h>

using namespace rax;
using namespace httpverbs::keywords;

struct byte_per_byte_bundle : deuceclient::bundle
{
	template <typename Reader>
	bool consume(Reader&& f)
	{
		auto len = std::forward<Reader>(f)(egptr(),
		    capacity() - size());

		while (len--)
			mark_new_block(1);

		return size() == capacity();
	}
};

TEST_CASE("split random data into bytes", "[deuce]")
{
	auto client = deuceclient::client("http://localhost:8080",
	    "sample_project_id");

	// bytes are fairly easy to dedup, so randomize vaults to
	// allow the test to run multiple times
	auto vault = client.create_vault("Alicization" +
	    get_random_text(4, "0123456789"));

	byte_per_byte_bundle bs;

	auto bu = deuceclient::make_file_uploader(5000,
	    [&](deuceclient::bundle& bn)
	    {
		REQUIRE(bn.serialized_size() <= 5000);
		vault.upload_bundle(bn);
	    });

	int target_file_size = 128;
	randomstream src(target_file_size);

	auto f = vault.make_file();
	auto file_size = bu.consume_all(f, bs, from_stream(src));

	bu.finish();

	REQUIRE(file_size == target_file_size);
	REQUIRE_NOTHROW(f.finalize_file(file_size));
}
