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

#ifndef RAX_DEUCECLIENT_CLIENT_H
#define RAX_DEUCECLIENT_CLIENT_H

#include "vault.h"
#include "file.h"

#include <httpverbs/header_dict.h>

namespace rax
{
namespace deuceclient
{

struct client
{
	explicit client(std::string host) :
		prefix_(std::move(host + "/v1.0/vaults/"))
	{
		// XXX should be get from identity service
		common_hdrs_.add("X-Project-ID: sample_project_id");
	}

	vault create_vault(stdex::string_view name);
	vault get_vault(stdex::string_view name);
	void delete_vault(stdex::string_view name);

	file get_file(std::string vaultname, std::string fileid)
	{
		return file(std::move(vaultname), std::move(fileid), *this);
	}

private:
	std::string prefix_;
	httpverbs::header_dict common_hdrs_;
};

}
}

#endif
