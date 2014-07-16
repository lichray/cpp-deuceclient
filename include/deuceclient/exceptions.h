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

#ifndef RAX_DEUCECLIENT_EXCEPTIONS_H
#define RAX_DEUCECLIENT_EXCEPTIONS_H

#include <stdexcept>

namespace rax
{
namespace deuceclient
{

struct unexpected_server_response : std::runtime_error
{
	explicit unexpected_server_response(int status_code);

	int status_code() const
	{
		return status_code_;
	}

private:
	int const status_code_;
};

struct error : std::runtime_error
{
	error(char const* msg) :
		std::runtime_error(msg)
	{}
};

struct not_found : error
{
	not_found();
};

struct cannot_delete : error
{
	cannot_delete();
};

template <typename RespType>
inline void expecting_server_response(int status_code, RespType const& resp)
{
	if (resp.status_code != status_code)
		throw unexpected_server_response(resp.status_code);
}

}
}

#endif
