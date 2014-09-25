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

inline
std::string last_component(std::string const& url)
{
	return url.substr(url.rfind('/') + 1);
}

file client::make_file(stdex::string_view vaultname)
{
	auto resp = httpverbs::post(url_for_vault(vaultname) + "/files",
	    common_hdrs_);

	expecting_server_response(201, resp);

	return file(vaultname.to_string(),
	    last_component(resp.headers["location"]), *this);
}

void client::download_file(stdex::string_view vaultname,
    stdex::string_view fileid, callback f)
{
	do_download(url_for_file(vaultname, fileid), std::move(f));
}

void client::delete_file(stdex::string_view vaultname,
    stdex::string_view fileid)
{
	do_delete(url_for_file(vaultname, fileid));
}

auto client::assign_blocks(stdex::string_view vaultname,
    stdex::string_view fileid, block_arrangement& ba)
	-> std::vector<sha1_digest>
{
	auto hdrs = common_hdrs_;
	hdrs.add("Content-Type", "application/json");

	// XXX should use PATCH
	auto resp = httpverbs::post(url_for_file(vaultname, fileid),
	    std::move(hdrs), data_from(ba.text()));

	expecting_server_response(200, resp);
	ba.clear();

	return parse_list_of_sha1(resp.content.data());
}

void client::finalize_file(stdex::string_view vaultname,
    stdex::string_view fileid, int64_t len)
{
	auto hdrs = common_hdrs_;
	char buf[22];
	*rapidjson::internal::i64toa(len, buf) = '\0';
	hdrs.add("X-File-Length", buf);

	// XXX should not require
	hdrs.add("Content-Length", "0");

	auto resp = httpverbs::post(url_for_file(vaultname, fileid),
	    std::move(hdrs));

	expecting_server_response(200, resp);
}

}
}
