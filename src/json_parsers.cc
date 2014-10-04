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

#include <rapidjson/reader.h>
#include <rapidjson/error/en.h>

#include "json_parsers.h"

namespace rax
{
namespace deuceclient
{

using namespace rapidjson;

template <typename F>
struct string_list_handler :
	BaseReaderHandler<UTF8<>, string_list_handler<F>>
{
	explicit string_list_handler(F f) :
		in_array_(false),
		f_(std::move(f))
	{}

	bool StartArray()
	{
		if (in_array_)
			return false;
		else
		{
			in_array_ = true;
			return true;
		}
	}

	bool EndArray(SizeType)
	{
		return true;
	}

	bool String(char const* str, SizeType length, bool)
	{
		return in_array_ ? (f_(str, length), true) : false;
	}

	bool Default()
	{
		return false;
	}

private:
	bool in_array_;
	F f_;
};

template <typename F>
inline
auto make_string_list_handler(F f) -> string_list_handler<F>
{
	return string_list_handler<F>(std::move(f));
}

auto parse_list_of_sha1(stdex::string_view src) -> std::vector<sha1_digest>
{
	auto const digest_size = std::tuple_size<sha1_digest>::value;

	std::vector<sha1_digest> v;

	// estimate number of ids, where each id is hexadecimal length
	// + 2 double quotes and a comma.  Ensure no reallocation.
	v.reserve(src.size() / (digest_size * 2 + 3));

	auto h = make_string_list_handler([&](char const* p, size_t sz)
	  {
		v.push_back(hashlib::unhexlify<digest_size>(
		    stdex::string_view(p, sz)));
	  });

	StringStream ss(src.data());
	Reader reader;

	if (reader.Parse(ss, h))
		return v;
	else
		throw std::invalid_argument(GetParseError_En(
		    reader.GetParseErrorCode()));
}

}
}
