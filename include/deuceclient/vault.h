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

#ifndef RAX_DEUCECLIENT_VAULT_H
#define RAX_DEUCECLIENT_VAULT_H

#include "clientfwd.h"
#include "file.h"

#include <functional>

namespace rax
{
namespace deuceclient
{

typedef std::function<size_t(char*, size_t)>	callback;

struct vault
{
	explicit vault(std::string name, client& handle) :
		client_(handle), vaultname_(std::move(name))
	{}

	void upload_block(stdex::string_view blockid, stdex::string_view data);
	void download_block(stdex::string_view blockid, callback);
	void delete_block(stdex::string_view blockid);

	file make_file();
	file get_file(stdex::string_view fileid);
	void download_file(stdex::string_view fileid, callback);
	void delete_file(stdex::string_view fileid);

	friend inline
	bool operator==(vault const& a, vault const& b)
	{
		return &a.client_ == &b.client_ and
		    a.vaultname_ == b.vaultname_;
	}

	friend inline
	bool operator!=(vault const& a, vault const& b)
	{
		return !(a == b);
	}

private:
	client& client_;
	std::string vaultname_;
};

}
}

#endif
