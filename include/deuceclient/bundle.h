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
#include <cerrno>
#include <system_error>

#include <boost/assert.hpp>

namespace rax
{
namespace deuceclient
{

typedef std::function<size_t(char*, size_t)>	callback;

struct bundle
{
private:
	typedef std::tuple<size_t, sha1_digest>	block_info;

public:
	bundle();

	bool empty() const;
	size_t size() const;
	size_t max_size() const;

	void clear();

	auto blocks() const -> std::vector<block_info> const&;

	size_t serialized_size() const;
	callback get_serializer() const;

protected:
	void mark_new_block(size_t blocksize);

	char* gbase() const;
	char* egptr() const;
	char* egptr(block_info const& info) const;

private:
	static size_t packed_size_of_block_map_header();
	static size_t packed_size_of_block_info();

	std::unique_ptr<char[]> buf_;
	std::vector<block_info> pos_;
};

struct unmanaged_bundle : bundle
{
	template <typename StringLike>
	void add_block(StringLike const& data)
	{
		BOOST_ASSERT_MSG((max_size() - size()) >= data.size(),
		    "bundle size overflow");

		std::copy_n(data.data(), data.size(), egptr());
		mark_new_block(data.size());
	}
};

template <typename Algorithm>
struct managed_bundle : bundle
{
	managed_bundle() :
		pptr_(pbase()),
		epptr_(pbase()),
		needs_reset_(false)
	{}

	template <typename Reader>
	bool consume(Reader reader)
	{
		std::error_code ec;
		auto is_full = consume(std::move(reader), ec);

		if (ec)
			throw std::system_error(ec);

		return is_full;
	}

	template <typename Reader>
	bool consume(Reader reader, std::error_code& ec)
	{
		if (needs_reset_)
		{
			epptr_ = std::move(pptr_, epptr_, pbase());
			pptr_ = pbase();
			needs_reset_ = false;
		}

		BOOST_ASSERT_MSG(not buffer_is_full(),
		    "buffer size overflow");

		auto len = reader(epptr_, unused_blen());

		if (len < 0)
		{
			ec.assign(errno, std::system_category());
			return false;
		}

		epptr_ += len;
		pptr_ = split_into_blocks(pptr_, epptr_,
		    size_t(len) < unused_blen());

		return needs_reset_ = buffer_is_full();
	}

	Algorithm& boundary()
	{
		return algo_;
	}

private:
	size_t unused_blen() const
	{
		return max_size() - (epptr_ - gbase());
	}

	bool buffer_is_full() const
	{
		return unused_blen() == 0;
	}

	char* split_into_blocks(char* first, char* last, bool reached_eof)
	{
		auto lbp = first;

		while (first != last)
		{
			algo_.process_byte(*first++);
			auto current_size = first - lbp;

			if ((first == last and reached_eof) or
			    algo_.reached_boundary(current_size))
			{
				mark_new_block(current_size);
				lbp = first;
				algo_.reset();
			}
		}

		return lbp;
	}

	char* pbase() const
	{
		return gbase();
	}

	char* pptr_;
	char* epptr_;
	bool needs_reset_;
	Algorithm algo_;
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
auto bundle::blocks() const -> std::vector<block_info> const&
{
	return pos_;
}

inline
void bundle::mark_new_block(size_t blocksize)
{
	pos_.push_back(std::make_tuple(size() + blocksize,
	    hashlib::sha1(egptr(), blocksize).digest()));
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
