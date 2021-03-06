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

#include <httpverbs/httpverbs.h>

#include <deuceclient/client.h>

using namespace httpverbs::keywords;

namespace rax
{
namespace deuceclient
{

void client::upload_block(stdex::string_view vaultname, sha1_digest blockid,
    stdex::string_view data)
{
	httpverbs::request req("PUT", url_for_block(vaultname, blockid));

	get_response<201>([&]() -> httpverbs::response
	    {
		req.headers = common_hdrs_;
		req.headers.add("Content-Type", "application/octet-stream");

		return req.perform(data_from(data));
	    });
}

void client::upload_bundle(stdex::string_view vaultname, bundle& bs)
{
	httpverbs::request req("POST", url_for_vault(vaultname) + "/blocks");

	get_response<201>([&]() -> httpverbs::response
	    {
		req.headers = common_hdrs_;
		req.headers.add("Content-Type", "application/msgpack");

		return req.perform(bs.serialized_size(), bs.get_serializer());
	    });

	bs.clear();
}

}
}
