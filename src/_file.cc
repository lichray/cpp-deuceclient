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

namespace rax
{
namespace deuceclient
{

inline
stdex::string_view last_component(stdex::string_view url)
{
	auto it = std::find(url.rbegin(), url.rend(), '/');
	return url.substr(url.rend() - it);
}

file client::make_file(stdex::string_view vaultname)
{
	auto resp = httpverbs::post(url_for_vault(vaultname) + "/files",
	    common_hdrs_);

	expecting_server_response(201, resp);

	// issuing no HTTP request
	return get_file(vaultname, last_component(resp.headers["location"]));
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

}
}
