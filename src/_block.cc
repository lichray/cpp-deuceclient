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
#include <deuceclient/exceptions.h>

using namespace httpverbs::keywords;

namespace rax
{
namespace deuceclient
{

void client::upload_block(stdex::string_view vaultname,
    stdex::string_view blockid, stdex::string_view data)
{
	auto hdrs = common_hdrs_;
	hdrs.add("Content-Type", "application/octet-stream");

	auto resp = httpverbs::put(url_for_block(vaultname, blockid),
	    std::move(hdrs), data_from(data));

	expecting_server_response(201, resp);
}

void client::download_block(stdex::string_view vaultname,
    stdex::string_view blockid, callback f)
{
	httpverbs::request req("GET", url_for_block(vaultname, blockid));
	req.headers = common_hdrs_;

	auto resp = req.perform(std::move(f));

	if (resp.status_code == 404)
		throw not_found();

	expecting_server_response(200, resp);
}

void client::delete_block(stdex::string_view vaultname,
    stdex::string_view blockid)
{
	auto resp = httpverbs::delete_(url_for_block(vaultname, blockid),
	    common_hdrs_);

	expecting_server_response(204, resp);
}

}
}
