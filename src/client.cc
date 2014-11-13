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

#include "config.h"

#if defined(USE_BOOST_THREAD)
#include <boost/thread/thread.hpp>
#else
#ifndef _GLIBCXX_USE_NANOSLEEP
#define _GLIBCXX_USE_NANOSLEEP
#endif
#include <thread>
#endif

namespace rax
{
namespace deuceclient
{

void client::do_download(std::string&& url, callback&& f)
{
	httpverbs::request req("GET", std::move(url));
	req.headers = common_hdrs_;
	req.allow_redirects();

	get_response<200>([&] { return req.perform(f); });
}

void client::do_delete(std::string&& url)
{
	httpverbs::request req("DELETE", std::move(url));
	req.headers = common_hdrs_;

	get_response<204>([&] { return req.perform(); });
}

void client::do_authenticate()
{
	common_hdrs_.set("X-Auth-Token", auth_());
}

void client::do_sleep(int n)
{
#if defined(USE_BOOST_THREAD)
	boost::this_thread::sleep(boost::posix_time::seconds(n));
#else
	std::this_thread::sleep_for(std::chrono::seconds(n));
#endif
}

}
}
