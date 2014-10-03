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

#include <deuceclient/bundle.h>

#include <string.h>

#if defined(__FreeBSD__) || defined(__NetBSD__) || \
	defined(__OpenBSD__) || defined(__DragonFly__)
#include <sys/endian.h>
#elif defined(__linux__)
#include <endian.h>
#elif defined(_MSC_VER)
#include <stdlib.h>

// assume little-endian
#define htobe16(x)	_byteswap_ushort(x)
#define htobe32(x)	_byteswap_ulong(x)

#endif

namespace rax
{
namespace deuceclient
{

inline
void pack_block_map_header(char* p, size_t n)
{
	*p++ = 0xde;
	auto n_be = htobe16(uint16_t(n));
	memcpy(p, &n_be, sizeof(n_be));
}

inline
void pack_block_info(char* p, size_t n, sha1_digest blockid)
{
	// length 40
	p = std::copy_n("\xd9\x28", 2, p);
	p = hashlib::detail::hexlify_to(blockid, p);

	*p++ = 0xc6;
	auto n_be = htobe32(uint32_t(n));
	memcpy(p, &n_be, sizeof(n_be));
}

callback bundle::get_serializer() const
{
	auto gptr = gbase();
	auto it = pos_.begin();
	size_t last_block_pos = 0;
	int state = 0;
	enum { map_header_written = 1, block_info_written = 2 };

	return [=](char* bp, size_t blen) mutable -> size_t
	    {
		BOOST_ASSERT_MSG(blen >= packed_size_of_block_info(),
		    "read buffer too small");

		auto p = bp;

		if (not (state & map_header_written))
		{
			pack_block_map_header(p, pos_.size());
			p += packed_size_of_block_map_header();
			state |= map_header_written;
		}

		for (; it != pos_.end(); ++it)
		{
			if (not (state & block_info_written))
			{
				if (blen - (p - bp) >=
				    packed_size_of_block_info())
				{
					pack_block_info(p, std::get<0>(*it) -
					    last_block_pos, std::get<1>(*it));
					last_block_pos = std::get<0>(*it);
					p += packed_size_of_block_info();
					state |= block_info_written;
				}
				else
					break;
			}

			auto len = std::min(blen - (p - bp),
			    size_t(egptr(*it) - gptr));

			p = std::copy_n(gptr, len, p);
			gptr += len;

			// finished current block
			if (gptr == egptr(*it))
				state &= ~block_info_written;
			else
				break;
		}

		return p - bp;
	    };
}

}
}
