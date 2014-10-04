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

#ifndef _RAX_DEUCECLIENT_JSON__PARSERS_H
#define _RAX_DEUCECLIENT_JSON__PARSERS_H

#include <deuceclient/file.h>

namespace rax
{
namespace deuceclient
{

auto parse_list_of_sha1(stdex::string_view src) -> std::vector<sha1_digest>;

}
}

#endif