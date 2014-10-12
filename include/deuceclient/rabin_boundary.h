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

#ifndef RAX_RABIN__BOUNDARY_H
#define RAX_RABIN__BOUNDARY_H

#include "rabin_fingerprint.h"

#include <boost/assert.hpp>

namespace rax
{

struct rabin_boundary
{
	rabin_boundary();

	void process_byte(unsigned char byte);
	bool reached_boundary(size_t blksize) const;
	void reset();

	void set_limits(size_t min, size_t average, size_t max);

private:
	static const uint64_t fingerprint_pt = 0xbfe6b8a5bf378d83;
	static const uint64_t break_value = 4;

	rabin_fingerprint<128> rabinfp_;
	size_t avg_;
	size_t min_;
	size_t max_;
};

inline
rabin_boundary::rabin_boundary() :
	rabinfp_(fingerprint_pt)
{}

inline
void rabin_boundary::set_limits(size_t min, size_t average, size_t max)
{
	BOOST_ASSERT_MSG(min < average and average < max,
	    "average not in (min. max)");

	avg_ = average;
	min_ = min;
	max_ = max;
}

inline
void rabin_boundary::process_byte(unsigned char byte)
{
	rabinfp_.process_byte(byte);
}

inline
bool rabin_boundary::reached_boundary(size_t blocksize) const
{
	return blocksize == max_ or
	    (blocksize >= min_ and
	     rabinfp_.value() % avg_ == break_value);
}

inline
void rabin_boundary::reset()
{
	rabinfp_.reset();
}

}

#endif
