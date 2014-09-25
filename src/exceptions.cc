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

#include <deuceclient/exceptions.h>

namespace rax
{
namespace deuceclient
{

template <int N, typename Int, typename BidirIt>
inline
void format_digits_backward(Int i, BidirIt it)
{
	if (N != 0)
	{
		*--it = i % 10 + '0';
		format_digits_backward<N != 0 ? N - 1 : 0>(i / 10, it);
	}
}

inline
std::string errmsg_from_code(int n)
{
	if (not (100 <= n and n < 1000))
		return "invalid status code";

	std::string s = "response [   ]";
	format_digits_backward<3>(n, end(s) - 1);

	return s;
}

unexpected_server_response::unexpected_server_response(int status_code) :
	std::runtime_error(errmsg_from_code(status_code)),
	status_code_(status_code)
{}

not_found::not_found() :
	error("resource not exist")
{}

cannot_delete::cannot_delete() :
	error("resource in use")
{}

}
}
