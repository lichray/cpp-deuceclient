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

namespace rax
{
namespace deuceclient
{

void client::do_download(std::string&& url, callback&& f)
{
	httpverbs::request req("GET", std::move(url));
	req.headers = common_hdrs_;

	auto resp = req.allow_redirects().perform(std::move(f));

	expecting_server_response(200, resp);
}

void client::do_delete(std::string&& url)
{
	httpverbs::request req("DELETE", std::move(url));
	req.headers = common_hdrs_;

	auto resp = req.perform();

	expecting_server_response(204, resp);
}

}
}
