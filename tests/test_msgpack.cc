#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "test_data.h"

#include <deuceclient/bundle.h>

using namespace rax;

TEST_CASE("msgpack of blocks")
{
	char buf[60];
	auto bs = deuceclient::unmanaged_bundle();

	REQUIRE(bs.empty());
	REQUIRE(bs.get_serializer()(buf, sizeof(buf)) == bs.serialized_size());

	bs.add_block(stdex::string_view(""));
	bs.add_block(get_random_text(100));
	bs.add_block(stdex::string_view("portable"));

	REQUIRE(bs.size() == 108);

	std::array<size_t, 3> lengths = {{ 47, 48, 60 }};
	std::string last_result;

	// compare serializing results using different buffer sizes

	for (size_t i = 0; i < lengths.size(); ++i)
	{
		std::string s;
		auto f = bs.get_serializer();

		for (size_t n = 0; n < bs.serialized_size();)
		{
			auto len = f(buf, lengths[i]);
			s.append(buf, len);
			n += len;
		}

		REQUIRE(s.size() == bs.serialized_size());

		if (i > 0)
			REQUIRE(s == last_result);

		last_result = std::move(s);
	}

	bs.clear();

	REQUIRE(bs.empty());
	REQUIRE(bs.get_serializer()(buf, sizeof(buf)) == bs.serialized_size());
}
