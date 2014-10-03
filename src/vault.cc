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

void vault::upload_block(sha1_digest blockid, stdex::string_view data)
{
	client_.upload_block(vaultname_, blockid, data);
}

void vault::download_block(sha1_digest blockid, callback f)
{
	client_.download_block(vaultname_, blockid, std::move(f));
}

void vault::delete_block(sha1_digest blockid)
{
	client_.delete_block(vaultname_, blockid);
}

void vault::upload_bundle(bundle& bs)
{
	client_.upload_bundle(vaultname_, bs);
}

file vault::make_file()
{
	return client_.make_file(vaultname_);
}

void vault::download_file(stdex::string_view fileid, callback f)
{
	client_.download_file(vaultname_, fileid, std::move(f));
}

void vault::delete_file(stdex::string_view fileid)
{
	client_.delete_file(vaultname_, fileid);
}

}
}
