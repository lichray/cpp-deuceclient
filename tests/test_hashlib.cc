#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "test_data.h"

#include <deuceclient/hashlib.h>

TEST_CASE("hexadecimal")
{
	SECTION("round trip")
	{
		hashlib::sha1 h;

		REQUIRE(h.digest() == hashlib::unhexlify
		    <hashlib::sha1::digest_size>(h.hexdigest()));
	}

	SECTION("malformed hex")
	{
		REQUIRE_NOTHROW(hashlib::unhexlify<1>("ef"));
		REQUIRE_THROWS_AS(hashlib::unhexlify<1>("EF"),
		    std::invalid_argument&);
		REQUIRE_THROWS_AS(hashlib::unhexlify<1>("eac"),
		    std::invalid_argument&);
	}
}

TEST_CASE("SHA1")
{
#define SRC_LITERAL "natsu no ame"
	SECTION("zero-length string")
	{
		REQUIRE(hashlib::sha1("") == hashlib::sha1());
		REQUIRE(hashlib::sha1("") != hashlib::sha1("", 1));
	}

	SECTION("null-terminated")
	{
		auto h = sha1(SRC_LITERAL).hexdigest();
		auto h1 = hashlib::sha1(SRC_LITERAL).hexdigest();
		auto h2 = hashlib::sha1(std::string(SRC_LITERAL)).hexdigest();

		REQUIRE(h1 == h);
		REQUIRE(h2 == h);
	}

	SECTION("embedded null")
	{
		std::array
		<
		    char const, sizeof(SRC_LITERAL)
		> arr = { SRC_LITERAL };

		auto h = sha1(arr).hexdigest();
		auto h1 = hashlib::sha1(arr).hexdigest();
		auto h2 = hashlib::sha1(SRC_LITERAL,
		    sizeof(SRC_LITERAL)).hexdigest();

		REQUIRE(h1 == h);
		REQUIRE(h2 == h);
	}

	SECTION("updating")
	{
		std::string part1 = SRC_LITERAL;
		std::string part2 = "\nyour diary";

		sha1 hobj;
		hashlib::sha1 hobj1;

		hobj.update(part1);
		hobj1.update(part1);

		REQUIRE(hobj1.hexdigest() == sha1(part1).hexdigest());

		hobj.update(part2);
		hobj1.update(part2);

		REQUIRE(hobj1.hexdigest() == hobj.hexdigest());
	}

	SECTION("stream output")
	{
		std::stringstream s;
		hashlib::sha1 h;

		using namespace std;
		s << left << setw(42) << setfill('_') << h;

		REQUIRE(s.str() == h.hexdigest() + "__");
	}
}

TEST_CASE("others")
{
	SECTION("SHA256 zero-length string")
	{
		REQUIRE(hashlib::sha256("").hexdigest() == "e3b0c44298fc1c149a"
		    "fbf4c8996fb92427ae41e4649b934ca495991b7852b855");
	}

	SECTION("SHA512 zero-length string")
	{
		REQUIRE(hashlib::sha512("").hexdigest() == "cf83e1357eefb8bdf1"
		    "542850d66d8007d620e4050b5715dc83f4a921d36ce9ce47d0d13c5d8"
		    "5f2b0ff8318d2877eec2f63b931bd47417a81a538327af927da3e");
	}

	SECTION("MD5 zero-length string")
	{
		REQUIRE(hashlib::md5("").hexdigest() == "d41d8cd98f00b204e9800"
		    "998ecf8427e");
	}
}
