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

#include <deuceclient/client.h>

namespace rax
{
namespace deuceclient
{

std::string file::url() const
{
	return client_.url_for_file(vaultname_, fileid_);
}

auto file::assign_blocks(block_arrangement& ba) -> std::vector<sha1_digest>
{
	return client_.assign_blocks(vaultname_, fileid_, ba);
}

void file::finalize_file(int64_t len)
{
	client_.finalize_file(vaultname_, fileid_, len);
}

}
}
