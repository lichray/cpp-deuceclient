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

#ifndef RAX_DEUCECLIENT_FILE_H
#define RAX_DEUCECLIENT_FILE_H

#include "clientfwd.h"
#include "block_arrangement.h"

#include <vector>
#include "string_view.h"

namespace rax
{
namespace deuceclient
{

struct file
{
	explicit file(std::string name, std::string fileid, client& handle) :
		client_(handle),
		vaultname_(std::move(name)),
		fileid_(std::move(fileid))
	{}

	stdex::string_view id() const;
	std::string url() const;

	auto assign_blocks(block_arrangement& ba) -> std::vector<sha1_digest>;
	void finalize_file(int64_t len);

	friend inline
	bool operator==(file const& a, file const& b)
	{
		return &a.client_ == &b.client_ and
		    a.vaultname_ == b.vaultname_ and
		    a.fileid_ == b.fileid_;
	}

	friend inline
	bool operator!=(file const& a, file const& b)
	{
		return !(a == b);
	}

private:
	client& client_;
	std::string vaultname_;
	std::string fileid_;
};

inline
stdex::string_view file::id() const
{
	return fileid_;
}

}
}

#endif
