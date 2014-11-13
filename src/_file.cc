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
#include <rapidjson/internal/itoa.h>

#include <deuceclient/client.h>
#include <deuceclient/exceptions.h>

#include "json_parsers.h"

using namespace httpverbs::keywords;

namespace rax
{
namespace deuceclient
{

file client::make_file(stdex::string_view vaultname)
{
	httpverbs::request req("POST", url_for_vault(vaultname) + "/files");
	req.headers = common_hdrs_;

	auto resp = req.perform();

	expecting_server_response(201, resp);

	return file(vaultname.to_string(), resp.headers["x-file-id"], *this);
}

auto client::assign_blocks(stdex::string_view vaultname,
    stdex::string_view fileid, block_arrangement& ba)
	-> std::vector<sha1_digest>
{
	httpverbs::request req("POST", url_for_file(vaultname, fileid) +
	    "/blocks");
	req.headers = common_hdrs_;
	req.headers.add("Content-Type", "application/json");

	auto resp = req.perform(data_from(ba.text()));

	expecting_server_response(200, resp);
	ba.clear();

	return parse_list_of_sha1(std::move(resp.content));
}

void client::finalize_file(stdex::string_view vaultname,
    stdex::string_view fileid, int64_t len)
{
	httpverbs::request req("POST", url_for_file(vaultname, fileid));
	req.headers = common_hdrs_;

	char buf[22];
	*rapidjson::internal::i64toa(len, buf) = '\0';
	req.headers.add("X-File-Length", buf);

	auto resp = req.perform();

	expecting_server_response(200, resp);
}

}
}
