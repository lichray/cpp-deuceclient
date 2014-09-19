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

vault client::create_vault(stdex::string_view name)
{
	auto hdrs = common_hdrs_;
	hdrs.add("Content-Type", "application/json");

	auto resp = httpverbs::put(url_for_vault(name), std::move(hdrs));

	expecting_server_response(201, resp);

	return vault(name.to_string(), *this);
}

vault client::get_vault(stdex::string_view name)
{
	auto resp = httpverbs::get(url_for_vault(name), common_hdrs_);

	if (resp.status_code == 404)
		throw not_found();

	expecting_server_response(200, resp);

	return vault(name.to_string(), *this);
}

void client::delete_vault(stdex::string_view name)
{
	do_delete(url_for_vault(name));
}

}
}
