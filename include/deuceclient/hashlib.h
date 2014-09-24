/*
 * Copyright 2014 Rackspace, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef RAX_DEUCECLIENT_HASHLIB_H
#define RAX_DEUCECLIENT_HASHLIB_H

#include <openssl/sha.h>

#include <array>
#include <string>
#include <algorithm>
#include <iterator>
#include <cstring>
#include <stdexcept>
#include <stdex/string_view.h>

namespace hashlib
{

namespace detail
{

struct sha1_provider
{
	typedef SHA_CTX context_type;
	static const size_t digest_size = SHA_DIGEST_LENGTH;

	static
	int init(context_type* ctx)
	{
		return SHA1_Init(ctx);
	}

	static
	int update(context_type* ctx, void const* data, size_t len)
	{
		return SHA1_Update(ctx, data, len);
	}

	static
	int final(unsigned char* md, context_type* ctx)
	{
		return SHA1_Final(md, ctx);
	}
};

template <size_t N, typename OutIt>
inline
void hexlify_to(std::array<unsigned char, N> md, OutIt it)
{
	auto half_to_hex = [](int c)
	{
		// does not work if the source encoding is not
		// ASCII-compatible
		return (c > 9) ? c + 'a' - 10 : c + '0';
	};

	std::for_each(begin(md), end(md), [&](unsigned char c)
	    {
		*it = half_to_hex((c >> 4) & 0xf);
		++it;
		*it = half_to_hex(c & 0xf);
		++it;
	    });
}

// only accept lower case hexadecimal
template <size_t N, typename OutIt>
inline
void unhexlify_to(stdex::string_view hs, OutIt first)
{
	auto hex_to_half = [](char c)
	{
		// does not work if the source encoding is not
		// ASCII-compatible
		if ('0' <= c and c <= '9')
			return c - '0';
		if ('a' <= c and c <= 'f')
			return c - 'a' + 10;

		throw std::invalid_argument("not a hexadecimal");
	};

	if (hs.size() != N * 2)
		throw std::invalid_argument("unexpected hexadecimal length");

	auto it = begin(hs);

	std::generate_n(first, N, [&]() -> int
	  {
		auto v = hex_to_half(*it) << 4;
		++it;
		v ^= hex_to_half(*it);
		++it;

		return v;
	  });
}

}

template <size_t N>
inline
std::string hexlify(std::array<unsigned char, N> md)
{
	std::string s;
	s.resize(N * 2);
	detail::hexlify_to(md, begin(s));

	return s;
}

template <size_t N>
inline
auto unhexlify(stdex::string_view hs) -> std::array<unsigned char, N>
{
	std::array<unsigned char, N> md;
	detail::unhexlify_to<N>(hs, begin(md));

	return md;
}

template <typename HashProvider>
struct hasher
{
	typedef typename HashProvider::context_type	context_type;
	static const size_t digest_size = HashProvider::digest_size;
	typedef std::array<unsigned char, digest_size>	digest_type;

	hasher()
	{
		HashProvider::init(&ctx_);
	}

	explicit hasher(char const* s)
	{
		HashProvider::init(&ctx_);
		update(s);
	}

	explicit hasher(char const* s, size_t n)
	{
		HashProvider::init(&ctx_);
		update(s, n);
	}

	template <typename StringLike>
	explicit hasher(StringLike const& bytes)
	{
		HashProvider::init(&ctx_);
		update(bytes);
	}

	void update(char const* s)
	{
		update(s, strlen(s));
	}

	void update(char const* s, size_t n)
	{
		HashProvider::update(&ctx_, s, n);
	}

	template <typename StringLike>
	void update(StringLike const& bytes)
	{
		update(bytes.data(), bytes.size());
	}

	auto digest() const -> digest_type
	{
		digest_type md;
		auto tmp_ctx = ctx_;

		HashProvider::final(md.data(), &tmp_ctx);

		return md;
	}

	auto hexdigest() const -> std::string
	{
		return hexlify(digest());
	}

private:
	context_type ctx_;
};

template <typename HashProvider>
inline
bool operator==(hasher<HashProvider> const& a, hasher<HashProvider> const& b)
{
	return a.digest() == b.digest();
}

template <typename HashProvider>
inline
bool operator!=(hasher<HashProvider> const& a, hasher<HashProvider> const& b)
{
	return !(a == b);
}

typedef hasher<detail::sha1_provider> sha1;

}

#endif
