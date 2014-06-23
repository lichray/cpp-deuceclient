#define CATCH_CONFIG_MAIN
#include <catch/catch.hpp>
#include <type_traits>

#include <deuceclient/client.h>

using namespace rax;

TEST_CASE("client")
{
	deuceclient::client cdeuce("http://deuce.apiary-mock.com");

	auto vt = cdeuce.get_vault("empty");
	auto vf = cdeuce.get_file("empty", "9ea72e0");
	auto vf2 = vt.get_file("9ea72e0");

	REQUIRE(vf2 == vf);
}
