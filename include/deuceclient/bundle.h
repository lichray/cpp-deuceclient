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

#ifndef RAX_DEUCECLIENT_BUNDLE_H
#define RAX_DEUCECLIENT_BUNDLE_H

#include "block_arrangement.h"

#include <vector>
#include <tuple>
#include <functional>

#include <boost/assert.hpp>

namespace rax
{
namespace deuceclient
{

typedef std::function<size_t(char*, size_t)>	callback;

struct bundle
{
	bundle();

	bool empty() const;
	size_t size() const;
	size_t max_size() const;

	void add_block(sha1_digest blockid, stdex::string_view data);
	void clear();

	size_t serialized_size() const;
	callback get_serializer() const;

private:
	typedef std::tuple<size_t, sha1_digest>	block_info;

	char* gbase() const;
	char* egptr() const;
	char* egptr(block_info const& info) const;

	static size_t packed_size_of_block_map_header();
	static size_t packed_size_of_block_info();

	std::unique_ptr<char[]> buf_;
	std::vector<block_info> pos_;
};

inline
bundle::bundle() : buf_(new char[max_size()])
{}

inline
bool bundle::empty() const
{
	return size() == 0;
}

inline
size_t bundle::size() const
{
	return pos_.empty() ? 0 : std::get<0>(pos_.back());
}

inline
size_t bundle::max_size() const
{
	return 5 * 1024 * 1024;
}

inline
void bundle::clear()
{
	pos_.clear();
}

inline
void bundle::add_block(sha1_digest blockid, stdex::string_view data)
{
	BOOST_ASSERT_MSG((SIZE_MAX - size()) >= data.size(),
	    "bundle size overflow");

	data.copy(egptr(), data.size());
	pos_.push_back(std::make_tuple(size() + data.size(), blockid));
}

inline
char* bundle::gbase() const
{
	return buf_.get();
}

inline
char* bundle::egptr() const
{
	return gbase() + size();
}

inline
char* bundle::egptr(block_info const& info) const
{
	return gbase() + std::get<0>(info);
}

inline
size_t bundle::serialized_size() const
{
	return packed_size_of_block_map_header() +
	    packed_size_of_block_info() * pos_.size() + size();
}

inline
size_t bundle::packed_size_of_block_map_header()
{
	// (map 16)
	return 3;
}

inline
size_t bundle::packed_size_of_block_info()
{
	// (str 8) + hex + (bin 32)
	return 2 + (std::tuple_size<sha1_digest>::value * 2) + 5;
}

}
}

#endif